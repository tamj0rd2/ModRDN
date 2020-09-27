/////////////////////////////////////////////////////////////////////
// File    : RDNEntityFilter.h
// Desc    : 
// Created : Tuesday, July 03, 2001
// Author  : 
// 
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include <ModInterface/EntityFilter.h>

#include <SimEngine/EntityGroup.h>

class RDNPlayer;

class RDNEntityFilter: public EntityFilter
{
public:
	// singleton interface
	static bool					Initialize( void );
	static void					Shutdown( void );

	static RDNEntityFilter*	Instance(void); 

public:
	// inherited from EntityFilter
	virtual void		SetMode( FilterMode );
	virtual void		ClearMode();

	virtual bool		QueryFilter( const Entity* ) const;
	virtual int			QueryPriority( const Entity* ) const;
	virtual void		FilterGroup( EntityGroup& ) const;

	virtual void		SetContextGroup( const EntityGroup& );
	virtual void		ClearContextGroup();

	virtual void		SetExclusionGroup( const EntityGroup& );
	virtual void		ClearExclusionGroup();

public:
	// extensions
	void SetLocalPlayer( const RDNPlayer *localPlayer );

private:
	RDNEntityFilter();

	bool			IsLocalPlayerUnit( const Entity* ) const;
	bool			IsSingleSelect( const Entity* ) const;

private:
	const RDNPlayer*	m_pLocalPlayer;	// weak pointer
	EntityFilter::FilterMode
						m_filterMode;
	bool				m_bContext_Enabled;
	bool				m_bContext_PickLocal;
	EntityGroup			m_exclusionGroup;
};