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