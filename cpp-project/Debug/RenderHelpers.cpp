/////////////////////////////////////////////////////////////////////
// File    : RenderHelpers.cpp
// Desc    :
// Created : Tuesday, February 13, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

// This is a list of static utility functions to render out re-occuring debug info/primitives
///	to reduce code duplication.

#include "pch.h"
#include "RenderHelpers.h"

#include "../ModObj.h"
#include "../Simulation/RDNWorld.h"

#include <EngineAPI/TerrainOverlayInterface.h>

#include <SimEngine/TerrainHMBase.h>

#include <Util/DebugRender.h>

/////////////////////////////////////////////////////////////////////
//

void DrawCircleOnTerrain(const Vec3f &pos, const float radius)
{
	// Draw a circle using some default values.
	//	Draw a white circle using 32 divisions.

	DrawCircleOnTerrain(pos, radius, Colour(255, 255, 255, 255), 16, 0);
}

void DrawCircleOnTerrain(
		const Vec3f &pos,
		const float radius,
		const Colour col,
		const size_t divs,
		const bool // currently ignored. Was used in a previous version of this function.
)
{
	// Draws a circle on the terrain.
	//	Shelby's standard value for "divs" was 32.

	// Get the terrain pointer.
	const TerrainHMBase *pTer = ModObj::i()->GetWorld()->GetTerrain();

	// Draw the circle as a series of lines around the circumference.
	Vec3f vstart, v1, v2;

	float theta = (TWOPI);
	vstart.x = pos.x + radius * cosf(theta);
	vstart.z = pos.z + radius * sinf(theta);
	vstart.y = pTer->GetSurfaceHeight(vstart.x, vstart.z) + 0.3f;

	v1 = vstart;

	for (size_t i = 1; i < divs; ++i)
	{
		float theta = (TWOPI) * (divs - i) / divs;
		v2.x = pos.x + radius * cosf(theta);
		v2.z = pos.z + radius * sinf(theta);
		v2.y = pTer->GetSurfaceHeight(v2.x, v2.z) + 0.3f;

		DebugRender::Draw(DebugRender::Line(v1, v2, col), "TerrainLine");

		v1 = v2;
	}

	DebugRender::Draw(DebugRender::Line(v2, vstart, col), "TerrainLine");
}

void DrawRectOnTerrain(const Vec3f &pos, const Vec2f &extents, const float rads)
{
	DrawRectOnTerrain(pos, extents, rads, Colour(255, 255, 255, 255));
}

void DrawRectOnTerrain(const Vec3f &pos, const Vec2f &extents, const float rads, const Colour &col)
{
	// Draw a rotated rectangle on the terrain.

	float costheta = cosf(rads);
	float sintheta = sinf(rads);

	Matrix2f rot;

	rot.m00 = costheta;
	rot.m01 = sintheta;
	rot.m10 = -sintheta;
	rot.m11 = costheta;

	DrawRectOnTerrain(pos, extents, rot, col);
}

void DrawRectOnTerrain(const Vec3f &pos, const Vec2f &extents, const Matrix2f &rot)
{
	// Draw a rotated rectangle on the terrain.

	DrawRectOnTerrain(pos, extents, rot, Colour(255, 255, 255, 255));
}

// SHOULD write a DrawRectOnTerrain that takes a OBB2f instead of all threat vars...

void DrawRectOnTerrain(const Vec3f &pos, const Vec2f &extents, const Matrix2f &rot, const Colour &col)
{
	// Draw a rotated rectangle on the terrain.

	// Get the terrain pointer.
	const TerrainHMBase *pTer = ModObj::i()->GetWorld()->GetTerrain();

	Vec2f vtemp = Vec2f(extents.x, extents.y) * rot + Vec2f(pos.x, pos.z);
	Vec3f v1(vtemp.x, pTer->GetHeight(vtemp.x, vtemp.y) + 0.2f, vtemp.y);

	vtemp = Vec2f(extents.x, -extents.y) * rot + Vec2f(pos.x, pos.z);
	Vec3f v2(vtemp.x, pTer->GetHeight(vtemp.x, vtemp.y) + 0.2f, vtemp.y);

	vtemp = Vec2f(-extents.x, -extents.y) * rot + Vec2f(pos.x, pos.z);
	Vec3f v3(vtemp.x, pTer->GetHeight(vtemp.x, vtemp.y) + 0.2f, vtemp.y);

	vtemp = Vec2f(-extents.x, extents.y) * rot + Vec2f(pos.x, pos.z);
	Vec3f v4(vtemp.x, pTer->GetHeight(vtemp.x, vtemp.y) + 0.2f, vtemp.y);

	DebugRender::Draw(DebugRender::Line(v1, v2, col), "TerrainLine");
	DebugRender::Draw(DebugRender::Line(v2, v3, col), "TerrainLine");
	DebugRender::Draw(DebugRender::Line(v3, v4, col), "TerrainLine");
	DebugRender::Draw(DebugRender::Line(v4, v1, col), "TerrainLine");
}

