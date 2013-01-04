#pragma once
#include "stdafx.h"
#include "GameObject.h"
#include "MeshGenerator.h"
#include <cmath>

class Monolith : public GameObject, private MeshGenerator
{
private:
	float _a, _b, _c;
	float _rotAngle;
	float _slant;

	Ogre::Vector3 _position;
	Ogre::Quaternion _orientation;

	Ogre::Vector2 _planarPoints[4];
	
public:
	Monolith(float a, float b, float c, float x, float y, float z, float rotAngle, float slant, float characterRadius, float characterHeight)
		 : _position(x,y,z)
	{
		_a = a; _b = b; _c = c;
		float L = std::sqrt(1+slant*slant);
		Ogre::Radian slantAng = Math::ASin(slant/L);
		//Ogre::Radian slantAng = Ogre::Radian(0.05);
		_orientation = Ogre::Quaternion(Radian(rotAngle), Ogre::Vector3::UNIT_Y) * Ogre::Quaternion(slantAng, Ogre::Vector3::UNIT_Z);

		float C = std::cos(rotAngle);
		float S = std::sin(rotAngle);
		float W = a*0.5+characterRadius+(characterHeight+std::abs(y))*slant;
		float H = c*0.5+characterRadius;
		Ogre::Vector2 pos2 = Ogre::Vector2(x, z);

		_planarPoints[0] = ROT(W, H, C, S)+pos2;
		_planarPoints[1] = ROT(-W,H, C, S)+pos2;
		_planarPoints[2] = ROT(-W,-H,C, S)+pos2;
		_planarPoints[3] = ROT(W,-H, C, S)+pos2;
	}
	
protected:

	static Ogre::Vector2 ROT(float x, float y, float C, float S)
	{
		//return Ogre::Vector2(C*x-S*y, S*x+C*y);
		return Ogre::Vector2(C*x+S*y, -S*x+C*y);
	}

	virtual void initImpl(World* world)
	{
		String matName =
#ifndef BASIC_GRAPHICS
			"Monolith";
#else
			"Monolith_basic";
#endif

		MeshPtr mesh = createCubeMesh(world->getNextId("mesh"), matName, _a, _b, _c);
		Entity* entity = world->getSceneManager()->createEntity(world->getNextId("monolith"), mesh);
		
		PhysicsManager* pxMgr = world->getPhysicsManager();
		PxVec3 halfExt = PxVec3(_a, _b, _c)*0.5f / PHYSICS2WORLD_SCALE;

		PxRigidStatic* actor = PxCreateStatic(*pxMgr->getPhysics(), PxTransform::createIdentity(), PxBoxGeometry(halfExt),
			*pxMgr->getDefaultMaterial(), PxTransform(halfExt));
		
		addElement(_position, _orientation, entity, actor);

		MeshPtr dbgMesh = createDbgMesh(world->getNextId("dbgMesh"), matName);
		Entity* dbgEntity = world->getSceneManager()->createEntity(world->getNextId("monolithDbg"), dbgMesh);
		addElement(Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY, dbgEntity, NULL, ET_MONOLITH_DBG);
	}

	inline void PV(int idx)
	{
		V(_planarPoints[idx].x, 0, _planarPoints[idx].y);
	}

	MeshPtr createDbgMesh(Ogre::String meshId, Ogre::String matName)
	{
		begin("monolithDbg", matName);
		
		N(Ogre::Vector3(0,1,0));

		PV(0); PV(1); PV(2); PV(3);

		TRI(2,1,0);
		TRI(0,3,2);

		return end(meshId);
	}

	/*
	void V(ManualObject* q, float x, float y, float z, Ogre::Vector3 n, float u, float v)
	{
		q->position(x,y,z); q->normal(n); q->textureCoord(u,v);
	}
*/
	MeshPtr createCubeMesh(Ogre::String meshId, Ogre::String matName, float a=1, float b=1, float c=1) 
	{
		//ManualObject* q = new ManualObject(name);
		
		//q->begin(matName);
		
		begin("monolithCube", matName);
		OFFSET(-a*0.5, 0, -c*0.5);

		Ogre::Vector3 frontNormal = -Ogre::Vector3::UNIT_Y; 
		Ogre::Vector3 backNormal = Ogre::Vector3::UNIT_Y;
		Ogre::Vector3 leftNormal = -Ogre::Vector3::UNIT_X;
		Ogre::Vector3 rightNormal = Ogre::Vector3::UNIT_X;
		Ogre::Vector3 topNormal = Ogre::Vector3::UNIT_Z;
		Ogre::Vector3 bottomNormal = -Ogre::Vector3::UNIT_Z;
		
		float u1 = a;
		float u2 = u1 + b;
		float u3 = u2 + u1;
        float u4 = u3 + b;
		float v1 = c;
		float v2 = b;

		V(0,0,0,frontNormal,0,v1); V(a,0,0,frontNormal,u1,v1); V(a,0,c,frontNormal,u1,0);V(0,0,c,frontNormal,0,0); // front
		V(a,0,0,rightNormal,u1,v1); V(a,b,0,rightNormal,u2,v1); V(a,b,c,rightNormal,u2,0); V(a,0,c,rightNormal,u1,0); // right
        V(a,b,0,backNormal,u2,v1); V(0,b,0,backNormal,u3,v1); V(0,b,c,backNormal,u3,0); V(a,b,c,backNormal,u2,0); // back
        V(0,b,0,leftNormal, u3,v1); V(0,0,0,leftNormal,u4,v1); V(0,0,c,leftNormal,u4,0); V(0,b,c,leftNormal,u3,0); // left
        V(0,0,c,topNormal,0,v2); V(a,0,c,topNormal,u1,v2); V(a,b,c,topNormal,u1,0); V(0,b,c,topNormal,0,0); // top
        V(0,b,0,bottomNormal,0,0); V(a,b,0,bottomNormal,u1,0); V(a,0,0,bottomNormal,u1,v2); V(0,0,0,bottomNormal,0,v2); // bottom

		TRI(0,1,2);      TRI(2,3,0);
		TRI(4,5,6);      TRI(6,7,4);
		TRI(8,9,10);     TRI(10,11,8);
		TRI(12,13,14);   TRI(14,15,12);
		TRI(16,17,18);   TRI(18,19,16);
		TRI(20,21,22);   TRI(22,23,20);
		
		return end(meshId);
	}

	virtual float getCtrDetectionExponent()
	{
		return 2.2;
	}
};
