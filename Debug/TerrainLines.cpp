// TerrainLines.cpp

#include "pch.h"
#include "TerrainLines.h"

#include "../ModObj.h"
#include "../Simulation/RDNWorld.h"

#include <math/Vec2.h>
#include <math/Vec3.h>

#include <SimEngine/TerrainHMBase.h>
#include <Util/DebugRender.h>

// Terrain Line helper class

/////////////////////////////////////////////////////////////////////
//	Name	: 
//	Desc.	: 
//	Param.	: 
//	Result	: 
//
TerrainLine::TerrainLine( const float stepsize )
{
	m_p1 = Vec2f(FLT_MAX, FLT_MAX);
	m_p2 = Vec2f(FLT_MAX, FLT_MAX);
	m_col = Colour(0,0,0,0);
	m_valid = false;

	m_accuracy = stepsize;
}

/////////////////////////////////////////////////////////////////////
//	Name	: 
//	Desc.	: 
//	Param.	: 
//	Result	: 
//
TerrainLine::TerrainLine( const Vec2f &p1, const Vec2f &p2, const Colour &col, const float stepsize )
{
	m_p1 = Vec2f(FLT_MAX, FLT_MAX);
	m_p2 = Vec2f(FLT_MAX, FLT_MAX);
	m_col = Colour(0,0,0,0);
	m_valid = false;

	SetPoints(p1, p2, col);

	m_accuracy = stepsize;
}

/////////////////////////////////////////////////////////////////////
//	Name	: 
//	Desc.	: 
//	Param.	: 
//	Result	: 
//
TerrainLine::~TerrainLine()
{
	m_linelist.clear();
}

float TerrainLine::GetHeight( const Vec2f &xzpos) const
{
	float height;

	height = ModObj::i()->GetWorld()->GetTerrain()->GetHeight(xzpos.x, xzpos.y);

	// the line is always draw 1/10 of a meter (10 cm) above the ground
	return (height+0.10f);
}


/////////////////////////////////////////////////////////////////////
//	Name	: 
//	Desc.	: 
//	Param.	: 
//	Result	: 
//
void TerrainLine::Render( void )
{
	if (!m_valid)
		CreatePrimitives();

	if(m_linelist.size() < 2)
		return;

	std::vector<Vec3f>::const_iterator iter_prev = m_linelist.begin();
	std::vector<Vec3f>::const_iterator iter = iter_prev + 1;
	const std::vector<Vec3f>::const_iterator iter_end = m_linelist.end();

	// Loop over all edges, and find which one this point is closest to.
	for(;iter != iter_end; iter_prev = iter, ++iter)
	{
		// Render all the sub-lines to make up the terrain hugging line.
		DebugRender::Draw( DebugRender::Line( *iter_prev, *iter, m_col ) );
	}
}

/////////////////////////////////////////////////////////////////////
//	Name	: 
//	Desc.	: 
//	Param.	: 
//	Result	: 
//
void TerrainLine::SetPoints(const Vec2f &p1, const Vec2f &p2)
{
	if (m_p1 != p1 || m_p2 != p2)
	{
		m_valid = false;
		m_p1	= p1;
		m_p2	= p2;
	}
}

/////////////////////////////////////////////////////////////////////
//	Name	: 
//	Desc.	: 
//	Param.	: 
//	Result	: 
//
void TerrainLine::SetPoints(const Vec2f &p1, const Vec2f &p2, const Colour &col)
{
	if (m_p1 != p1 || m_p2 != p2)
	{
		m_valid = false;
		m_p1	= p1;
		m_p2	= p2;
		m_col	= col;
	}

	SetColour(col);
}

/////////////////////////////////////////////////////////////////////
//	Name	: 
//	Desc.	: 
//	Param.	: 
//	Result	: 
//
void TerrainLine::SetColour(const Colour &col)
{
	m_col = col;
}

