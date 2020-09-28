/////////////////////////////////////////////////////////////////////
// File    : DlgModOptions.h
// Desc    :
// Created : Tuesday, January 29, 2002
// Author  :
//
// (c) 2002 Relic Entertainment Inc.
//

#pragma once

#include <Assist/Callback.h>

#include <Platform/Platform.h>

#include "RDNInputBinder.h"

class RTSHud;
class RDNSimProxy;
class RDNUIOptions;

/////////////////////////////////////////////////////////////////////
// DlgModOptions

class DlgModOptions
{
	// types
public:
	typedef Callback::Obj0nRet CloseCB;

	typedef std::list<std::string> HotKeyList;

	// construction
private:
	DlgModOptions(RTSHud *, const RDNSimProxy *, RDNInputBinder *pInputBinder, RDNUIOptions *pRDNUIOptions, const CloseCB &cb);

public:
	~DlgModOptions();

	static DlgModOptions *Create(RTSHud *, const RDNSimProxy *, RDNInputBinder *pInputBinder, RDNUIOptions *pRDNUIOptions, const CloseCB &cb);

	// fields
private:
	RTSHud *m_hud;
	const RDNSimProxy *m_proxy;

	// weak pointer
	RDNInputBinder *m_pInputBinder;

	// weak pointer
	RDNUIOptions *m_pRDNUIOptions;

	CloseCB m_cbClose;

	std::vector<size_t> m_groupIndexTable; // because some of the groups don't get displayed we have

	size_t m_nonModifierKeyCount;
	HotKeyList m_hotKeyList;

	// implementation
private:
	void OnClose(const std::string &);
	void InitKeyGroupUI(void);

	void RefreshKeyGroupList(bool bKeepState, bool bHotKeyKeepState);
	void RefreshHotKeyList(bool bKeepState);

	void OnGroupItemChangeCB(const std::string &, long);
	void OnHotKeyItemChangeCB(const std::string &, long);
	void OnHotKeyItemActivatedCB(const std::string &, long);

	void OnAssignKeyCB(const std::string &);
	void OnRestoreAllKeysCB(const std::string &);
	void OnRestoreKeyCB(const std::string &);

	void HandleKeyResult(RDNInputBinder::SetKeyResult result);

	bool HookInput(const Plat::InputEvent &);

	void ShowKeyMessagePopup(const char *key, bool bShow);
	void ShowWarning(const wchar_t *warning, bool bShow);

	void FillHotKeyItem(long group, long key);

	size_t GetRealGroupIndex(size_t group);

	void ClearHotkeyList(void);
	void AddHotkey(const char *hotkey);
	void RemoveHotkey(const char *hotkey);
	bool IsHotkeyAdded(const char *hotkey);
	void GetCurrentHotkeyString(char *str, size_t strLen);

	void InitOptionsUI();
	void SaveOptionsUI();

	// copy -- do not define
private:
	DlgModOptions(const DlgModOptions &);
	DlgModOptions &operator=(const DlgModOptions &);
};
