#pragma once

#include "stdafx.h"
#include "common.h"

using namespace physx;

class PhysicsManager
{
private:
	PxPhysics* _physics;
	PxFoundation* _foundation;
	PxDefaultErrorCallback _DefaultErrorCallback;
	PxDefaultAllocator _DefaultAllocatorCallback;
	PxCooking* _cooking;
	PxScene* _scene;
	PxDefaultCpuDispatcher* _cpuDispatcher;
	PxControllerManager* _controllerManager;
	
	PVD::PvdConnection* _pvdConnection;
	PxMaterial* _defaultMaterial;

public:

	PxPhysics* getPhysics()
	{
		return _physics;
	}

	PxMaterial* getDefaultMaterial()
	{
		return _defaultMaterial;
	}

	PxControllerManager* getControllerManager()
	{
		return _controllerManager;
	}

	PxCapsuleController* createCapsuleController(const PxCapsuleControllerDesc& desc)
	{
		return (PxCapsuleController*)_controllerManager->createController(*_physics, _scene, desc);
	}

	void simulate(float dt)
	{
		_scene->simulate(dt);
		_scene->fetchResults(true);
	}

	void addActor(PxActor* actor)
	{
		_scene->addActor(*actor);
	}

	void setupPVD();

	PxRigidActor* pickNearestActor(const Ogre::Ray& ray, Ogre::Vector3& impact)
	{
		PxVec3 origin = PxVec3(ray.getOrigin().x, ray.getOrigin().y, ray.getOrigin().z);
		origin /= PHYSICS2WORLD_SCALE;
		PxVec3 dir = PxVec3(ray.getDirection().x, ray.getDirection().y, ray.getDirection().z);
		dir.normalizeFast();
		const PxSceneQueryFlags outputFlags = PxSceneQueryFlag::eDISTANCE | PxSceneQueryFlag::eIMPACT;

		PxRaycastHit hit;

		if (_scene->raycastSingle(origin, dir, 100000, outputFlags, hit))
		{
			impact = Ogre::Vector3(hit.impact.x, hit.impact.y, hit.impact.z)*PHYSICS2WORLD_SCALE;
			PxRigidActor& result = hit.shape->getActor();
			return &result;
		}
		else
		{
			return NULL;
		}
	}

	PhysicsManager();
	~PhysicsManager();
};