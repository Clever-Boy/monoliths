#pragma once

#include "stdafx.h"

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

	PhysicsManager();
	~PhysicsManager();
};