/*
void DrawRectOnTerrain(const TerrainHMBase *pTer, const Vec3f &pos, const float x, const float z, const float rads)
{
	// Draw a circle using some default values.
	//	Draw a white circle using 32 divisions.

	DrawRectOnTerrain(pTer, pos, x, z, rads, 255, 255, 255, 0);
}

void DrawRectOnTerrain(const TerrainHMBase *pTer, const Vec3f &pos, const float x, const float z, const float rads, const unsigned char r, const unsigned char g, const unsigned char b, const bool bSolid)
{
	// Draws a circle on the terrain.
	//	Shelby's standard value for "divs" was 32.

	sp::SimpleVertex *sv = new sp::SimpleVertex[5];

	for( int j = 0; j < 5; j++ )
	{
		sv[j].clear();
		sv[j].diffuse = Colour(r,g,b,80);
	}
	// hint that no lighting will be used
	sv[0].normal = Vec3f(0,0,0);

	// This can be optimised a bit, by cache-ing some of the calculations.
	float costheta = cosf(rads);
	float sintheta = sinf(rads);

	sv[0].vertex.x = pos.x + x*costheta + z*sintheta;
	sv[0].vertex.z = pos.z + -x*sintheta + z*costheta;
	sv[0].vertex.y = pTer->GetHeight(sv[0].vertex.x, sv[0].vertex.z)+0.02f;

	sv[1].vertex.x = pos.x + x*costheta - z*sintheta;
	sv[1].vertex.z = pos.z + -x*sintheta - z*costheta;
	sv[1].vertex.y = pTer->GetHeight(sv[1].vertex.x, sv[1].vertex.z)+0.02f;

	sv[2].vertex.x = pos.x + -x*costheta - z*sintheta;
	sv[2].vertex.z = pos.z + x*sintheta - z*costheta;
	sv[2].vertex.y = pTer->GetHeight(sv[2].vertex.x, sv[2].vertex.z)+0.02f;

	sv[3].vertex.x = pos.x + -x*costheta + z*sintheta;
	sv[3].vertex.z = pos.z + x*sintheta + z*costheta;
	sv[3].vertex.y = pTer->GetHeight(sv[3].vertex.x, sv[3].vertex.z)+0.02f;

	sv[4].vertex = sv[0].vertex;

	if(bSolid)
		sp::Interface::RenderSimple( NULL, sp::PT_TriangleFan, sv, 5 );
	else
		sp::Interface::RenderSimple( NULL, sp::PT_LineStrip, sv, 5 );

	delete [] sv;
}


void DrawRectOnTerrain(const TerrainHMBase *pTer, const Vec3f &pos, const float x, const float z, const Vec3f &lookat)
{
	DrawRectOnTerrain(pTer, pos, x, z, lookat, 255, 255, 255, 0);
}


void DrawRectOnTerrain(const TerrainHMBase *pTer, const Vec3f &pos, const float x, const float z, const Vec3f &lookat, const unsigned char r, const unsigned char g, const unsigned char b, const bool bSolid)
{
	float theta = acosf(lookat%Vec3f(0.0f,0.0f,1.0f));
	if(lookat%Vec3f(1.0f,0.0f,0.0f) < 0.0f)
		theta = -theta;

	DrawRectOnTerrain(pTer, pos, x, z, theta, r, g, b, bSolid);
}


void DrawRotCircleOnTerrain(const TerrainHMBase *pTer, const Vec3f &pos, const float radius, const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a_centre, const unsigned char a_outer, const size_t divs, const float rads)
{
	// Draws a circle on the terrain.
	//	Shelby's standard value for "divs" was 32.

	std::vector< sp::SimpleVertex > sv( divs+2 );

	for( size_t j = 0; j != sv.size(); ++j )
	{
		sv[j].clear();
		sv[j].diffuse = Colour(r,g,b,a_outer);
	}
	// hint that no lighting will be used
	sv[0].normal = Vec3f(0,0,0);
	sv[0].diffuse.a = a_centre;

	sv[0].vertex.x = pos.x;
	sv[0].vertex.z = pos.z;
	sv[0].vertex.y = pTer->GetHeight(sv[0].vertex.x, sv[0].vertex.z)+0.02f;

	for( size_t i = 1; i < divs+1; ++i )
	{
		float theta = (TWOPI)*(divs-(i-1))/divs + rads;

		sv[i].vertex.x = pos.x + radius*cosf(theta);
		sv[i].vertex.z = pos.z + radius*sinf(theta);
		sv[i].vertex.y = pTer->GetHeight(sv[i].vertex.x, sv[i].vertex.z)+0.02f;
	}

	sv[divs+1].vertex = sv[1].vertex;

	sp::Interface::RenderSimple( NULL, sp::PT_TriangleFan, sv.begin(), sv.size() );
}

void DrawScreenAlignedCircle(const Vec3f &pos, const float radius, const Colour col, const size_t divs, const bool bSolid)
{
	Matrix4f worldView;
	sp::Interface::GetMatrix( sp::MC_WorldView0, worldView );

	Vec3f up, right;
	right.x = worldView.m00;
	right.y = worldView.m10;
	right.z = worldView.m20;
	//	right.NormalizeSelf();
	up.x = worldView.m01;
	up.y = worldView.m11;
	up.z = worldView.m21;
	//	up.NormalizeSelf();

	size_t j;
	std::vector<sp::SimpleVertex> cVertex(divs+1);

	sp::SimpleVertex sv, sv1;
	cVertex.clear();

	Vec3f centre = (Vec3f&)pos;
	sv1.vertex = centre;
	sv1.diffuse = col;
	cVertex.push_back(sv1);

	for ( j=0; j<divs+1; ++j )
	{
		float angle = PI*2*j/divs;
		sv.vertex = centre + (up*cosf( angle )*radius) + (right*sinf( angle )*radius);
		sv.diffuse = col;
		cVertex.push_back(sv);
	}

//	cVertex.push_back(sv1);
	if ( bSolid )
		sp::Interface::RenderSimple(NULL, sp::PT_TriangleFan, &cVertex[0], boost::numeric_cast< unsigned short >( cVertex.size() ) );
	else
		sp::Interface::RenderSimple(NULL, sp::PT_TriangleFan, &cVertex[1], boost::numeric_cast< unsigned short >( cVertex.size() )-1 );
}


// Terrain Line helper class

/////////////////////////////////////////////////////////////////////
//	Name	:
//	Desc.	:
//	Param.	:
//	Result	:
//
TerrainLine::TerrainLine(const TerrainHMBase *pTer, float stepsize)
{
	m_pTer = pTer;

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
TerrainLine::TerrainLine(const TerrainHMBase *pTer, const Vec2f &p1, const Vec2f &p2, const Colour col, float stepsize)
{
	m_pTer = pTer;

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

	height = m_pTer->GetHeight(xzpos.x, xzpos.y);

	// the line is always draw 1/20 of a meter (5 cm) above the ground
	return (height+0.05f);
}


/////////////////////////////////////////////////////////////////////
//	Name	:
//	Desc.	:
//	Param.	:
//	Result	:
//
void TerrainLine::Render( void ) const
{
	if (!m_valid)
	{
		CreatePrimitives();
	}

	sp::Interface::RenderSimple(NULL, sp::PT_LineStrip, &m_linelist[0], m_linelist.size());
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
void TerrainLine::SetPoints(const Vec2f &p1, const Vec2f &p2, const Colour col)
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
void TerrainLine::SetColour(Colour col)
{
	if (m_col != col && m_valid)
	{
		// we just need to update the colour not the vertices
		for( size_t i=0; i < m_linelist.size(); i++)
		{
			m_linelist[i].diffuse = col;
		}
	}
}

/////////////////////////////////////////////////////////////////////
//	Name	:
//	Desc.	:
//	Param.	:
//	Result	:
//
void TerrainLine::CreatePrimitives( void ) const
{
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
}

*/

