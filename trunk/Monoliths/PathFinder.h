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
	enum MutualPoint
	{
		MP_UNDEFINED,
		MP_LEFT,
		MP_RIGHT
	};

	NavMeshTriangle* from;
	NavMeshTriangle* to;
	int leftVertex;
	int rightVertex;
	MutualPoint mutualPoint;

	PathEdge(NavMeshTriangle* from, NavMeshTriangle* to, int leftVertex, int rightVertex)
		: from(from), to(to), leftVertex(leftVertex), rightVertex(rightVertex), mutualPoint(MP_UNDEFINED)
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
	NavMesh* _navMesh;
	NavMeshTriangle* _first;
	NavMeshTriangle* _last;
	std::vector<PathEdge> _edges;
	std::vector<NavMeshTriangle*> _triangles;
	
public:
	TrianglePath(const std::vector<NavMeshTriangle*>& triangles, NavMesh* navMesh)
		: _triangles(triangles), 
		_first(triangles[0]), 
		_last(triangles.back()), 
		_navMesh(navMesh)
	{
		for (int i=0;i<triangles.size()-1;i++)
		{
			PathEdge edge = PathEdge::create(triangles[i], triangles[i+1], navMesh);
			_edges.push_back(edge);
		}

		for (int i=0;i<(int)_edges.size()-1;i++)
		{
			PathEdge& prev = _edges[i];
			PathEdge& act = _edges[i+1];
			act.mutualPoint = act.leftVertex == prev.leftVertex ? PathEdge::MP_LEFT : PathEdge::MP_RIGHT;
			if (act.mutualPoint == PathEdge::MP_RIGHT)
			{
				assert(act.rightVertex == prev.rightVertex);
			}
		}
	}

	NavMesh* getNavMesh() const
	{
		return _navMesh;
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

	static bool isLeft(const Vector2& a, const Vector2& b)
	{
		return a.crossProduct(b) > 0;
	}

	Vector2 nextCorner(const Vector2& from, const Vector2& to)
	{
		if (_edges.size() == 0)
		{
			return to;
		}
		auto i = _edges.begin();
		Vector2 left = i->getLeftPoint(_navMesh);
		Vector2 right = i->getRightPoint(_navMesh);
		Vector2 leftDir = left - from;
		Vector2 rightDir = right - from;
		i++;
		for (;i!=_edges.end();i++)
		{
			if (i->mutualPoint == PathEdge::MP_RIGHT)
			{
				const Vector2& left0 = i->getLeftPoint(_navMesh);
				const Vector2& leftDir0 = left0 - from;
				if (!isLeft(leftDir0, rightDir)) break;
				if (isLeft(leftDir, leftDir0))
				{
					left = left0;
					leftDir = leftDir0;
				}
			}
			else
			{
				assert(i->mutualPoint == PathEdge::MP_LEFT);

				const Vector2& right0 = i->getRightPoint(_navMesh);
				const Vector2& rightDir0 = right0 - from;
				if (isLeft(rightDir0, leftDir)) break;
				if (isLeft(rightDir0, rightDir))
				{
					right = right0;
					rightDir = rightDir0;
				}
			}
		}
		if (i == _edges.end())
		{
			Vector2 toDir = to-from;
			if (isLeft(toDir,rightDir) && isLeft(leftDir, toDir))
			{
				return to;
			}
		}

		//float l = left.distance(from)+left.distance(to);
		//float r = right.distance(from)+right.distance(to);
		float l = left.distance(to);
		float r = right.distance(to);

		return l < r ? left : right;
	}
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