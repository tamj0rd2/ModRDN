/////////////////////////////////////////////////////////////////////
// File    : RDNInputBinder.h
// Desc    : 
// Created : Wednesday, January 30, 2002
// Author  : 
// 
// (c) 2002 Relic Entertainment Inc.
//

#pragma once 

#include <Assist/FixedString.h>

#include <Platform/Platform.h>


class RDNInputBinder
{

// types
public:
	enum KeyGroupType
	{
		KGT_Normal,
		KGT_GlobalUnlocked,
		KGT_GlobalLocked,
		KGT_GlobalLockedAndHidden,
	};

	enum SetKeyResult
	{
		SKR_Error_General,			// general error, indexes out of range, couldn't load file or default file, etc.
		SKR_Error_Locked,			// the target command's hotkey cannot be changed, it is locked
		SKR_Error_Reserved,			// the hotkey that was passed is reserved by another command is locked
		SKR_Successful_NoConflict,	//
		SKR_Successful_Replaced,	//
	};

	struct KeyGroup
	{
		std::string		luaTableName;
		
		int				locID;
		KeyGroupType	type;

		// These two vars are used together to make sure there aren't any keys mapped to different commands in any given context
		// NOTE(1):	These two vars are only used for groups with grouptype = 0
		// NOTE(2):	A key cannot be assigned to two or more different commands in the following situation:
		//		If the groups the commands belong are:
		//			a) part of the same overlapfamily(if a overlapfamily=0, then a group has no family), and one overlapid=0 or both overlapids are equal
		//			b) not part of the same overlapfamily but have equal overlapids
		int				overlapID;
		int				overlapFamily;
	};

	struct HotKey
	{
		std::string		luaTableName;

		std::string		keyCombo;
		int				locID;
	};

	typedef fstring<256> BindedKeyComboName;

// construction
public:
	 RDNInputBinder();
	~RDNInputBinder();

// noncopyable
private:
	RDNInputBinder( const RDNInputBinder& );

// interface
public:
	bool			Save( void );
	bool			Load( void );

	bool			RestoreAllGroups( void );
	SetKeyResult	RestoreKey( size_t group, size_t key );
	SetKeyResult	SetKey( size_t group, size_t key, const char* keycombo );

	size_t			GetGroupCount( void ) const;
	const KeyGroup*	GetGroupAt( size_t group ) const;

	size_t			GetHotKeyCount( size_t group ) const;
	const HotKey*	GetHotKeyAt( size_t group, size_t key ) const;

	bool			HasUnassignedKey( size_t group ) const;
	
	size_t			IsComboKeyPressed( const char* keyComboName, const Plat::InputEvent* e ) const;
						//	returns number of key matches, higher is a better match
	
	const HotKey*	GetHotKeyByTableName( const char* luaTableName ) const;

// implementation
private:

	void			Reset( void );

	// all of these functions assume that all the parameters have already been validated
	void			UpdateKeyCombo( size_t group, size_t key, const char* keycombo );
	SetKeyResult	IsKeyChangeValid( size_t group, size_t key, const char* keycombo );
	bool			RemoveSimilarHotkeys( size_t group, size_t key, const char* keycombo );

	bool			load( void );

// fields
private:
	class Data;
	Data* m_pimpl;

};
