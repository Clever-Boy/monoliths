#pragma once
#include "stdafx.h"
#include "common.h"
#include "NavMesh.h"

using namespace Ogre;



class TrianglePair
{
	const NavMeshTriangle* _tri1;
	const NavMeshTriangle* _tri2;

public:
	//friend class Hasher;

	class Hasher
	{
	public:
		size_t operator()(const TrianglePair& pair) const
		{
			return ((size_t)pair._tri1)*31+(size_t)pair._tri2;
		};
	};

	TrianglePair(const NavMeshTriangle* tri1, const NavMeshTriangle* tri2)
		: _tri1(tri1), _tri2(tri2)
	{
	}

	bool operator ==(const TrianglePair& other) const
	{
		return _tri1 == other._tri1 && _tri2 == other._tri2 ||
			_tri1 == other._tri2 && _tri2 == other._tri1;
	}
};

struct PathEdge
{
	NavMeshTriangle* from;
	NavMeshTriangle* to;
	int leftVertex;
	int rightVertex;

	PathEdge(NavMeshTriangle* from, NavMeshTriangle* to, int leftVertex, int rightVertex)
		: from(from), to(to), leftVertex(leftVertex), rightVertex(rightVertex)
	{
	}

	const Ogre::Vector2& getLeftPoint(const NavMesh* navMesh) const
	{
		return navMesh->getVertex(leftVertex);
	}

	const Ogre::Vector2& getRightPoint(const NavMesh* navMesh) const
	{
		return navMesh->getVertex(rightVertex);
	}

	static PathEdge create(NavMeshTriangle* from, NavMeshTriangle* to, NavMesh* navMesh)
	{
		TriangleConnection* conn = TriangleConnection::getConnectionBeetween(from, to);
		int left = conn->vIdxA;
		int right = conn->vIdxB;
		if (left == from->idx0 && right == from->idx1 ||
			left == from->idx1 && right == from->idx2 ||
			left == from->idx2 && right == from->idx0)
		{
			std::swap(left, right);
		}

		return PathEdge(from, to, left, right);
	}

	
};

class TrianglePath
{
	NavMeshTriangle* _first;
	NavMeshTriangle* _last;
	std::vector<PathEdge> _edges;
	std::vector<NavMeshTriangle*> _triangles;
	
public:
	TrianglePath(const std::vector<NavMeshTriangle*>& triangles, NavMesh* navMesh)
		: _triangles(triangles), _first(triangles[0]), _last(triangles.back())
	{
		for (int i=0;i<triangles.size()-1;i++)
		{
			PathEdge edge = PathEdge::create(triangles[i], triangles[i+1], navMesh);
			_edges.push_back(edge);
		}
	}

	const std::vector<PathEdge>& getEdges() const
	{
		return _edges;
	}

	const std::vector<NavMeshTriangle*>& getTriangles() const
	{
		return _triangles;
	}

	NavMeshTriangle* firstTriangle() { return _first; }
	NavMeshTriangle* lastTriangle() { return _last; }
};

class PathFinder
{
	typedef std::unordered_map<TrianglePair, TrianglePath*, TrianglePair::Hasher> TMap;
	NavMesh* _navMesh;
	TMap _map;

public:

	PathFinder(NavMesh* navMesh)
		: _navMesh(navMesh)
	{
	}

	TrianglePath* findPathBeetween(const NavMeshTriangle* tri1, const NavMeshTriangle* tri2)
	{
		TrianglePath* result;
		TrianglePair pair(tri1, tri2);
		TMap::iterator bitch = _map.find(pair);
		if (bitch != _map.end())
		{
			result = bitch->second;
		}
		else
		{
			std::vector<NavMeshTriangle*> triangles = _navMesh->findPathBetween(tri1, tri2);
			result = new TrianglePath(triangles, _navMesh);
			_map[pair] = result;
		}
		return result;
	}
};