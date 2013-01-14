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

class TrianglePath
{
	NavMeshTriangle* _first;
	NavMeshTriangle* _last;
	std::vector<TriangleConnection*> _connections;
	std::vector<NavMeshTriangle*> _triangles;
	
	static void getLeftRight(Vector2& left, Vector2& right, Vector2& leftDir,Vector2& rightDir, const NavMesh* navMesh, const Vector2& from, const std::vector<TriangleConnection*>::iterator& i)
	{
		left = (*i)->getPointA(navMesh); right = (*i)->getPointB(navMesh);
		leftDir = left - from;
		rightDir = right - from;
		
		leftDir.x = -leftDir.x; // mert az X tengely balra mutat ebben a beteg univerzumban
		rightDir.x = -rightDir.x;

		if (leftDir.crossProduct(rightDir) < 0)
		{
			std::swap(left, right);
			std::swap(leftDir, rightDir);
		}
	}


public:
	TrianglePath(const std::vector<NavMeshTriangle*>& triangles)
		: _triangles(triangles), _first(triangles[0]), _last(triangles.back())
	{
		for (int i=0;i<triangles.size()-1;i++)
		{
			TriangleConnection* connection = TriangleConnection::getConnectionBeetween(triangles[i], triangles[i+1]);
			_connections.push_back(connection);
		}
	}

	const std::vector<TriangleConnection*>& getConnections()
	{
		return _connections;
	}

	const std::vector<NavMeshTriangle*>& getTriangles()
	{
		return _triangles;
	}

	NavMeshTriangle* firstTriangle() { return _first; }

	NavMeshTriangle* lastTriangle() { return _last; }

	Vector2 nextCorner(const Vector2& from, const Vector2& dest, const NavMesh* navMesh)
	{
		auto i = _connections.begin();
		Vector2 left, right, leftDir, rightDir;
		getLeftRight(left,right,leftDir,rightDir,navMesh,from, i);
		if (leftDir.crossProduct(rightDir) < 0)
		{
			std::swap(left, right);
			std::swap(leftDir, rightDir);
		}
		i++;
		for (;i != _connections.end(); i++)
		{
			Vector2 left1, right1, leftDir1, rightDir1;
			getLeftRight(left1,right1,leftDir1,rightDir1,navMesh,from, i);
			if (leftDir1.crossProduct(leftDir) > 0)
			{
				left = left1;
				leftDir = leftDir1;
			}

			if (rightDir1.crossProduct(rightDir) < 0)
			{
				right = right1;
				rightDir = rightDir1;
			}
		}

		float l = left.distance(from) + left.distance(dest);
		float r = right.distance(from) + right.distance(dest);

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
			result = new TrianglePath(triangles);
			_map[pair] = result;
		}
		return result;
	}
};