/////////////////////////////////////////////////////////////////////
// debug terrain overlay stuff

static std::vector<unsigned char> s_grid;
static std::vector<Colour> s_palette;
static int s_gridW = 0;
static int s_gridH = 0;

void DrawTerrainOverlayFill(size_t palIndex)
{
	// area
	Vec3f tl(FLT_MIN, FLT_MIN, FLT_MIN);
	Vec3f br(FLT_MAX, FLT_MAX, FLT_MAX);

	ModObj::i()->GetWorld()->ClampPointToWorld(tl);
	ModObj::i()->GetWorld()->ClampPointToWorld(br);

	//
	DrawTerrainOverlayRect(palIndex, Rect2f(tl.x, br.x, tl.z, br.z));

	return;
}

bool DrawTerrainOverlayUpdate()
{
	if (s_grid.empty() || s_palette.empty())
		return false;

	//
	return ModObj::i()->GetTerrainOverlayInterface()->DrawGrid(
			&s_grid[0],
			s_gridW,
			s_gridH,
			&s_palette[0],
			s_palette.size());
}

void DrawTerrainOverlayPalette(size_t palIndex, const Colour &col)
{
	// validate parm
	if (palIndex > UCHAR_MAX)
	{
		dbFatalf("RDNDllSetup::DrawTerrainOverlayPalette failed");
		return;
	}

	// allocate palette
	if (s_palette.empty())
	{
		s_palette.resize(UCHAR_MAX, Colour::BuildFrom(1.0f, 1.0f, 1.0f, 0.0f));
	}

	//
	s_palette[palIndex] = col;

	return;
}

