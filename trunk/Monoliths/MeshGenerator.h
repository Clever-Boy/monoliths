#pragma once
#include "stdafx.h"

using namespace Ogre;

class MeshGenerator
{
	ManualObject* q;
	Ogre::Vector3 currentNormal;
	Ogre::Vector3 offset;
public:

	MeshGenerator() : q(NULL), currentNormal(Ogre::Vector3::UNIT_Y) { }

	void begin(String name, String materialName, RenderOperation::OperationType operationType = RenderOperation::OT_TRIANGLE_LIST)
	{
		q = new ManualObject(name);
		q->begin(materialName, operationType);
		OFFSET(0,0,0);
	}

	void OFFSET(float x, float y, float z)
	{
		this->offset = Ogre::Vector3(x,y,z);
	}

	void N(const Ogre::Vector3& n)
	{
		currentNormal = n;
	}

	void V(float x, float y, float z)
	{
		V(x,y,z,0,0);
	}

	void V(float x, float y, float z, float u, float v)
	{
		V(x, y, z, currentNormal, u, v);
	}

	void V(float x, float y, float z, const Ogre::Vector3& n, float u, float v)
	{
		q->position(x+offset.x,y+offset.y,z+offset.z); q->normal(n); q->textureCoord(u,v);
	}

	void TRI(int i0, int i1, int i2)
	{
		q->triangle(i0, i1, i2);
	}

	MeshPtr end(String id)
	{
		q->end();
		return q->convertToMesh(id);
	}

	~MeshGenerator()
	{
		//if (q != NULL) delete q;
	}
};