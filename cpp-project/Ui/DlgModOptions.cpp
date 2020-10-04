/////////////////////////////////////////////////////////////////////
// File    : DlgModOptions.cpp
// Desc    :
// Created : Wednesday, September 12, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "DlgModOptions.h"

#include "RDNSimProxy.h"
#include "RDNUIOptions.h"
#include "RDNHUD.h"

#include "../ModObj.h"
#include "../RDNDllSetup.h"
#include "RDNText.h"

#include "../Simulation/RDNWorld.h"

#include <EngineAPI/RTSHud.h>
#include <EngineAPI/UIInterface.h>

namespace
{
	const char *SCREENNAME = "igmodoptions";

	const char *GROUPLIST = "hotkeygroups_list";
	const char *GROUPLISTCUSTOMITEM = "list1_customitem";
	const char *KEYLIST = "keybinding_list";
	const char *KEYLISTCUSTOMITEM = "list2_customitem";

	const size_t MAXNONMODIFIERKEYS = 1;

	/////////////////////////////////////////////////////////////////////
	// Desc.     :
	// Result    :
	// Param.    :
	// Author    :
	//
	size_t GetModifierKeyPriority(const char *modifierKey)
	{
		if (!strcmp(modifierKey, Plat::Input::GetKeyName(Plat::KEY_Control)))
			return 0;
		if (!strcmp(modifierKey, Plat::Input::GetKeyName(Plat::KEY_Alt)))
			return 1;
		if (!strcmp(modifierKey, Plat::Input::GetKeyName(Plat::KEY_Shift)))
			return 2;
		dbBreak();
		return 0;
	}

	/////////////////////////////////////////////////////////////////////
	// Desc.     :
	// Result    :
	// Param.    :
	// Author    :
	//
	bool IsModifierKey(const char *hotkey)
	{
		return !strcmp(hotkey, Plat::Input::GetKeyName(Plat::KEY_Control)) ||
					 !strcmp(hotkey, Plat::Input::GetKeyName(Plat::KEY_Shift)) ||
					 !strcmp(hotkey, Plat::Input::GetKeyName(Plat::KEY_Alt));
	}