static void CreateOverlay()
{
	// creates overlay based on world coords
	const float islandW = ModObj::i()->GetWorld()->GetTerrain()->GetIslandWidth();
	const float islandH = ModObj::i()->GetWorld()->GetTerrain()->GetIslandLength();

	s_gridW = (int)(islandW * 2.0f);
	s_gridH = (int)(islandH * 2.0f);

	s_grid.resize(s_gridW * s_gridH, 0);
}

void DrawTerrainOverlayRect(size_t palIndex, const Rect2f &area)
{
	// allocate grid
	if (s_grid.empty())
	{
		CreateOverlay();
	}

	//
	Rect2f r(area);
	r.Sort();

	// offset
	const float islandW = ModObj::i()->GetWorld()->GetTerrain()->GetIslandWidth();
	const float islandH = ModObj::i()->GetWorld()->GetTerrain()->GetIslandLength();

	r.xmin += (islandW / 2.0f);
	r.xmax += (islandW / 2.0f);
	r.ymin += (islandH / 2.0f);
	r.ymax += (islandH / 2.0f);

	// scale
	r.xmax *= float(s_gridW) / islandW;
	r.ymax *= float(s_gridH) / islandH;
	// clamp area to world coord
	if (r.xmin < 0.0f)
		r.xmin = 0.0f;

	if (r.ymin < 0.0f)
		r.ymin = 0.0f;

	if (r.xmax >= float(s_gridW))
		r.xmax = float(s_gridW) - 1;

	if (r.ymax >= float(s_gridH))
		r.ymax = float(s_gridH) - 1;

	//
	const Rect2i rInt(
			int(floorf(r.xmin)), int(floorf(r.xmax)),
			int(floorf(r.ymin)), int(floorf(r.ymax)));

	// fill area
	for (int y = rInt.ymin; y != rInt.ymax; ++y)
	{
		unsigned char *p = &s_grid[0] + (y * s_gridW) + rInt.xmin;
		std::fill(p, p + rInt.xmax - rInt.xmin, static_cast<unsigned char>(palIndex));
	}

	return;
}

void DrawTerrainOverlayPixel(size_t palIndex, const long x, const long z)
{
	// allocate grid
	if (s_grid.empty())
	{
		CreateOverlay();
	}

	dbAssert(x >= 0 && x < s_gridW);
	dbAssert(z >= 0 && z < s_gridH);
	dbAssert(palIndex <= UCHAR_MAX);

	s_grid[x + z * s_gridW] = static_cast<unsigned char>(palIndex);
}

void DrawTerrainOverlayPixel(size_t palIndex, const float worldx, const float worldz)
{
	// convert world xz to texture xz
	const float islandW = ModObj::i()->GetWorld()->GetTerrain()->GetIslandWidth() * 0.5f;
	const float islandH = ModObj::i()->GetWorld()->GetTerrain()->GetIslandLength() * 0.5f;

	long x = static_cast<long>((worldx + islandW) * 2.0f);
	long z = static_cast<long>((worldz + islandH) * 2.0f);

	DrawTerrainOverlayPixel(palIndex, x, z);

	return;
}

/////////////////////////////////////////////////////////////////////
//	Desc.	: draws a pixel on the terrain as if it were a given resolution.
//	Result	:
//	Param.	:
//	Author	: dswinerd
//
void DrawTerrainOverlayPixelAtResolution(size_t palIndex, const long x, const long z, const long resX, const long resZ)
{
	// allocate grid
	if (s_grid.empty())
	{
		CreateOverlay();
	}

	dbAssert(x >= 0 && x < s_gridW);
	dbAssert(z >= 0 && z < s_gridH);
	dbAssert(palIndex <= UCHAR_MAX);

	int fillW = s_gridW / resX;
	int fillH = s_gridH / resZ;

	for (int h = 0; h < fillH; h++)
	{
		int row = z * fillH + h;
		for (int w = 0; w < fillW; w++)
		{
			int col = x * fillW + w;
			s_grid[row * s_gridW + col] = (unsigned char)palIndex;
		}
	}
}
