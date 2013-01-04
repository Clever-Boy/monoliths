#pragma once
#include "stdafx.h"
#include "common.h"

using namespace Ogre;
using namespace ClipperLib;



class NavMesh
{
	Polygons _polygons;

public:

	void addPolygon(Ogre::Vector2* polygonVertices, int vertexCount);

	void init(const Ogre::Vector2& mapSize);
};