#include "stdafx.h"
#include "PhysicsManager.h"

using namespace physx;

PhysicsManager::PhysicsManager()
{
	_foundation = PxCreateFoundation(PX_PHYSICS_VERSION,
		_DefaultAllocatorCallback, _DefaultErrorCallback);
	_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *_foundation, PxTolerancesScale());
	_cooking = PxCreateCooking(PX_PHYSICS_VERSION, *_foundation, PxCookingParams());

	PxSceneDesc sceneDesc(_physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0,-1000,0);
	if (!sceneDesc.cpuDispatcher)
	{
		_cpuDispatcher = PxDefaultCpuDispatcherCreate(4);
		sceneDesc.cpuDispatcher = _cpuDispatcher;
	}

	if (!sceneDesc.filterShader)
	{
		sceneDesc.filterShader = &PxDefaultSimulationFilterShader;
	}
	
	_scene = _physics->createScene(sceneDesc);

	_controllerManager = PxCreateControllerManager(*_foundation);

	_defaultMaterial = _physics->createMaterial(0.5, 0.5, 0.5);
}

void PhysicsManager::setupPVD()
{
	if (_physics->getPvdConnectionManager() == NULL)
	{
		throw Ogre::Exception(42, Ogre::String("FOOOOS"), Ogre::String("KAKAAA"));
	}

	const char*     pvd_host_ip = "127.0.0.1";  // IP of the PC which is running PVD
	int             port        = 5425;         // TCP port to connect to, where PVD is listening
	unsigned int    timeout     = 100;          // timeout in milliseconds to wait for PVD to respond,
												// consoles and remote PCs need a higher timeout.
	PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerExt::getAllConnectionFlags();
	_pvdConnection = PxVisualDebuggerExt::createConnection(_physics->getPvdConnectionManager(),
		pvd_host_ip, port, timeout, connectionFlags);
}

PhysicsManager::~PhysicsManager()
{
	_defaultMaterial->release();
	_controllerManager->release();
	_scene->release();
	_cooking->release();
	_physics->release();
	_foundation->release();
}
