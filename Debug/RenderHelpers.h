/////////////////////////////////////////////////////////////////////
// File    : SigmaRenderHelpers.h
// Desc    :
// Created : Thursday, November 08, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#pragma once

#include <Math/Vec3.h>
#include <Math/Vec2.h>

#include <SurfVol/Rec2.h>

#include <Util/colour.h>

/////////////////////////////////////////////////////////////////////
// debug terrain render stuff

// * this stuff is very slow, as it uses the debug render system

void DrawCircleOnTerrain(const Vec3f &pos, const float radius);
void DrawCircleOnTerrain(
		const Vec3f &pos,
		const float radius,
		const Colour Col,
		const size_t divs,
		const bool bSolid);

//--
// SHOULD also write a DrawRectOnTerrain that takes a OBB2f instead of all threat vars...
//--

void DrawRectOnTerrain(const Vec3f &pos, const Vec2f &extents, const float rads);
void DrawRectOnTerrain(const Vec3f &pos, const Vec2f &extents, const float rads, const Colour &col);
void DrawRectOnTerrain(const Vec3f &pos, const Vec2f &extents, const Matrix2f &rot);
void DrawRectOnTerrain(const Vec3f &pos, const Vec2f &extents, const Matrix2f &rot, const Colour &col);

/////////////////////////////////////////////////////////////////////
// debug terrain overlay stuff

// * this stuff uses the terrain overlay system, which is pretty fast to render,
// * but slow to update.

// clear all to predefined colour
void DrawTerrainOverlayFill(size_t palIndex);

// send all update to the texture
bool DrawTerrainOverlayUpdate();

// setup the palette (256 entries)
void DrawTerrainOverlayPalette(size_t palIndex, const Colour &col);

// fill an area with specified palette index
void DrawTerrainOverlayRect(size_t palIndex, const Rect2f &area);

// Set the pixel at location x, z to the col specicfied, x and z are in texture coords, i.e. 0,0 is top left corner, 512,512 is bottom right
void DrawTerrainOverlayPixel(size_t palIndex, const long x, const long z);

// Set the pixel at location worldx worldz to the col specified.
void DrawTerrainOverlayPixel(size_t palIndex, const float worldx, const float worldz);

// Set the pixel at location x, z to the col specicfied, x and z are in texture coords, i.e. 0,0 is top left corner, resX,resZ is bottom right
void DrawTerrainOverlayPixelAtResolution(size_t palIndex, const long x, const long z, const long resX, const long resZ);