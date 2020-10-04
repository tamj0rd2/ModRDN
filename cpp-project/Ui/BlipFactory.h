/////////////////////////////////////////////////////////////////////
// File    : BlipFactory.h
// Desc    :
// Created : Monday, September 24, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include <SimEngine/EntityGroup.h>

#include <Util/Colour.h>

/////////////////////////////////////////////////////////////////////
// Blip

class Blip
{
	friend class BlipFactory;

	// types
public:
	enum Flags
	{
		BF_ENTITY = 1 << 0,
		BF_POS = 1 << 1,
		BF_TEMP = 1 << 2,
	};

	// construction
	// private so it must be created by friends
private:
	Blip(int id, bool temp);

	// interface
public:
	int GetID() const;

	// needed to display
	const Vec3f &GetPosition() const;
	float GetRadius() const;
	Colour GetColour() const;

	// call this every frame
	bool Update(float timeNow);

	// call this to determine if we should remove this blip
	bool IsDead() const;

	// don't save temporaries
	bool IsTemp() const;

	//
	void SetPosition(const Vec3f &pos);
	void SetEntity(const Entity *e);

	// override default life time for temp blips
	void SetLifeTime(float lifetime);

	// load and save blips
	void Load(IFF &iff);
	void Save(IFF &iff);

	// fields
private:
	const int m_id;

	// use an entity group to detect the destruction of entity
	int m_flags;
	EntityGroup m_entity;
	Vec3f m_pos;

	// for animation purposes
	float m_timeBlip;
	float m_lifetime;
	float m_radius;

	// implementation
private:
	void Reset();

	// copy -- do not define
private:
	Blip(const Blip &);
	Blip &operator=(const Blip &);
};

/////////////////////////////////////////////////////////////////////
// BlipFactory

class BlipFactory
{
	// types
public:
	typedef std::vector<Blip *> BlipArray;

	// construction
public:
	BlipFactory();
	~BlipFactory();

	// interface
public:
	Blip *CreateBlipTemp();
	Blip *CreateBlip(int id);

	// NOTE: order can change from frame to frame
	size_t GetBlipCount() const;
	Blip *GetBlipAt(size_t index);

	Blip *GetBlipFromId(int id);

	void DeleteBlip(int id);
	void DeleteBlipDead();

	// load and save blips
	void Load(IFF &iff);
	void Save(IFF &iff);

	// fields
private:
	int m_blipTemp;
	BlipArray m_blips;

	// copy -- do not define
private:
	BlipFactory(const BlipFactory &);
	BlipFactory &operator=(const BlipFactory &);
};

/////////////////////////////////////////////////////////////////////
// inlines

inline int Blip::GetID() const
{
	return m_id;
}

inline const Vec3f &Blip::GetPosition() const
{
	dbAssert(!IsDead());
	return m_pos;
}

inline float Blip::GetRadius() const
{
	dbAssert(!IsDead());
	return m_radius;
}

inline Colour Blip::GetColour() const
{
	return Colour(0xff, 0xff, 0);
}

inline size_t BlipFactory::GetBlipCount() const
{
	return m_blips.size();
}

inline bool Blip::IsDead() const
{
	return m_flags == 0;
}

inline bool Blip::IsTemp() const
{
	return (m_flags & BF_TEMP) != 0;
}

inline Blip *BlipFactory::GetBlipAt(size_t index)
{
	// validate parm
	dbAssert(index < m_blips.size());

	return m_blips[index];
}