	class SortHotKeyObj
	{
	public:
		bool operator()(const std::string &l, const std::string &r)
		{
			// left is a modifier key
			if (IsModifierKey(l.c_str()))
			{
				if (IsModifierKey(r.c_str()))
				{
					return GetModifierKeyPriority(l.c_str()) < GetModifierKeyPriority(r.c_str());
				}
				else
					return true; // left should precede right
			}
			// left is a modifier key
			else if (IsModifierKey(r.c_str()))
			{
				if (IsModifierKey(l.c_str()))
				{
					return GetModifierKeyPriority(l.c_str()) < GetModifierKeyPriority(r.c_str());
				}
				else
					return false; // right should precede left
			}
			else
			{
				return strcmp(l.c_str(), r.c_str()) < 0;
			}
		}
	};
} // namespace

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
DlgModOptions::DlgModOptions(RTSHud *h, const RDNSimProxy *proxy, RDNInputBinder *pInputBinder, RDNUIOptions *pRDNUIOptions, const CloseCB &cb)
		: m_hud(h),
			m_cbClose(cb),
			m_proxy(proxy),
			m_pInputBinder(pInputBinder),
			m_pRDNUIOptions(pRDNUIOptions),
			m_nonModifierKeyCount(0)
{
	// setup the dlg
	m_hud->SetButtonCB(SCREENNAME, "done_button", RTSHud::ButtonCallback::Bind(this, &DlgModOptions::OnClose));

	// hide warning and messages
	ShowKeyMessagePopup("", false);
	ShowWarning(L"", false);

	// init
	InitKeyGroupUI();

	//
	InitOptionsUI();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
DlgModOptions::~DlgModOptions()
{
	// hide the popup
	m_hud->PopupHide(SCREENNAME);

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
DlgModOptions *DlgModOptions::Create(RTSHud *h, const RDNSimProxy *proxy, RDNInputBinder *pInputBinder, RDNUIOptions *pRDNUIOptions, const CloseCB &cb)
{
	// validate parm
	dbAssert(h);
	dbAssert(cb.empty() == 0);

	// try to show popup
	if (h->PopupShow(SCREENNAME, RTSHud::InputCallback()) == 0)
		return 0;

	// create instance
	DlgModOptions *dlg = new DlgModOptions(h, proxy, pInputBinder, pRDNUIOptions, cb);

	// applies tootltips
	h->ApplyTooltipFile(SCREENNAME, "data:ui/screens/tooltips/igmodoptionstt.lua");

	return dlg;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void DlgModOptions::OnClose(const std::string &)
{
	// save updated keys
	m_pInputBinder->Save();

	SaveOptionsUI();

	//
	m_cbClose();
	// IMPORTANT NOTE: no member functions/vars must be used after this call

	return;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void DlgModOptions::InitKeyGroupUI(void)
{
	// add on item press/activated callbacks
	m_hud->SetOnItemChangeCB(SCREENNAME, GROUPLIST, RTSHud::ListCallback::Bind(this, &DlgModOptions::OnGroupItemChangeCB));
	m_hud->SetOnItemChangeCB(SCREENNAME, KEYLIST, RTSHud::ListCallback::Bind(this, &DlgModOptions::OnHotKeyItemChangeCB));
	m_hud->SetOnItemActivatedCB(SCREENNAME, KEYLIST, RTSHud::ListCallback::Bind(this, &DlgModOptions::OnHotKeyItemActivatedCB));

	// add button callbacks
	m_hud->SetButtonCB(SCREENNAME, "resetall_button", RTSHud::ButtonCallback::Bind(this, &DlgModOptions::OnRestoreAllKeysCB));
	m_hud->SetButtonCB(SCREENNAME, "resetdefaults_button0", RTSHud::ButtonCallback::Bind(this, &DlgModOptions::OnRestoreKeyCB));
	m_hud->SetButtonCB(SCREENNAME, "assignkey_button", RTSHud::ButtonCallback::Bind(this, &DlgModOptions::OnAssignKeyCB));

	// hide custom items
	m_hud->Show(SCREENNAME, GROUPLISTCUSTOMITEM, false);
	m_hud->Show(SCREENNAME, KEYLISTCUSTOMITEM, false);

	RefreshKeyGroupList(false, false);
	RefreshHotKeyList(false);

	// init lists to both have the first item selected
	m_hud->SetCustomListSelectedItem(SCREENNAME, GROUPLIST, 0);
	m_hud->SetCustomListSelectedItem(SCREENNAME, KEYLIST, 0);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void DlgModOptions::RefreshKeyGroupList(bool bKeepState, bool bHotKeyKeepState)
{
	// clear index table to zero

	m_groupIndexTable.resize(m_pInputBinder->GetGroupCount());
	std::fill(m_groupIndexTable.begin(), m_groupIndexTable.end(), 0);

	int sel = m_hud->GetCustomListSelectedItem(SCREENNAME, GROUPLIST);
	int keySel = m_hud->GetCustomListSelectedItem(SCREENNAME, KEYLIST);

	int pos = m_hud->GetListPos(SCREENNAME, GROUPLIST);
	int keyPos = m_hud->GetListPos(SCREENNAME, KEYLIST);

	m_hud->ClearCustomList(SCREENNAME, GROUPLIST);
	for (size_t i = 0; i < m_pInputBinder->GetGroupCount(); i++)
	{
		// add an item and then get the items name so we can fill it out
		const RDNInputBinder::KeyGroup *pGroup = m_pInputBinder->GetGroupAt(i);

		// only add non hidden groups
		if (pGroup->type == RDNInputBinder::KGT_GlobalLockedAndHidden)
			continue;

		// add group to group list
		int ndx = m_hud->AddCustomItemToCustomList(SCREENNAME, GROUPLIST, GROUPLISTCUSTOMITEM);

		// fill out group entry
		char itemName[128];
		if (m_hud->GetCustomListItemName(SCREENNAME, GROUPLIST, ndx, itemName, LENGTHOF(itemName)))
		{
			// create a string to access the text label
			char subItem[256];
			strcpy(subItem, itemName);
			strcat(subItem, ".list1_customitem.groupname_label");

			// set textlabel
			wchar_t buf[1024];
			Localizer::GetString(buf, LENGTHOF(buf), pGroup->locID);
			m_hud->SetText(SCREENNAME, subItem, buf);

			// create a string to show/hide, and set text for unassigned key hud
			strcpy(subItem, itemName);
			strcat(subItem, ".list1_customitem.icon_bitmaplabel");

			// set visibility
			m_hud->Show(SCREENNAME, subItem, m_pInputBinder->HasUnassignedKey(i));
			// set text
			Localizer::GetString(buf, LENGTHOF(buf), MODOPTIONS_KEYMISSING);
			m_hud->SetText(SCREENNAME, subItem, buf);

			// create a string to show/hide the lock icon
			strcpy(subItem, itemName);
			strcat(subItem, ".list1_customitem.locked_label");

			// set visibility
			m_hud->Show(SCREENNAME, subItem, (pGroup->type == RDNInputBinder::KGT_GlobalLocked));
		}

		// add entry to realindex table
		dbAssert(ndx < (int)m_groupIndexTable.size());
		m_groupIndexTable[ndx] = i;
	}

	if (!bKeepState)
	{
		sel = 0;
		pos = 0;
	}
	if (!bHotKeyKeepState)
	{
		keySel = 0;
		keyPos = 0;
	}

	m_hud->SetCustomListSelectedItem(SCREENNAME, GROUPLIST, -1);
	m_hud->SetCustomListSelectedItem(SCREENNAME, GROUPLIST, sel);
	m_hud->SetCustomListSelectedItem(SCREENNAME, KEYLIST, keySel);

	m_hud->SetListPos(SCREENNAME, GROUPLIST, pos);
	m_hud->SetListPos(SCREENNAME, KEYLIST, keyPos);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void DlgModOptions::RefreshHotKeyList(bool bKeepState)
{
	// convert from list index to group index
	int group = GetRealGroupIndex(m_hud->GetCustomListSelectedItem(SCREENNAME, GROUPLIST));

	if (group == -1)
		return;

	int sel = m_hud->GetCustomListSelectedItem(SCREENNAME, KEYLIST);
	int pos = m_hud->GetListPos(SCREENNAME, KEYLIST);

	// update current hotkeylist
	m_hud->ClearCustomList(SCREENNAME, KEYLIST);
	for (size_t key = 0; key < m_pInputBinder->GetHotKeyCount(group); key++)
	{
		// add item
		m_hud->AddCustomItemToCustomList(SCREENNAME, KEYLIST, KEYLISTCUSTOMITEM);

		// fill out item
		FillHotKeyItem(group, key);
	}

	if (!bKeepState)
	{
		sel = 0;
		pos = 0;
	}

	m_hud->SetCustomListSelectedItem(SCREENNAME, KEYLIST, sel);
	m_hud->SetListPos(SCREENNAME, KEYLIST, pos);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void DlgModOptions::OnGroupItemChangeCB(const std::string &, long)
{
	RefreshHotKeyList(false);
	ShowKeyMessagePopup("", false);
	ShowWarning(L"", false);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void DlgModOptions::OnHotKeyItemChangeCB(const std::string &, long)
{
	ShowKeyMessagePopup("", false);
	ShowWarning(L"", false);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void DlgModOptions::OnHotKeyItemActivatedCB(const std::string &, long)
{
	// delegate logic
	OnAssignKeyCB("");
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void DlgModOptions::OnAssignKeyCB(const std::string &)
{
	// convert from list index to group index
	int group = GetRealGroupIndex(m_hud->GetCustomListSelectedItem(SCREENNAME, GROUPLIST)),
			key = m_hud->GetCustomListSelectedItem(SCREENNAME, KEYLIST);
	if ((group != -1) && (key != -1))
	{
		// don't allow the user to modify locked keys
		const RDNInputBinder::KeyGroup *pGroup = m_pInputBinder->GetGroupAt(group);
		if (pGroup->type == RDNInputBinder::KGT_GlobalLocked)
		{
			// give user feedback
			HandleKeyResult(RDNInputBinder::SKR_Error_Locked);
			return;
		}

		const RDNInputBinder::HotKey *pHotkey = m_pInputBinder->GetHotKeyAt(group, key);
		if (!pHotkey)
			return;

		// add input callback
		m_hud->AddGlobalInputHook("DlgModOptions", RTSHud::InputCallback::Bind(this, &DlgModOptions::HookInput));

		ShowKeyMessagePopup(pHotkey->keyCombo.c_str(), true);
	}
	else
	{
		ShowKeyMessagePopup("", true);
	}
	ShowWarning(L"", false);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void DlgModOptions::OnRestoreAllKeysCB(const std::string &)
{
	if (m_pInputBinder->RestoreAllGroups())
	{
		RefreshKeyGroupList(true, true);
		RefreshHotKeyList(true);
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void DlgModOptions::OnRestoreKeyCB(const std::string &)
{
	// convert from list index to group index
	int group = GetRealGroupIndex(m_hud->GetCustomListSelectedItem(SCREENNAME, GROUPLIST)),
			key = m_hud->GetCustomListSelectedItem(SCREENNAME, KEYLIST);
	if ((group != -1) && (key != -1))
	{
		RDNInputBinder::SetKeyResult res = m_pInputBinder->RestoreKey((size_t)group, (size_t)key);
		HandleKeyResult(res);
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void DlgModOptions::HandleKeyResult(RDNInputBinder::SetKeyResult res)
{
	long warningID = 0;
	bool bRefreshLists = false;

	switch (res)
	{
	case RDNInputBinder::SKR_Error_General:
		break;
	case RDNInputBinder::SKR_Error_Locked:
	case RDNInputBinder::SKR_Error_Reserved:
	{
		if (res == RDNInputBinder::SKR_Error_Reserved)
			warningID = MODOPTIONS_KEYISRESERVED;
		else
			warningID = MODOPTIONS_CANNOTMODIFYCOMMANDHOTKEY;
	}
	break;
	case RDNInputBinder::SKR_Successful_NoConflict:
	case RDNInputBinder::SKR_Successful_Replaced:
	{
		if (res == RDNInputBinder::SKR_Successful_Replaced)
			warningID = MODOPTIONS_KEYUNASSIGNED;
		bRefreshLists = true;
	}
	break;
	}

	if (bRefreshLists)
	{
		RefreshKeyGroupList(true, true);
		RefreshHotKeyList(true);
	}

	wchar_t buf[1024];
	Localizer::GetString(buf, LENGTHOF(buf), warningID);
	ShowKeyMessagePopup("", false);
	ShowWarning(buf, true);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool DlgModOptions::HookInput(const Plat::InputEvent &e)
{
	//
	// look for key presses/releases

	if (e.filter == Plat::IFT_Mouse)
		return true;

	// add keys as they are being held
	if (e.type == Plat::IET_KeyPress)
	{
		if (e.key == Plat::KEY_Escape)
		{
			ClearHotkeyList();
			// remove input callback
			m_hud->DelGlobalInputHook("DlgModOptions");
			ShowKeyMessagePopup("", false);
			ShowWarning(L"", true);
		}
		else
		{
			const char *name = Plat::Input::GetKeyName(e.key);
			AddHotkey(name);

			char str[1024];
			GetCurrentHotkeyString(str, LENGTHOF(str));
			ShowKeyMessagePopup(str, true);
		}
	}
	//
	else if (e.type == Plat::IET_KeyRelease)
	{
		const char *name = Plat::Input::GetKeyName(e.key);
		if (!IsHotkeyAdded(name))
		{
			return true;
		}
		else
		{
			if (IsModifierKey(name))
			{
				RemoveHotkey(name);
				char str[1024];
				GetCurrentHotkeyString(str, LENGTHOF(str));
				ShowKeyMessagePopup(str, true);
				return true;
			}
		}

		// convert from list index to group index
		int group = GetRealGroupIndex(m_hud->GetCustomListSelectedItem(SCREENNAME, GROUPLIST)),
				key = m_hud->GetCustomListSelectedItem(SCREENNAME, KEYLIST);

		if ((group != -1) && (key != -1))
		{
			char str[1024];

			GetCurrentHotkeyString(str, LENGTHOF(str));

			ClearHotkeyList();

			// remove input callback
			m_hud->DelGlobalInputHook("DlgModOptions");
			ShowKeyMessagePopup("", false);

			RDNInputBinder::SetKeyResult res = m_pInputBinder->SetKey((size_t)group, (size_t)key, str);

			HandleKeyResult(res);
		}
	}
	return true;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void DlgModOptions::ShowKeyMessagePopup(const char *key, bool bShow)
{
	//
	// get the localized version of the keycombo

	wchar_t keyW[1024];
	keyW[0] = 0;

	Plat::ComboKey comboKey = Plat::Input::GetComboKeyFromName(key);
	if ((comboKey.numkeys != 0) && !Plat::Input::GetComboKeyLocalizedName(comboKey, keyW, LENGTHOF(keyW)))
	{
		keyW[0] = 0;
	}

	//
	// get message string

	wchar_t msg[1024];
	Localizer::GetString(msg, LENGTHOF(msg), MODOPTIONS_PRESSNEWKEY);

	m_hud->Show(SCREENNAME, "keypopup", bShow);

	m_hud->SetText(SCREENNAME, "keymessage_label", msg);
	m_hud->SetText(SCREENNAME, "currentkey_label", keyW);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void DlgModOptions::ShowWarning(const wchar_t *warning, bool bShow)
{
	m_hud->Show(SCREENNAME, "warning_label", bShow);
	m_hud->SetText(SCREENNAME, "warning_label", warning);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void DlgModOptions::FillHotKeyItem(long group, long key)
{
	char itemName[128];
	if (m_hud->GetCustomListItemName(SCREENNAME, KEYLIST, key, itemName, LENGTHOF(itemName)))
	{

		const RDNInputBinder::HotKey *pHotKey = m_pInputBinder->GetHotKeyAt(group, key);

		char subItem[256];
		wchar_t buf[1024];

		// create a string to set the command name
		strcpy(subItem, itemName);
		strcat(subItem, ".list2_customitem.keyname_label");
		Localizer::GetString(buf, LENGTHOF(buf), pHotKey->locID);
		m_hud->SetText(SCREENNAME, subItem, buf);

		//
		Plat::ComboKey comboKey = Plat::Input::GetComboKeyFromName(pHotKey->keyCombo.c_str());
		if (comboKey.numkeys != 0)
		{
			// create a string to set the command keycombo
			strcpy(subItem, itemName);
			strcat(subItem, ".list2_customitem.key_label");

			if (!Plat::Input::GetComboKeyLocalizedName(comboKey, buf, LENGTHOF(buf)))
			{
				buf[0] = 0;
			}
		}
		else
		{
			// create a string to set the command keycombo
			strcpy(subItem, itemName);
			strcat(subItem, ".list2_customitem.keymissing_label");

			Localizer::GetString(buf, LENGTHOF(buf), MODOPTIONS_KEYMISSING);
		}
		m_hud->SetText(SCREENNAME, subItem, buf);
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
size_t DlgModOptions::GetRealGroupIndex(size_t group)
{
	if (group == size_t(-1) || group >= m_groupIndexTable.size())
		return size_t(-1);

	return m_groupIndexTable[group];
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void DlgModOptions::ClearHotkeyList(void)
{
	m_nonModifierKeyCount = 0;
	m_hotKeyList.clear();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void DlgModOptions::AddHotkey(const char *hotkey)
{
	bool isModifierKey = IsModifierKey(hotkey);

	// make sure we don't have more than the maximum amount of nonmodifier keys in our hotkeycombo
	// ie you can have A+B but not A+B+C, if max == 2, Ctrl+Shift+Alt+A+B is okay though
	if (!isModifierKey && (m_nonModifierKeyCount >= MAXNONMODIFIERKEYS))
		return;

	if (IsHotkeyAdded(hotkey))
		return;

	if (!isModifierKey)
		m_nonModifierKeyCount++;

	m_hotKeyList.push_back(hotkey);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void DlgModOptions::RemoveHotkey(const char *hotkey)
{
	bool isModifierKey = IsModifierKey(hotkey);

	HotKeyList::iterator i = m_hotKeyList.begin(),
											 e = m_hotKeyList.end();
	while (i != e)
	{
		if (strcmp(hotkey, i->c_str()) == 0)
		{
			if (!isModifierKey)
			{
				m_nonModifierKeyCount--;
			}
			m_hotKeyList.erase(i);
			return;
		}
		i++;
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool DlgModOptions::IsHotkeyAdded(const char *hotkey)
{
	HotKeyList::iterator i = m_hotKeyList.begin(),
											 e = m_hotKeyList.end();
	while (i != e)
	{
		if (strcmp(hotkey, i->c_str()) == 0)
		{
			// key already added
			return true;
		}
		i++;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void DlgModOptions::GetCurrentHotkeyString(char *str, size_t strLen)
{
	// sort hotkey list so that modifier keys go first followed by the other keys in alpha sorted order
	// ie. Shift+Control+Alt

	m_hotKeyList.sort(SortHotKeyObj());

	HotKeyList::iterator i = m_hotKeyList.begin(),
											 e = m_hotKeyList.end();
	str[0] = 0;
	while (i != e)
	{
		strcat(str, i->c_str());

		i++;

		if (i != e)
		{
			// add plus inbetween all keys
			strncat(str, "+", strLen);
		}
	}
}

/////////////////////////////////////////////////////////////////////
//

namespace
{
	// HUD string ID's for the options

	// MiniMap Options
	const char *k_MM_ZOOM = "mapzoom_toggle";
	const char *k_MM_ROTATE = "maprotation_toggle";
	const char *k_MM_PAN = "mapscroll_toggle";

	// Camera Options
	const char *k_CAM_ROTATE = "camerarotation_toggle";
	const char *k_CAM_DECLINATION = "cameradeclination_toggle";
	const char *k_CAM_INVERTDEC = "invertdec_toggle";
	const char *k_CAM_INVERTPAN = "invertpan_toggle";

	// Event Sound Options
	const char *k_ESND_BUILDING = "buildingsounds_toggle";
	const char *k_ESND_EVENTCUE = "eventcuesounds_toggle";
	const char *k_ESND_UNITRESPONSE = "unitresponses_toggle";
	const char *k_ESND_UNITCONFIRM = "unitconfirmations_toggle";

	// Mouse Options
	const char *k_UI_MOUSESPEED = "mousesensitivity_statbar";
	const char *k_UI_KEYSCROLL = "keyscroll_statbar";
	const char *k_UI_MOUSESCROLL = "mousescroll_statbar";
} // namespace

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void DlgModOptions::InitOptionsUI()
{
	// Retreive the MOD Options

#define SETCHECK(hud, opt) \
	m_hud->SetCheckBoxState(SCREENNAME, hud, m_pRDNUIOptions->GetBoolOption(RDNUIOptions::opt))

	// mini-map options
	SETCHECK(k_MM_ZOOM, MMO_Zoom);
	SETCHECK(k_MM_ROTATE, MMO_Rotate);
	SETCHECK(k_MM_PAN, MMO_Pan);

	// camera options
	SETCHECK(k_CAM_ROTATE, CO_Rotate);
	SETCHECK(k_CAM_DECLINATION, CO_Declinate);
	SETCHECK(k_CAM_INVERTDEC, CO_InvertDec);
	SETCHECK(k_CAM_INVERTPAN, CO_InvertPan);

#undef SETCHECK

	// map 0.0 to 1.0 to 0 to 100
	m_hud->SetSliderInfo(SCREENNAME, k_UI_KEYSCROLL, 101, long(m_pRDNUIOptions->GetFloatOption(RDNUIOptions::UIO_KeyScroll) * 100.0f));
	m_hud->SetSliderInfo(SCREENNAME, k_UI_MOUSESCROLL, 101, long(m_pRDNUIOptions->GetFloatOption(RDNUIOptions::UIO_MouseScroll) * 100.0f));
}

/////////////////////////////////////////////////////////////////////
// Desc.     : Will update the RDNUIOptions class and save them
// Result    :
// Param.    :
// Author    :
//
void DlgModOptions::SaveOptionsUI()
{
	// Update the RDNUIOptions class

#define GETCHECK(hud, opt) \
	m_pRDNUIOptions->SetBoolOption(RDNUIOptions::opt, m_hud->GetCheckBoxState(SCREENNAME, hud))

	// mini-map options
	GETCHECK(k_MM_ZOOM, MMO_Zoom);
	GETCHECK(k_MM_ROTATE, MMO_Rotate);
	GETCHECK(k_MM_PAN, MMO_Pan);

	// camera options
	GETCHECK(k_CAM_ROTATE, CO_Rotate);
	GETCHECK(k_CAM_DECLINATION, CO_Declinate);
	GETCHECK(k_CAM_INVERTDEC, CO_InvertDec);
	GETCHECK(k_CAM_INVERTPAN, CO_InvertPan);

#undef GETCHECK

	m_pRDNUIOptions->SetFloatOption(RDNUIOptions::UIO_KeyScroll, float(m_hud->GetSliderPosition(SCREENNAME, k_UI_KEYSCROLL)) / 100.0f);
	m_pRDNUIOptions->SetFloatOption(RDNUIOptions::UIO_MouseScroll, float(m_hud->GetSliderPosition(SCREENNAME, k_UI_MOUSESCROLL)) / 100.0f);

	//
	m_pRDNUIOptions->Save();

	//
	m_pRDNUIOptions->ApplyOptions();
}