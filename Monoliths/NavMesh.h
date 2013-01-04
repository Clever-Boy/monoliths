#pragma once
#include "stdafx.h"
#include "common.h"

using namespace Ogre;
using namespace ClipperLib;

const float NAVMESH_POLY_SCALE = 1;

class NavMesh
{
	Polygons _positivePolygons;
	Polygons _negativePolygons;

	void AddPolygon2(Ogre::Vector2* polygonVertices, int vertexCount, Polygons& polygons)
	{
		ClipperLib::Polygon polygon;
		for (int i = 0; i < vertexCount; i++)
		{
			Vector2 v = polygonVertices[i]*NAVMESH_POLY_SCALE;
			IntPoint p = IntPoint((long64)v.x, (long64)v.y);
			polygon.push_back(p);
		}
		polygons.push_back(polygon);
	}

public:

	void addPositivePolygon(Ogre::Vector2* polygonVertices, int vertexCount)
	{
		AddPolygon2(polygonVertices, vertexCount, _positivePolygons);		
	}

	void addNegativePolygon(Ogre::Vector2* polygonVertices, int vertexCount)
	{
		AddPolygon2(polygonVertices, vertexCount, _positivePolygons);
	}
};