// TerrainLines.h

#pragma once

#include <Util/Colour.h>

class TerrainLine
{
public:
	TerrainLine(const float stepsize = 0.5f);
	TerrainLine(const Vec2f &p1, const Vec2f &p2, const Colour &col, const float stepsize = 0.5f);

	~TerrainLine();

	void Render(void);

	void SetPoints(const Vec2f &p1, const Vec2f &p2);
	void SetPoints(const Vec2f &p1, const Vec2f &p2, const Colour &col);

	void SetColour(const Colour &col);

private:
	// Inputs.
	Vec2f m_p1, m_p2;
	Colour m_col;
	float m_accuracy; // Used for computing the sub-lines.

	// calculated positions for the lines to be drawn.
	std::vector<Vec3f> m_linelist;
	mutable bool m_valid;

	void CreatePrimitives(void);
	float GetHeight(const Vec2f &xzpos) const;
};
