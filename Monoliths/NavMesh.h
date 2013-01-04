#pragma once
#include "stdafx.h"
#include "common.h"

using namespace Ogre;
using namespace ClipperLib;

class NavMesh
{
	Polygons _polygons;
	
	std::vector<Ogre::Vector2> _vertices;
	std::vector<int> _triangles;

public:

	const std::vector<Ogre::Vector2>& getVertices() const
	{
		return _vertices;
	}

	const std::vector<int> getTriangles() const
	{
		return _triangles;
	}

	void addPolygon(Ogre::Vector2* polygonVertices, int vertexCount);
	void init(const Ogre::Vector2& mapSize);
};