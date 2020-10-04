/////////////////////////////////////////////////////////////////////
// File    : RDNUIOptions.h
// Desc    :
// Created : Saturday, March 23, 2002
// Author  :
//
// (c) 2002 Relic Entertainment Inc.
//
#pragma once

#include <Lua/LuaConfig.h>

/////////////////////////////////////////////////////////////////////
// Forward Declarations

class UIInterface;
class CameraInterface;

/////////////////////////////////////////////////////////////////////
// RDNUIOptions

class RDNUIOptions
{
	// types
public:
public:
	RDNUIOptions(UIInterface *pUIInterface, CameraInterface *pCameraInterface);
	~RDNUIOptions();

	// Interface
public:
	//
	void Load();
	void Save();

	//
	void ApplyOptions();

	// Options Interface

	/////////////////////////////////////////////////////////////////////
	// Boolean Options Interface

	enum BooleanOptions
	{
		BO_First,

		// MiniMap Options
		MMO_Rotate = BO_First,
		MMO_Zoom,
		MMO_Pan,

		// Camera Options
		CO_Rotate,
		CO_Declinate,
		CO_InvertDec,
		CO_InvertPan,

		BO_Last
	};

	void SetBoolOption(BooleanOptions Option, bool bValue);
	bool GetBoolOption(BooleanOptions Option) const;

	/////////////////////////////////////////////////////////////////////
	// Boolean Options Interface

	enum FloatOptions
	{
		FO_First,

		// UI Options
		UIO_MouseScroll = FO_First,
		UIO_KeyScroll,

		FO_Last
	};

	void SetFloatOption(FloatOptions Option, float value);
	float GetFloatOption(FloatOptions Option) const;

	// Implementation
private:
	// Data Members
private:
	// the LuaConfig object is mutable becase the Get Functions are Non-Const, they modify the Lua State
	mutable LuaConfig m_LuaConfig;

	// Weak Pointers to Interfaces
	UIInterface *m_pUIInterface;
	CameraInterface *m_pCameraInterface;
};