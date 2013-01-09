#pragma once
#include "stdafx.h"
#include "common.h"
#include "GameObject.h"
#include "NavMesh.h"
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
	std::vector<NavMeshTriangle*> _path;

public:

	PathDebugObject(const std::vector<NavMeshTriangle*> path)
		: _path(path)
	{
	}

	virtual void initImpl(World* world)
	{
		begin("NavMeshPathDbg", "NavMeshPathDbg", RenderOperation::OT_LINE_STRIP);
		N(Ogre::Vector3::UNIT_Y);

		for (auto i = _path.begin(); i != _path.end(); i++)
		{
			Ogre::Vector2 p = (*i)->center;
			V(p.x, 0, p.y);
		}

		Ogre::MeshPtr mesh = end(world->getNextId("mesh"));
		Entity* entity = world->getSceneManager()->createEntity(world->getNextId("pathDbgObject"), mesh);
		addElement(entity, NULL, ET_PATH_DBG);
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