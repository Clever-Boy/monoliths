#pragma once
#include "stdafx.h"
#include "World.h"
#include "GameObject.h"

using namespace Ogre;
using namespace physx;

class BoxObject : public GameObject
{
private:
	Ogre::Vector3 _position;
	PxVec3 _force;

public:
	BoxObject(const Ogre::Vector3& position, const Ogre::Vector3 force = Ogre::Vector3::ZERO)
		: _position(position), _force(force.x, force.y, force.z)
	{
	}
	

protected:

	virtual float getScale() const 
	{ 
		return 100;
	}

	virtual void initImpl(World* world)
	{
		Entity* entity = world->getSceneManager()->createEntity(world->getNextId("doboz_"),"doboz.mesh");
		entity->setMaterialName("Doboz");
		
		PhysicsManager* physMgr = world->getPhysicsManager();

		PxMaterial* matcsi = physMgr->getDefaultMaterial();

		PxVec3 ext = PxVec3(50,50,50)/PHYSICS2WORLD_SCALE;
		PxRigidDynamic* actor =  PxCreateDynamic(*physMgr->getPhysics(), 
			PxTransform::createIdentity(), PxBoxGeometry(ext), *matcsi, 0.10);

		addElement(_position, Ogre::Quaternion::IDENTITY, entity, actor);
	}

	virtual void postInit()
	{
		PxRigidDynamic* actor = (PxRigidDynamic*) _elements[0].actor;
		actor->addForce(_force, PxForceMode::eVELOCITY_CHANGE);
	}
};