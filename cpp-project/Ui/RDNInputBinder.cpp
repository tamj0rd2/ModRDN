/////////////////////////////////////////////////////////////////////
// File    : RDNInputBinder.cpp
// Desc    :
// Created : Wednesday, January 30, 2002
// Author  :
//
// (c) 2002 Relic Entertainment Inc.
//

#include "pch.h"
#include "RDNInputBinder.h"

#include <Lua/LuaConfig.h>

#include <Localizer/Localizer.h>

namespace
{
	const char *KEYBINDINGFILE = "player:IC-keybindings.lua";
	const char *DEFAULTFILE = "data:defprofile/IC-keybindings.lua";

	const char *KEYGROUPTABLENAME = "keygroups";
	const char *KEYTABLENAME = "keys";

	typedef std::map<RDNInputBinder::BindedKeyComboName, Plat::ComboKey> KeyComboMap;

	class KeyGroupPlusHotKeys
	{
	public:
		KeyGroupPlusHotKeys(){};

		~KeyGroupPlusHotKeys()
		{
			std::for_each(m_hotKeys.begin(), m_hotKeys.end(), DELETEITEM());
			m_hotKeys.clear();
		}

		RDNInputBinder::KeyGroup &
		GetKeyGroup() { return m_group; }

		void AddHotKey(RDNInputBinder::HotKey *hk) { m_hotKeys.push_back(hk); }

		size_t GetNumHotKeys(void) { return m_hotKeys.size(); }

		RDNInputBinder::HotKey *
		GetHotKey(size_t index) { return m_hotKeys[index]; }

		void sort(void);

	private:
		typedef std::vector<RDNInputBinder::HotKey *> HotKeyList;

		RDNInputBinder::KeyGroup m_group;

		HotKeyList m_hotKeys;
	};

	typedef std::vector<KeyGroupPlusHotKeys *> KeyGroupPlusHotKeysList;

