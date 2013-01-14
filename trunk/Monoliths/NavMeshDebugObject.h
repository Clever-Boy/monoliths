#pragma once
#include "stdafx.h"
#include "common.h"
#include "GameObject.h"
#include "NavMesh.h"
#include "PathFinder.h"
#include "MeshGenerator.h"


class NavMeshDebugObject : public GameObject, private MeshGenerator
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

		begin("NavMeshDbg", "NavMeshDbg");
		N(Ogre::Vector3::UNIT_Y);
		for (auto i = vertices.begin(); i != vertices.end(); i++)
		{
			V(i->x, 0, i->y);
		}

		for (auto i = triangles.begin(); i != triangles.end(); i++)
		{
			TRI(i->idx0, i->idx1, i->idx2);
		}

		Ogre::MeshPtr mesh = end(world->getNextId("mesh"));
		Entity* entity = world->getSceneManager()->createEntity("NavMeshDbgEntity", mesh);
		addElement(Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY, entity, NULL, ET_MONOLITH_DBG);

		const std::vector<TriangleConnection*>& connections = _navMesh->getTriangleConnections();

		
		begin("NavMeshConnectionDbg", "NavMeshConnectionDbg", RenderOperation::OT_LINE_LIST);

		for (auto i = connections.begin(); i != connections.end(); i++)
		{
			
			Ogre::Vector2 p0 = (*i)->tri0->center;
			Ogre::Vector2 p1 = (*i)->tri1->center;
			V(p0.x, 0, p0.y);
			V(p1.x, 0, p1.y);
		}

		Ogre::MeshPtr connMesh = end(world->getNextId("mesh"));
		Entity* connEntity = world->getSceneManager()->createEntity("NavMeshConnDbgEntity", connMesh);
		addElement(connEntity, NULL, ET_MONOLITH_DBG);
		//
		
		//end(world->ge
	}

};


class PathDebugObject :  public GameObject, private MeshGenerator
{
	TrianglePath* _path;
	NavMesh* _navMesh;

public:

	PathDebugObject(TrianglePath* path, NavMesh* navMesh)
		: _path(path), _navMesh(navMesh)
	{
	}

	void end(World* world, const String& name)
	{
		Ogre::MeshPtr mesh = MeshGenerator::end(world->getNextId("mesh"));
		Entity* entity = world->getSceneManager()->createEntity(world->getNextId(name), mesh);
		addElement(entity, NULL, ET_PATH_DBG);
	}

	virtual void initImpl(World* world)
	{
		begin("NavMeshPathDbg", "NavMeshPathDbg", RenderOperation::OT_LINE_STRIP);
		N(Ogre::Vector3::UNIT_Y);
		const std::vector<TriangleConnection*> connections = _path->getConnections();
		for (auto i = _path->getTriangles().begin(); i != _path->getTriangles().end(); i++)
		{
			V((*i)->center);
		}
		end(world, "pathDbgObj");

		begin("NavMeshEdgeDbg","NavMeshEdgeDbg", RenderOperation::OT_LINE_STRIP);
		for (auto i = connections.begin(); i != connections.end(); i++)
		{
			V((*i)->getPointA(_navMesh));
			V((*i)->getPointB(_navMesh));
		}
		end(world, "navmeshEdge");
		

		begin("NavMeshStartDbg","NavMeshStartDbg");
		NavMeshTriangle* tri = _path->firstTriangle();
		V(tri->getPosition(0, *_navMesh));
		V(tri->getPosition(1, *_navMesh));
		V(tri->getPosition(2, *_navMesh));
		TRI(0,1,2);
		end(world, "startDbgObject");

		begin("NavMeshEndDbg","NavMeshEndDbg");
		tri = _path->lastTriangle();
		V(tri->getPosition(0, *_navMesh));
		V(tri->getPosition(1, *_navMesh));
		V(tri->getPosition(2, *_navMesh));
		TRI(0,1,2);
		end(world,"endDbgObj");
		
		//return;
		begin("NavMeshCornDbg", "NavMeshCornDbg", RenderOperation::OT_LINE_STRIP);
		Vector2 s = _path->firstTriangle()->center;
		Vector2 c = _path->nextCorner(s, _path->lastTriangle()->center, _navMesh);
		V(s.x, 0, s.y);
		V(c.x, 0, c.y);
		end(world,"cornDbgObj");
	}
};

class TriangleDebugObject : public GameObject, private MeshGenerator
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
		begin("Tri", "Marked");
		N(Ogre::Vector3::UNIT_Y);
		const Ogre::Vector2& v0 = _triangle->getPosition(0, *_navMesh);
		const Ogre::Vector2& v1 = _triangle->getPosition(1, *_navMesh);
		const Ogre::Vector2& v2 = _triangle->getPosition(2, *_navMesh);

		V(v0.x, 0, v0.y);
		V(v1.x, 0, v1.y);
		V(v2.x, 0, v2.y);

		TRI(0,1,2);

		Ogre::MeshPtr mesh = end(world->getNextId("mesh"));
		Entity* entity = world->getSceneManager()->createEntity(world->getNextId("dbgTri"), mesh);
		addElement(entity, NULL, ET_MONOLITH_DBG);
	}
};