/////////////////////////////////////////////////////////////////////
//	Name	: 
//	Desc.	: 
//	Param.	: 
//	Result	: 
//
void TerrainLine::CreatePrimitives( void )
{
	// Create a vector of Vec3's that can be drawn by DebugRender.
	Vec3f	newPt;
	Vec2f	start, delta;
	float	t_inc, len, curh, lasth;
	size_t	count;

	m_linelist.resize(0);

	// The (x,z) start position
	start.x = m_p1.x;
	start.y = m_p1.y;

	len = (m_p2-m_p1).Length();

	// the step increment ammount for the line
	float accuracy = m_accuracy;
	count = (size_t)(len/accuracy);

	m_linelist.reserve(count+2);

	// what the increment ammount along the line should be
	t_inc = accuracy; //accuracy/len;

	delta = (m_p2-m_p1).Normalize();
	delta *= t_inc;

	// Add the first point to the list.
	newPt.x = start.x;
	lasth = GetHeight(start);
	newPt.y = lasth;
	newPt.z = start.y;

	m_linelist.push_back(newPt);

	// Add all the "middle points" to the list.
	//	Note: if m_accuracy is greater than length, there will be 0 middle points.
	int skip_point = 0;

	Vec2f curpos = start + delta;
	Vec2f lastpos;

	for(size_t i=0;i<count;i++)
	{
		curh = GetHeight(curpos);

		if (curh == lasth)
		{
			// skip this point.
			skip_point = 1;
		}
		else
		{
			// Add this point, and also the previous point if we skipped it.

			if(skip_point)
			{
				// Add the previous point to the list.
				newPt.x = lastpos.x;
				newPt.y = lasth;
				newPt.z = lastpos.y;

				m_linelist.push_back(newPt);
				skip_point = 0;
			}

			// Add the current point to the list.
			newPt.x = curpos.x;
			newPt.y = curh;
			newPt.z = curpos.y;

			m_linelist.push_back(newPt);

			lasth = curh;
		}

		lastpos = curpos;
		curpos += delta;
	}

	// Add the last point to the list.
	newPt.x = m_p2.x;
	newPt.y = GetHeight(m_p2);
	newPt.z = m_p2.y;
	m_linelist.push_back(newPt);

	m_valid = true;


/*
	sp::SimpleVertex	spVec;
	Vec2f				start, delta;
	float				t_inc, len, curh, lasth;
	size_t				count;

	m_linelist.clear();
	
	// The (x,z) start position
	start.x = m_p1.x;
	start.y = m_p1.y;

	len = (m_p2-m_p1).Length();

	// the step increment ammount for the line
	float accuracy = m_accuracy;
	count = len/accuracy;

	m_linelist.reserve(count+2);

	// what the increment ammount along the line should be
	t_inc = accuracy; //accuracy/len;

//	delta.x = (m_p2.x-m_p1.x)*t_inc;
//	delta.y = (m_p2.y-m_p1.y)*t_inc;
	delta = (m_p2-m_p1).Normalize();
	delta *= t_inc;

//	m_linelist.clear();

	spVec.clear();
	spVec.normal = Vec3f(0.0f,0.0f,0.0f);
	spVec.diffuse = m_col;


	// Add the first point to the list.
	spVec.vertex.x = start.x;
	lasth = GetHeight(start);
	spVec.vertex.y = lasth;
	spVec.vertex.z = start.y;

	m_linelist.push_back(spVec);

	// Add all the "middle points" to the list.
	//	Note: if m_accuracy is greater than length, there will be 0 middle points.
	int skip_point = 0;

	Vec2f curpos = start + delta;
	Vec2f lastpos;

	for(size_t i=0;i<count;i++)
	{
		curh = GetHeight(curpos);

		if (curh == lasth)
		{
			// skip this point.
			skip_point = 1;
		}
		else
		{
			// Add this point, and also the previous point if we skipped it.

			if(skip_point)
			{
				// Add the previous point to the list.
				spVec.vertex.x = lastpos.x;
				spVec.vertex.y = lasth;
				spVec.vertex.z = lastpos.y;

				m_linelist.push_back(spVec);
				skip_point = 0;
			}

			// Add the current point to the list.
			spVec.vertex.x = curpos.x;
			spVec.vertex.y = curh;
			spVec.vertex.z = curpos.y;

			m_linelist.push_back(spVec);

			lasth = curh;
		}

		lastpos = curpos;
		curpos += delta;
	}

	// Add the last point to the list.
	spVec.vertex.x = m_p2.x;
	spVec.vertex.y = GetHeight(m_p2);
	spVec.vertex.z = m_p2.y;
	m_linelist.push_back(spVec);

	m_valid = true;
*/
}