	class LessPredObj
	{
	public:
		bool CompareLocID(int lhs, int rhs)
		{
			wchar_t l[1024], r[1024];
			Localizer::GetString(l, LENGTHOF(l), lhs);
			Localizer::GetString(r, LENGTHOF(r), rhs);
			return Localizer::CompareSort(l, r) < 0;
		}
		bool operator()(KeyGroupPlusHotKeys *lhs, KeyGroupPlusHotKeys *rhs)
		{
			dbAssert(lhs && rhs);
			return CompareLocID(lhs->GetKeyGroup().locID, rhs->GetKeyGroup().locID);
		}
		bool operator()(RDNInputBinder::HotKey *lhs, RDNInputBinder::HotKey *rhs)
		{
			dbAssert(lhs && rhs);
			return CompareLocID(lhs->locID, rhs->locID);
		}
	};

} // namespace

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void KeyGroupPlusHotKeys::sort(void)
{
	std::sort(m_hotKeys.begin(), m_hotKeys.end(), LessPredObj());
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
static bool IsSameOverlapFamily(int overlapFamilyA, int overlapFamilyB)
{
	// check if either does not belong to a family
	if (!overlapFamilyA || !overlapFamilyB)
		return false;

	// check if they belong to the same family
	return overlapFamilyA == overlapFamilyB;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
static bool AreOverlapIDsEqualOrZero(int overlapIDA, int overlapIDB)
{
	// check if either is equal to zero
	if (!overlapIDA || !overlapIDB)
		return true;

	// check if they are equal
	return overlapIDA == overlapIDB;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
static bool DoGroupsOverlap(RDNInputBinder::KeyGroup &lhs, RDNInputBinder::KeyGroup &rhs)
{
	// figure out if groups are from the same overlap family
	bool bIsSameOverlapFamily = IsSameOverlapFamily(lhs.overlapFamily, rhs.overlapFamily);

	// if not early out
	if (!bIsSameOverlapFamily)
	{
		// groups do not belong to the same overlap family
		return false;
	}

	// groups belong to the same family, so now check their overlapids

	// figure out if
	return AreOverlapIDsEqualOrZero(lhs.overlapID, rhs.overlapID);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
static bool IsFileOutdated(LuaConfig &local, LuaConfig &def)
{
	int localVersion = 0,
			defVersion = 0;
	if (local.GetInt("keybindingversion", localVersion) && def.GetInt("keybindingversion", defVersion))
	{
		return localVersion != defVersion;
	}
	else
	{
		dbBreak();
		return true;
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
static void CreateKeyName(RDNInputBinder::BindedKeyComboName &keyCombo, const char *groupName, const char *keyName)
{
	std::string tmp;

	tmp = KEYGROUPTABLENAME;
	tmp += ".";
	tmp += groupName;
	tmp += ".";
	tmp += KEYTABLENAME;
	tmp += ".";
	tmp += keyName;

	keyCombo = tmp.c_str();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
static void AddKeyCombo(KeyComboMap &keyComboMap, const char *groupLuaName, const char *keyLuaName, const char *keyCombo)
{
	RDNInputBinder::BindedKeyComboName entryKey;
	CreateKeyName(entryKey, groupLuaName, keyLuaName);

	KeyComboMap::iterator i = keyComboMap.find(entryKey);
	if (i == keyComboMap.end())
	{
		// add entry
		Plat::ComboKey comboKey = Plat::Input::GetComboKeyFromName(keyCombo);

		if (comboKey.numkeys == 0 && strlen(keyCombo) > 0)
		{
			dbTracef("MOD UI -- Invalid key combo in keybinding file (%s)", keyCombo);
		}

		keyComboMap[entryKey] = comboKey;
	}
	else
	{
		// adding a key that already exists
		dbBreak();
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
static bool LoadKeyGroups(KeyGroupPlusHotKeysList &groups, LuaConfig &luaConfig, KeyComboMap &keyComboMap)
{
	luaConfig.PushTable(KEYGROUPTABLENAME);

	char keyGroupName[64];
	keyGroupName[0] = 0;

	groups.clear();

	// find each keygroup and store it's properties
	while (luaConfig.NextEntry(keyGroupName, LENGTHOF(keyGroupName)))
	{
		KeyGroupPlusHotKeys *pKeyGroup = new KeyGroupPlusHotKeys;

		// store lua name
		pKeyGroup->GetKeyGroup().luaTableName = keyGroupName;
		luaConfig.PushTable(keyGroupName);

		// get loc id
		if (!luaConfig.GetInt("locid", pKeyGroup->GetKeyGroup().locID))
		{
			dbPrintf("group(%s) is missing it's locid", keyGroupName);
			return false;
		}
		// get group type
		int type;
		if (!luaConfig.GetInt("grouptype", type))
		{
			dbPrintf("group(%s) is missing it's type", keyGroupName);
			return false;
		}
		pKeyGroup->GetKeyGroup().type = (RDNInputBinder::KeyGroupType)type;
		// get overlapid
		int overlapID;
		if (!luaConfig.GetInt("overlapid", overlapID))
		{
			dbPrintf("group(%s) is missing it's overlapid", keyGroupName);
			return false;
		}
		pKeyGroup->GetKeyGroup().overlapID = overlapID;

		// get overlap family
		int overlapFamily;
		if (!luaConfig.GetInt("overlapfamily", overlapFamily))
		{
			dbPrintf("group(%s) is missing it's overlapFamily", keyGroupName);
			return false;
		}
		pKeyGroup->GetKeyGroup().overlapFamily = overlapFamily;

		// search for keys within keygroup
		if (luaConfig.GetType("keys") != LuaConfig::LT_TABLE)
		{
			dbPrintf("group(%s) is missing it's key table", keyGroupName);
			return false;
		}

		// push key table
		luaConfig.PushTable(KEYTABLENAME);

		char hotKeyName[64];
		hotKeyName[0] = 0;

		while (luaConfig.NextEntry(hotKeyName, LENGTHOF(hotKeyName)))
		{
			RDNInputBinder::HotKey *pHotKey = new RDNInputBinder::HotKey;

			// set the table name
			pHotKey->luaTableName = hotKeyName;

			luaConfig.PushTable(hotKeyName);

			// get loc id
			if (!luaConfig.GetInt("2", pHotKey->locID))
			{
				dbPrintf("group(%s)/key(%s) is missing it's locid", keyGroupName, hotKeyName);
				return false;
			}

			// get keycombo
			char keyCombo[64];
			if (!luaConfig.GetString("1", keyCombo, LENGTHOF(keyCombo)))
			{
				dbPrintf("group(%s)/key(%s) is missing it's keycombo", keyGroupName, hotKeyName);
				return false;
			}
			pHotKey->keyCombo = keyCombo;

			// add key
			AddKeyCombo(
					keyComboMap,
					pKeyGroup->GetKeyGroup().luaTableName.c_str(),
					pHotKey->luaTableName.c_str(),
					keyCombo);

			luaConfig.PopTable();

			// add the hot key
			pKeyGroup->AddHotKey(pHotKey);
		}

		luaConfig.PopTable(); // end group keys

		luaConfig.PopTable(); // end group

		// if there are actually hot keys, add to the groups list
		if (pKeyGroup->GetNumHotKeys() != 0)
		{
			// add the group
			groups.push_back(pKeyGroup);
		}
		else
		{
			// discard the group
			delete pKeyGroup;
		}
	}

	luaConfig.PopTable();

	// sort groups
	std::sort(groups.begin(), groups.end(), LessPredObj());

	// sort the keys of each group
	for (KeyGroupPlusHotKeysList::iterator iter = groups.begin(); iter != groups.end(); iter++)
	{
		(*iter)->sort();
	}

	return !groups.empty();
}

/////////////////////////////////////////////////////////////////////
//	Desc.	:
//	Result	:
//	Param.	:
//	Author	:
//
static void FreeGroups(KeyGroupPlusHotKeysList &groups)
{
	std::for_each(groups.begin(), groups.end(), DELETEITEM());
	groups.clear();
}

/////////////////////////////////////////////////////////////////////
// Desc.     : returns true if the status is the same as in the combo
//			 : ie: key isn't down and key isn't part of combo
//			 : or key is down and is part of the combo
// Result    :
// Param.    :
// Author    :
//
static bool CompareKeyStatus(Plat::ComboKey combo, Plat::InputKey key)
{
	dbAssert(combo.numkeys);

	bool bFound = false;
	for (int i = 0; i < (combo.numkeys - 1); i++)
	{
		if ((key == combo.key[i]) && Plat::Input::IsKeyPressed(key))
		{
			bFound = true;
			break;
		}
	}

	if (bFound)
	{
		return Plat::Input::IsKeyPressed(key);
	}
	else
	{
		return !Plat::Input::IsKeyPressed(key);
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
class RDNInputBinder::Data
{
public:
	LuaConfig m_luaConfig;

	KeyGroupPlusHotKeysList m_groups;

	KeyComboMap m_keyComboMap;
};

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
RDNInputBinder::RDNInputBinder()
		: m_pimpl(new Data)
{
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
RDNInputBinder::~RDNInputBinder()
{
	FreeGroups(m_pimpl->m_groups);

	m_pimpl->m_keyComboMap.clear();

	DELETEZERO(m_pimpl);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool RDNInputBinder::Save(void)
{
	if (m_pimpl->m_groups.empty())
		return false;

	if (!m_pimpl->m_luaConfig.StartSave(KEYBINDINGFILE))
		return false;

	m_pimpl->m_luaConfig.SaveAll(true);

	if (!m_pimpl->m_luaConfig.EndSave())
	{
		dbBreak();
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool RDNInputBinder::Load(void)
{
	if (!load())
	{
		return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool RDNInputBinder::RestoreAllGroups(void)
{
	if (!m_pimpl->m_groups.empty())
	{
		Reset();
	}

	if (!m_pimpl->m_luaConfig.LoadFile(DEFAULTFILE))
		return false;

	if (!LoadKeyGroups(m_pimpl->m_groups, m_pimpl->m_luaConfig, m_pimpl->m_keyComboMap))
	{
		Reset();
		return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//

RDNInputBinder::SetKeyResult RDNInputBinder::RestoreKey(size_t group, size_t key)
{
	if (group >= m_pimpl->m_groups.size())
		return SKR_Error_General;
	if (key >= m_pimpl->m_groups[group]->GetNumHotKeys())
		return SKR_Error_General;

	LuaConfig luaConfig;

	if (!luaConfig.LoadFile(DEFAULTFILE))
		return SKR_Error_General;

	if (IsFileOutdated(m_pimpl->m_luaConfig, luaConfig))
	{
		RestoreAllGroups();
		return SKR_Successful_NoConflict;
	}

	// restore key by loading default file
	luaConfig.PushTable("keygroups");
	luaConfig.PushTable(m_pimpl->m_groups[group]->GetKeyGroup().luaTableName.c_str());
	luaConfig.PushTable("keys");
	luaConfig.PushTable(m_pimpl->m_groups[group]->GetHotKey(key)->luaTableName.c_str());

	char keyCombo[64];
	if (!luaConfig.GetString("1", keyCombo, LENGTHOF(keyCombo)))
	{
		dbPrintf("group(%s)/key(%s) is missing it's keycombo", m_pimpl->m_groups[group]->GetKeyGroup().luaTableName.c_str(), m_pimpl->m_groups[group]->GetHotKey(key)->luaTableName.c_str());
		return SKR_Error_General;
	}

	// don't bother popping tables because the config file gets destroyed anyway
	return SetKey(group, key, keyCombo);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
RDNInputBinder::SetKeyResult RDNInputBinder::SetKey(size_t group, size_t key, const char *keyCombo)
{
	if (group >= m_pimpl->m_groups.size())
		return SKR_Error_General;
	else
	{
		if (key >= m_pimpl->m_groups[group]->GetNumHotKeys())
			return SKR_Error_General;
	}

	// make sure the new key combo isn't similar to others
	SetKeyResult result = IsKeyChangeValid(group, key, keyCombo);
	if (result != SKR_Successful_NoConflict)
	{
		return result;
	}

	// remove hotkeys that are in overlappings groups that are similar
	bool bKeyWasUnassigned = RemoveSimilarHotkeys(group, key, keyCombo);

	// update keycombo
	UpdateKeyCombo(group, key, keyCombo);

	return bKeyWasUnassigned ? SKR_Successful_Replaced : SKR_Successful_NoConflict;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
size_t RDNInputBinder::GetGroupCount(void) const
{
	return m_pimpl->m_groups.size();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
const RDNInputBinder::KeyGroup *RDNInputBinder::GetGroupAt(size_t group) const
{
	if (group >= m_pimpl->m_groups.size())
		return NULL;
	return &m_pimpl->m_groups[group]->GetKeyGroup();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
size_t RDNInputBinder::GetHotKeyCount(size_t group) const
{
	if (group >= m_pimpl->m_groups.size())
		return 0;
	return m_pimpl->m_groups[group]->GetNumHotKeys();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
const RDNInputBinder::HotKey *RDNInputBinder::GetHotKeyAt(size_t group, size_t key) const
{
	if (group >= m_pimpl->m_groups.size())
		return NULL;
	else
	{
		if (key >= m_pimpl->m_groups[group]->GetNumHotKeys())
			return NULL;
	}

	return m_pimpl->m_groups[group]->GetHotKey(key);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool RDNInputBinder::HasUnassignedKey(size_t group) const
{
	size_t keyCount = GetHotKeyCount(group);
	while (keyCount--)
	{
		const HotKey *pHotKey = GetHotKeyAt(group, keyCount);
		if (strlen(pHotKey->keyCombo.c_str()) == 0)
			return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
size_t RDNInputBinder::IsComboKeyPressed(const char *keyComboName, const Plat::InputEvent *e) const
{
	// early out
	if (keyComboName == 0 || *keyComboName == 0 || e->type != Plat::IET_KeyPress)
		return 0;

	KeyComboMap::iterator i = m_pimpl->m_keyComboMap.find(keyComboName);
	if (i != m_pimpl->m_keyComboMap.end())
	{
		if (Plat::Input::IsComboKeyPressed(i->second, e))
		{
			// only allow perfect match with the exception ignoring shift key state,
			// combos with shift keys get higher priority when shift key is held

			// check if ctrl/alt is in conflicting status
			if (!CompareKeyStatus(i->second, Plat::KEY_Control) || !CompareKeyStatus(i->second, Plat::KEY_Alt))
				return 0;

			if (CompareKeyStatus(i->second, Plat::KEY_Shift))
				return 2;
			else
				return 1;
		}
	}
	else
	{
		dbTracef("MOD UI -- Key not found in keybinding file (%s)", keyComboName);

		//	I'm adding the invalid key to the map so we only fail once
		RDNInputBinder *pThis = const_cast<RDNInputBinder *>(this);
		Plat::ComboKey key;
		key.numkeys = 0;
		pThis->m_pimpl->m_keyComboMap.insert(std::make_pair(keyComboName, key));
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////
// Desc.    : Searches for a hotkey based on a name that is assumed to be in
//			: the format 'KEYGROUPTABLENAME.<hotkeygroupname>.KEYTABLENAME.<hotkeyname>'
// Result   :
// Param.   :
// Author   :
//
const RDNInputBinder::HotKey *RDNInputBinder::GetHotKeyByTableName(const char *tableName) const
{
	// validation
	dbAssert(tableName);

	// early out
	if (*tableName == 0)
		return 0;

	// copy tableName so that we can manipulate it
	const char *key = tableName;

	// move curpos to after KEYGROUPTABLENAME
	const size_t keygroupLen = strlen(KEYGROUPTABLENAME);

	if (strncmp(key, KEYGROUPTABLENAME, keygroupLen) != 0 || key[keygroupLen] != '.')
		return 0;

	key += keygroupLen + 1;

	// search for group
	char *sepGroup = strchr(key, '.');

	if (sepGroup == 0)
		return 0;

	// look for group with matching table name
	size_t gi = 0;
	size_t ge = m_pimpl->m_groups.size();

	for (; gi != ge; ++gi)
	{
		if (strncmp(key, m_pimpl->m_groups[gi]->GetKeyGroup().luaTableName.c_str(), sepGroup - key) == 0)
			break;
	}

	if (gi == ge)
		// group not found
		return 0;

	//
	key = sepGroup + 1;

	// move curpos to after KEYTABLENAME
	const size_t keytableLen = strlen(KEYTABLENAME);

	if (strncmp(key, KEYTABLENAME, keytableLen) != 0 || key[keytableLen] != '.')
		return 0;

	key += keytableLen + 1;

	// what remains is the name of the hotkey
	size_t ki = 0;
	size_t ke = m_pimpl->m_groups[gi]->GetNumHotKeys();

	for (; ki != ke; ++ki)
	{
		if (strcmp(key, m_pimpl->m_groups[gi]->GetHotKey(ki)->luaTableName.c_str()) == 0)
			break;
	}

	if (ki == ke)
		return 0;

	return m_pimpl->m_groups[gi]->GetHotKey(ki);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNInputBinder::Reset(void)
{
	// clear all
	m_pimpl->m_luaConfig.ClearVariable("keybindingversion");
	m_pimpl->m_luaConfig.ClearVariable("keygroups");

	FreeGroups(m_pimpl->m_groups);
	m_pimpl->m_keyComboMap.clear();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void RDNInputBinder::UpdateKeyCombo(size_t group, size_t key, const char *keyCombo)
{
	KeyGroupPlusHotKeys *pGroup = m_pimpl->m_groups[group];

	// update cached state
	pGroup->GetHotKey(key)->keyCombo = keyCombo;

	// update lua state
	m_pimpl->m_luaConfig.PushTable("keygroups");
	m_pimpl->m_luaConfig.PushTable(pGroup->GetKeyGroup().luaTableName.c_str());
	m_pimpl->m_luaConfig.PushTable("keys");
	m_pimpl->m_luaConfig.PushTable(pGroup->GetHotKey(key)->luaTableName.c_str());

	m_pimpl->m_luaConfig.SetString("1", keyCombo);

	m_pimpl->m_luaConfig.PopTable();
	m_pimpl->m_luaConfig.PopTable();
	m_pimpl->m_luaConfig.PopTable();
	m_pimpl->m_luaConfig.PopTable();

	// update cached hotkey
	RDNInputBinder::BindedKeyComboName entryKey;
	CreateKeyName(
			entryKey,
			pGroup->GetKeyGroup().luaTableName.c_str(),
			pGroup->GetHotKey(key)->luaTableName.c_str());

	KeyComboMap::iterator i = m_pimpl->m_keyComboMap.find(entryKey);
	if (i != m_pimpl->m_keyComboMap.end())
	{
		i->second = Plat::Input::GetComboKeyFromName(keyCombo);
	}
	else
	{
		// updating a key that doesn't exist
		dbBreak();
	}
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
// TODO: implement this
RDNInputBinder::SetKeyResult RDNInputBinder::IsKeyChangeValid(size_t group, size_t key, const char *keyCombo)
{
	// early out if key group is locked
	if ((m_pimpl->m_groups[group]->GetKeyGroup().type == KGT_GlobalLocked) ||
			(m_pimpl->m_groups[group]->GetKeyGroup().type == KGT_GlobalLockedAndHidden))
	{
		return SKR_Error_Locked;
	}

	for (size_t gndx = 0; gndx < GetGroupCount(); gndx++)
	{
		// only check for conflicts with locked groups
		if ((m_pimpl->m_groups[gndx]->GetKeyGroup().type == KGT_GlobalLocked) ||
				(m_pimpl->m_groups[gndx]->GetKeyGroup().type == KGT_GlobalLockedAndHidden))
		{
			for (size_t kndx = 0; kndx < GetHotKeyCount(gndx); kndx++)
			{
				// TODO: write a better hotkey overlap test
				const HotKey *pHotKey = GetHotKeyAt(gndx, kndx);
				if (strcmp(pHotKey->keyCombo.c_str(), keyCombo) == 0)
				{
					return SKR_Error_Reserved;
				}
			}
		}
	}

	return SKR_Successful_NoConflict;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool RDNInputBinder::RemoveSimilarHotkeys(size_t group, size_t key, const char *keyCombo)
{
	bool bKeyWasUnassigned = false;

	for (size_t gndx = 0; gndx < GetGroupCount(); gndx++)
	{
		// don't check locked groups because we can assume that
		// this function wouldn't be called if there was collision with global hotkeys
		if ((m_pimpl->m_groups[gndx]->GetKeyGroup().type == KGT_GlobalLocked) ||
				(m_pimpl->m_groups[gndx]->GetKeyGroup().type == KGT_GlobalLockedAndHidden))
			continue;

		// if both groups are non global, then check if they are either:
		//	a) part of the same overlapfamily(if a overlapfamily=0, then a group has no family), and one overlapid=0 or both overlapids are equal
		//	b) not part of the same overlapfamily but have equal overlapids
		if (
				(m_pimpl->m_groups[group]->GetKeyGroup().type == KGT_Normal) &&
				(m_pimpl->m_groups[gndx]->GetKeyGroup().type == KGT_Normal) &&
				((gndx != group) && !DoGroupsOverlap(m_pimpl->m_groups[group]->GetKeyGroup(), m_pimpl->m_groups[gndx]->GetKeyGroup())))
			continue;

		// otherwise if the group the key belongs to is global we have to check for collision
		// against all keygroups

		for (size_t kndx = 0; kndx < GetHotKeyCount(gndx); kndx++)
		{
			// skip the key we are checking with
			if ((group == gndx) && (key == kndx))
				continue;

			// TODO: write a better hotkey overlap test
			const HotKey *pHotKey = GetHotKeyAt(gndx, kndx);
			if (strcmp(pHotKey->keyCombo.c_str(), keyCombo) == 0)
			{
				UpdateKeyCombo(gndx, kndx, "");
				bKeyWasUnassigned = true;
			}
		}
	}

	return bKeyWasUnassigned;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
bool RDNInputBinder::load(void)
{
	if (!m_pimpl->m_groups.empty())
	{
		Reset();
	}

	// load profile keybindings file
	if (!m_pimpl->m_luaConfig.LoadFile(KEYBINDINGFILE))
	{
		Reset();

		// fallback to loading defprofile keybindings file
		if (!m_pimpl->m_luaConfig.LoadFile(DEFAULTFILE))
		{
			Reset();
			return false;
		}
	}
	else
	{
		// load default file to check if the local key.lua file is outdated
		LuaConfig luaConfig;
		if (luaConfig.LoadFile(DEFAULTFILE))
		{
			if (IsFileOutdated(m_pimpl->m_luaConfig, luaConfig))
			{
				return RestoreAllGroups();
			}
		}
	}

	if (!LoadKeyGroups(m_pimpl->m_groups, m_pimpl->m_luaConfig, m_pimpl->m_keyComboMap))
	{
		Reset();
		return false;
	}

	return true;
}
