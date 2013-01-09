#pragma once
#include "stdafx.h"
#include "common.h"

using namespace Ogre;
using namespace ClipperLib;

class NavMesh;
struct NavMeshTriangle;


class NavmeshGraphFunctionContainer : public SearchGraphDescriptorFunctionContainer<NavMeshTriangle*, float>
{
public:
	virtual int getHashBin(NavMeshTriangle*& n) // Use the absolute value of x coordinate as hash bin counter. Not a good choice though!
	{
		return ((int)n) % 128;
	}

	virtual bool isAccessible(NavMeshTriangle*& n)
	{
		return true;
	}

	virtual void getSuccessors(NavMeshTriangle*& n, std::vector<NavMeshTriangle*>* s, std::vector<float>* c);

	virtual float getHeuristics(NavMeshTriangle*& n1, NavMeshTriangle*& n2);
};


struct TriangleConnection
{
	NavMeshTriangle* tri0;
	NavMeshTriangle* tri1;
	float distance;

	TriangleConnection(NavMeshTriangle* tri0);

	void setTri1(NavMeshTriangle* tri1);

	NavMeshTriangle* getOppositeTriangle(NavMeshTriangle* tri)
	{
		return tri == tri0 ? tri1 : tri0;
	}
};

struct NavMeshTriangle
{
	int idx0;
	int idx1;
	int idx2;

	Ogre::Vector2 center;
	std::vector<TriangleConnection*> connections;

	NavMeshTriangle(const NavMeshTriangle& other)
		: idx0(other.idx0), idx1(other.idx1), idx2(other.idx2),
		  center(other.center), connections(other.connections)
	{
	}

	NavMeshTriangle(const NavMesh& navMesh, int i0, int i1, int i2);

	void addConnection(TriangleConnection* connection)
	{
		connections.push_back(connection);
	}

	bool operator==(const NavMeshTriangle& other)
	{
		return this == &other;
	}

	const Ogre::Vector2& getPosition(int i, const NavMesh& navMesh);

	~NavMeshTriangle()
	{
		/*OutputDebugString("NEEE PUSZULOOOK\n");
		idx0 = -42;
		idx1 = -42;
		idx2 = -42;*/
	}

	bool containsPoint(const Ogre::Vector2& p, const NavMesh& navMesh);

};

class NavMesh
{
	Polygons _polygons;
	
	std::vector<Ogre::Vector2> _vertices;
	std::vector<NavMeshTriangle> _triangles;
	std::vector<TriangleConnection*> _triangleConnections;

	NavmeshGraphFunctionContainer _functionContainer;

public:

	const std::vector<Ogre::Vector2>& getVertices() const
	{
		return _vertices;
	}

	const std::vector<NavMeshTriangle>& getTriangles() const	
	{
		return _triangles;
	}

	const std::vector<TriangleConnection*>& getTriangleConnections() const
	{
		return _triangleConnections;
	}

	void addPolygon(Ogre::Vector2* polygonVertices, int vertexCount);
	void init(const Ogre::Vector2& mapSize);

	const Ogre::Vector2& getVertex(int idx) const
	{
		return _vertices[idx];
	}

	std::vector<NavMeshTriangle*> findPathBetween(const NavMeshTriangle* a, const NavMeshTriangle* b);

	NavMeshTriangle* findTriangleOfPoint(const Ogre::Vector2& point)
	{
		for (auto i = _triangles.begin(); i != _triangles.end(); i++)
		{
			if (i->containsPoint(point, *this))
			{
				return &(*i);
			}
		}
		return NULL;
	}
};



//88 426 011

//	1445