/////////////////////////////////////////////////////////////////////
// File    : ModStaticInfo.h
// Desc    : 
// Created : Monday, March 19, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include <ModInterface/ECStaticInfo.h>

// forward declaration
class ControllerBlueprint;
class ModController;
class EntityController;
class Entity;

///////////////////////////////////////////////////////////////////// 
// ModStaticInfo

class ModStaticInfo : public ECStaticInfo
{
// types
public:
	class ExtInfo;

	enum ExtInfoList
	{
		EXTINFOID_Attack,
		EXTINFOID_Cost,
		EXTINFOID_Health,
		EXTINFOID_Moving,
		EXTINFOID_Warrior,
		EXTINFOID_Resource,
		EXTINFOID_Sight,
		EXTINFOID_Site,
		EXTINFOID_UI,
	};

// construction
public:
	ModStaticInfo( const ControllerBlueprint* cbp )
	  : ECStaticInfo( cbp ) 
	{
	}

// interface
public:
	// query for the specified substruct
	virtual const ModStaticInfo::ExtInfo* QInfo( unsigned char id ) const;
};

///////////////////////////////////////////////////////////////////// 
// 

template < class T >
	inline const T* QIExtInfo( const ModStaticInfo* msi )
	{
		return ( msi == 0 )? 0 : static_cast< const T* >( msi->QInfo( T::ExtensionID ) );
	}

template < class T >
	inline const T* QIExtInfo( const ECStaticInfo* si )
	{
		return QIExtInfo<T>( static_cast< const ModStaticInfo* >( si ) );
	}

template < class T >
	inline const T* QIExtInfo( const ModController* control )
	{
		return (control == 0)? 0 : QIExtInfo<T>( control->GetECStaticInfo() );
	}

template < class T >
	inline const T* QIExtInfo( const EntityController* control )
	{
		return QIExtInfo<T>( static_cast< const ModController* >(control) );
	}

template < class T >
	inline const T* QIExtInfo( const Entity* pEntity )
	{
		return QIExtInfo<T>( pEntity->GetController() );
	}

///////////////////////////////////////////////////////////////////// 
// ModStaticInfo::ExtInfo

class ModStaticInfo::ExtInfo
{
// interface
protected:
	template< typename T >
		T GetVal( const ControllerBlueprint* cbp, const char* attr, T def )
	{
		// validate parm
		dbAssert( cbp != 0 );
		dbAssert( strlen( attr ) != 0 );
		
		// check if attribute exists
		if( !cbp->GameAttributeCheck( attr ) )
		{
			dbWarningf( 'SMOD', "ebp %S missing attribute %s", 
				cbp->GetScreenName(),
				attr
				);

			return def;
		}

		// retrieve value
		return T( cbp->GameAttributeRetrieve( attr ) );
	}
		
	template< typename T >
		T GetVal( const ControllerBlueprint* cbp, const char* attr, T tmin, T tmax )
	{
		// validate parm
		dbAssert( tmin < tmax );
		
		// retrieve value
		T t = GetVal( cbp, attr, tmin );

		if ( t < tmin || t > tmax )
		{
			dbWarningf( 'SMOD', "ebp %S attribute %s out of bounds", cbp->GetScreenName(), attr );

			if ( t < tmin )
				t = tmin;

			if( t > tmax )
				t = tmax;
		}

		return t;
	}

	template<>
		bool GetVal<bool>( const ControllerBlueprint* cbp, const char* attr, bool def )
	{
		// validate parm
		dbAssert( cbp != 0 );
		dbAssert( strlen( attr ) != 0 );
		
		// check if attribute exists
		if( !cbp->GameAttributeCheck( attr ) )
		{
			dbWarningf( 'SMOD', "ebp %S missing attribute %s", 
				cbp->GetScreenName(),
				attr
				);

			return def;
		}

		// retrieve value
		return cbp->GameAttributeRetrieve( attr ) != 0.0f;
	}
};
