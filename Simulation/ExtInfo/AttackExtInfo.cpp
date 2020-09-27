/////////////////////////////////////////////////////////////////////
// File    : AttackExtInfo.cpp
// Desc    : 
// Created : Wednesday, Dec 19, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h" 
#include "AttackExtInfo.h" 

#include "../RDNWorld.h" 
#include "../UnitConversion.h"

const size_t MAX_ATTACKPACKAGES = 4;

///////////////////////////////////////////////////////////////////// 
// AttackExtInfo

AttackExtInfo::AttackExtInfo( const ControllerBlueprint* cbp )
{
	// mimic the code in AttackExtInfo
	attackInfo.hasAttack		= false;

	float maxRange = 0.0f;

	// get melee attack info
	const char* type		= "Melee";

	for (int pkgIndex=0; pkgIndex<MAX_ATTACKPACKAGES; pkgIndex++)
	{
		char dmg[128];
		sprintf( dmg, "%s%d_damage", type, pkgIndex );

		AttackPackage attack;
		memset( &attack, 0, sizeof( attack ) );

		attack.m_type = ATTACKTYPE_Melee;
		
		if( cbp->GameAttributeCheck( dmg ) )
		{
			// get all the attack info from this pkgIndex
			attack.m_damagePerSec = GetVal( cbp, dmg, 0.1f, 1000.0f );

			//
			char contact[ 128 ];
			sprintf( contact, "%s%d_contact", type, pkgIndex );
			float contactpercent = GetVal( cbp, contact, 0.05f, 1.0f );
			
			//
			char rate[ 128 ];
			sprintf( rate, "%s%d_rate", type, pkgIndex );
			float ratesec = GetVal( cbp, rate, 0.1f, 1000.0f );

			// convert the attack rate in sec's to attack ticks
			TimeSec_To_NumTicks( k_SimStepsPerSecond, ( ratesec * contactpercent ), attack.m_attackticks );
			TimeSec_To_NumTicks( k_SimStepsPerSecond, ( ratesec * (1.0f - contactpercent ) ), attack.m_coolticks );
			
			//
			char dmgtype[ 128 ];
			sprintf( dmgtype, "%s%d_dmgtype", type, pkgIndex );
			attack.m_dmgType = DamageType( GetVal( cbp, dmgtype, 0, 256 ) );
			
			// calculate damage done on each hit
			attack.m_damagePerHit = attack.m_damagePerSec * ratesec;

			unsigned long attacknum;
			char attknum[128];
			sprintf( attknum, "%s%d_number", type, pkgIndex );
			attacknum = GetVal( cbp, attknum, 1 );

			// create the proper animation name
			sprintf( attack.m_Animation, "%s%d", type, attacknum );

			// melee attack has a default max range of 0
			attack.m_maxRange = 0.0f;

			attackInfo.meleeList.push_back( attack );

			maxRange = __max( maxRange, attack.m_maxRange );

			attackInfo.hasAttack = true;
		}
	}
}

