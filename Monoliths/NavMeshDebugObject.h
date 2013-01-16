#pragma once
#include "stdafx.h"
#include "common.h"
#include "GameObject.h"
#include "NavMesh.h"
#include "PathFinder.h"
#include "MeshGenerator.h"

template <ElementType ELEMENT_TYPE = ET_MONOLITH_DBG>
class DebugObject : public GameObject, protected MeshGenerator
{
protected:
	void endMesh(World* world, const String& name)
	{
		Ogre::MeshPtr mesh = MeshGenerator::end(world->getNextId("mesh"));
		Entity* entity = world->getSceneManager()->createEntity(world->getNextId(name), mesh);
		addElement(entity, NULL, ELEMENT_TYPE);
	}

	void putBox(float x, float y, float z, const String& materialName, World* world, float scale = 10)
	{
		Entity* entity = world->getSceneManager()->createEntity(world->getNextId("doboz_"),"doboz.mesh");
		entity->setMaterialName(materialName);
		addElement(Ogre::Vector3(x,y,z), Ogre::Quaternion::IDENTITY, entity, NULL, ELEMENT_TYPE, scale);
	}
};

class NavMeshDebugObject : public DebugObject<>
{
	NavMesh* _navMesh;

public:
	NavMeshDebugObject(NavMesh* navMesh)
	{
		_navMesh = navMesh;
	}

	virtual void initImpl(World* world)
	{
		const std::vector<Ogre::Vector2>& vertices = _navMesh->getVertices();
		const std::vector<NavMeshTriangle>& triangles = _navMesh->getTriangles();
		const std::vector<TriangleConnection*>& connections = _navMesh->getTriangleConnections();

		begin("NavMeshDbg", "RedWf");
		N(Ogre::Vector3::UNIT_Y);
		for (auto i = vertices.begin(); i != vertices.end(); i++)
		{
			V(i->x, 0, i->y);
		}
		for (auto i = triangles.begin(); i != triangles.end(); i++)
		{
			TRI(i->idx0, i->idx1, i->idx2);
		}
		endMesh(world, "NavMeshDbgEntity");

		
		begin("NavMeshConnectionDbg", "Blue", RenderOperation::OT_LINE_LIST);
		for (auto i = connections.begin(); i != connections.end(); i++)
		{
			Ogre::Vector2 p0 = (*i)->tri0->center;
			Ogre::Vector2 p1 = (*i)->tri1->center;
			V(p0.x, 0, p0.y);
			V(p1.x, 0, p1.y);
		}
		endMesh(world, "NavMeshConnDbgEntity");
	}

};


class PathDebugObject :  public DebugObject<ET_PATH_DBG>
{
	TrianglePath* _path;
	NavMesh* _navMesh;
	Ogre::Vector2 _from;
	Ogre::Vector2 _to;
	Ogre::Vector2 _nextCorner;

public:

	PathDebugObject(TrianglePath* path, const Ogre::Vector2& from, const Ogre::Vector2& to, const Ogre::Vector2& nextCorner)
		: _path(path), _navMesh(path->getNavMesh()),
		_from(from),
		_to(to),
		_nextCorner(nextCorner)
	{
	}


	virtual void initImpl(World* world)
	{
		begin("NavMeshPathDbg", "Green", RenderOperation::OT_LINE_STRIP);
		N(Ogre::Vector3::UNIT_Y);
		for (auto i = _path->getTriangles().begin(); i != _path->getTriangles().end(); i++)
		{
			V((*i)->center);
		}
		endMesh(world, "pathDbgObj");

		if (_path->getEdges().size() > 0)
		{
			begin("NavMeshPathEdgesDbg", "Cyan", RenderOperation::OT_LINE_LIST);
			for (auto i = _path->getEdges().begin(); i != _path->getEdges().end(); i++)
			{
				const Ogre::Vector2& l = i->getLeftPoint(_navMesh);
				const Ogre::Vector2& r = i->getRightPoint(_navMesh);
				V(l); V(r);
				putBox(l.x, 0, l.y, "Red", world);
				putBox(r.x, 0, r.y, "Green", world);
			
				Vector2 dir, pos;
				if (i->mutualPoint == PathEdge::MP_LEFT)
				{
					dir = r-l;
					pos = l;
				}
				else if (i->mutualPoint == PathEdge::MP_RIGHT)
				{
					dir = l-r;
					pos = r;
				}

				if (i != _path->getEdges().begin())
				{
					dir.normalise();
					pos+= dir*20;
					putBox(pos.x, 0, pos.y, "Black", world);
				}
			}
			endMesh(world, "gnegklegvklne");
		}

		begin("NavMeshStartDbg","Purple");
		NavMeshTriangle* tri = _path->firstTriangle();
		V(tri->getPosition(0, *_navMesh));
		V(tri->getPosition(1, *_navMesh));
		V(tri->getPosition(2, *_navMesh));
		TRI(0,1,2);
		endMesh(world, "asdasdsa");

		begin("NavMeshEndDbg","Yellow");
		tri = _path->lastTriangle();
		V(tri->getPosition(0, *_navMesh));
		V(tri->getPosition(1, *_navMesh));
		V(tri->getPosition(2, *_navMesh));
		TRI(0,1,2);
		endMesh(world,"fefwefewf");

		begin("navmeshNextCornerEdge", "Black", RenderOperation::OT_LINE_LIST);
		//Ogre::Vector2 corn = _path->nextCorner(_from, _to);
		V(_from);
		V(_nextCorner);
		endMesh(world,"meghalsz");

		//begin("NavMeshCornDbg", "NavMeshCornDbg", RenderOperation::OT_LINE_STRIP);
		//end(world,"cornDbgObj");
	}
};

class TriangleDebugObject : public DebugObject<ET_STANDARD>
{
	NavMeshTriangle* _triangle;
	NavMesh* _navMesh;
public:

	TriangleDebugObject(NavMeshTriangle* triangle, NavMesh* navMesh) 
		: _triangle(triangle), 
		  _navMesh(navMesh)
	{
	}

	virtual void initImpl(World* world)
	{
		begin("Tri", "Red");
		N(Ogre::Vector3::UNIT_Y);
		const Ogre::Vector2& v0 = _triangle->getPosition(0, *_navMesh);
		const Ogre::Vector2& v1 = _triangle->getPosition(1, *_navMesh);
		const Ogre::Vector2& v2 = _triangle->getPosition(2, *_navMesh);

		V(v0.x, 0, v0.y);
		V(v1.x, 0, v1.y);
		V(v2.x, 0, v2.y);

		TRI(0,1,2);

		endMesh(world, "dbgTri");
	}
};