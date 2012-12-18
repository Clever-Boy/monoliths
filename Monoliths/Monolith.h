#pragma once
#include "stdafx.h"
#include "GameObject.h"

class Monolith : public GameObject
{
private:
	float _a, _b, _c;
	Ogre::Vector3 _position;
	Ogre::Quaternion _orientation;
public:
	Monolith(float a, float b, float c, float x, float y, float z, float rotAngle, Ogre::Vector3 rotAxis = Ogre::Vector3::UNIT_Y)
		 : _position(x,y,z),
		 _orientation(Radian(rotAngle), rotAxis)
	{
		_a = a; _b = b; _c = c;
	}
	
protected:

	virtual void initImpl(World* world)
	{
		ManualObject* cube = createCubeMesh("kaka", "Monolith", _a, _b, _c);
		MeshPtr mesh = cube->convertToMesh(world->getNextId("mesh"));
		Entity* entity = world->getSceneManager()->createEntity(world->getNextId("monolith"), mesh);
		
		PhysicsManager* pxMgr = world->getPhysicsManager();
		PxVec3 halfExt = PxVec3(_a, _b, _c)*0.5f;
		PxRigidStatic* actor = PxCreateStatic(*pxMgr->getPhysics(), PxTransform::createIdentity(), PxBoxGeometry(halfExt),
			*pxMgr->getDefaultMaterial(), PxTransform(-halfExt));
		
		//PxCreateDynamic(*_physics, PxTransform(PxVec3(x,y,z)), PxBoxGeometry(50,50,50), *matcsi, 10 /*,PxTransform(PxVec3(50,50,50))*/);
		
		addElement(_position, _orientation, entity, actor);


		//entities.push_back(entity);
	}
	
	void V(ManualObject* q, float x, float y, float z, Ogre::Vector3 n, float u, float v)
	{
		q->position(x,y,z); q->normal(n); q->textureCoord(u,v);
	}

	ManualObject* createCubeMesh(Ogre::String name, Ogre::String matName, float a=1, float b=1, float c=1) 
	{
		ManualObject* q = new ManualObject(name);
		
		q->begin(matName);
		

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

		V(q,0,0,0,frontNormal,0,v1); V(q,a,0,0,frontNormal,u1,v1); V(q,a,0,c,frontNormal,u1,0);V(q,0,0,c,frontNormal,0,0); // front
		V(q,a,0,0,rightNormal,u1,v1); V(q,a,b,0,rightNormal,u2,v1); V(q,a,b,c,rightNormal,u2,0); V(q,a,0,c,rightNormal,u1,0); // right
        V(q,a,b,0,backNormal,u2,v1); V(q,0,b,0,backNormal,u3,v1); V(q,0,b,c,backNormal,u3,0); V(q,a,b,c,backNormal,u2,0); // back
        V(q,0,b,0,leftNormal, u3,v1); V(q,0,0,0,leftNormal,u4,v1); V(q,0,0,c,leftNormal,u4,0); V(q,0,b,c,leftNormal,u3,0); // left
        V(q,0,0,c,topNormal,0,v2); V(q,a,0,c,topNormal,u1,v2); V(q,a,b,c,topNormal,u1,0); V(q,0,b,c,topNormal,0,0); // top
        V(q,0,b,0,bottomNormal,0,0); V(q,a,b,0,bottomNormal,u1,0); V(q,a,0,0,bottomNormal,u1,v2); V(q,0,0,0,bottomNormal,0,v2); // bottom

		q->triangle(0,1,2);      q->triangle(2,3,0);
		q->triangle(4,5,6);      q->triangle(6,7,4);
		q->triangle(8,9,10);      q->triangle(10,11,8);
		q->triangle(12,13,14);   q->triangle(14,15,12);
		q->triangle(16,17,18);   q->triangle(18,19,16);
		q->triangle(20,21,22);   q->triangle(22,23,20);
		
		q->end(); 

		return q;

	}

	virtual float getCtrDetectionExponent()
	{
		return 2.2;
	}
};
