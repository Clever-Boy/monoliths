#include "stdafx.h"
#include "GameController.h"
#include "BoxObject.h"
#include "NinjaObject.h"
#include "FreeCameraController.h"
#include "NinjaController.h"

using namespace Ogre;

void GameController::initPhysX()
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
	
#ifdef PX_WINDOWS
	//Px
#endif

	_scene = _physics->createScene(sceneDesc);

	_controllerManager = PxCreateControllerManager(*_foundation);

}


void GameController::setupRenderTargets()
{
	
}

void GameController::setupViewport()
{
	_freeCam1 = new FreeCameraController(this);
	//_freeCam2 = new FreeCameraController(this);

	_viewport = _window->addViewport(_freeCam1->getCamera());
	_viewport->setBackgroundColour(ColourValue(0.8f,0.8f,0.8f));
	_window->setActive(true);

	setInputListener(_freeCam1);
}

void GameController::setupScene()
{
	
	
	Plane plane(Ogre::Vector3::UNIT_Y, 0);
	MeshPtr mp = MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane, 1000, 1000, 1, 1, true, 1, 10, 10, Ogre::Vector3::UNIT_Z);
	Entity* entGround = _sceneManager->createEntity(mp);
	addEntity(entGround, "Rocky");

	/*Ogre::Entity* entNinja = _sceneManager->createEntity("Ninja", "ninja.mesh");
	_ninjaNode = addEntity(entNinja, "BuziNinnya");*/

	_ninnya = new NinjaObject(0,0,0, this);
	_ninjaController = new NinjaController(_ninnya, this);

	PxMaterial* matcsi = _physics->createMaterial(0.5,0.5,0.5);
	PxRigidStatic* pxPlane = PxCreatePlane(*_physics, PxPlane(PxVec3(0,1,0), 0), *matcsi);
	_scene->addActor(*pxPlane); 

	/*BoxObject* box1 = new BoxObject(150,0,0, this);
	_boxes.push_back(box1);

	BoxObject* box2 = new BoxObject(150,100,0, this);
	_boxes.push_back(box2);

	BoxObject* box3 = new BoxObject(150,200,0, this);
	_boxes.push_back(box2);*/


	BoxObject* box1 = new BoxObject(-200,300,0, this);
	_boxes.push_back(box1);

	BoxObject* box2 = new BoxObject(200,1000,0, this);
	_boxes.push_back(box2);

	BoxObject* box3 = new BoxObject(0,100,500, this);
	_boxes.push_back(box3);	
}


bool GameController::frameStarted(const FrameEvent& evt)
{ 
	
	

	_time += evt.timeSinceLastEvent;

	if (_keyboard->isKeyDown(KC_ESCAPE))
	{
		return false;
	}

	if (_keyboard->isKeyDown(KC_SPACE))
	{
		for (int i = 0; i < _boxes.size(); i++)
		{
			_boxes[i]->reset();
		}
	}

	if (_keyboard->isKeyDown(KC_U))
	{
		PxCapsuleControllerDesc desc;
		desc.setToDefault();
		desc.radius = 30;
		desc.height = 200;
		desc.upDirection = PxVec3(0, 1, 0);

		desc.slopeLimit = 0.707;
		desc.stepOffset = 0.5;
		desc.callback = NULL;
		desc.position.set(0,0,0);
		

		PxControllerManager* cm = PxCreateControllerManager(*_foundation);
		
		PxController* c = cm->createController(*_physics, _scene, desc);
	}


	if (_keyboard->isKeyDown(KC_F1))
	{
		setInputListener(_freeCam1);
	}

	if (_keyboard->isKeyDown(KC_F2))
	{
		setInputListener(_ninjaController);
	}

	const float stepSize = 0.01f;
	_simulationAccumulator+=evt.timeSinceLastFrame;
	
	if (_simulationAccumulator>=stepSize)
	{
		_simulationAccumulator-=stepSize;
		_scene->simulate(stepSize);
		_scene->fetchResults(true);

		for (int i = 0; i < _boxes.size(); i++)
		{
			_boxes[i]->update();
		}

		_ninjaController->update(stepSize);
	}

	

	_mouse->capture();
	_keyboard->capture();

	return true; 
}

void GameController::setupPVD()
{
	if (_physics->getPvdConnectionManager() == NULL)
	{
		throw Ogre::Exception(42, String("FOOOOS"), String("KAKAAA"));
	}

	const char*     pvd_host_ip = "127.0.0.1";  // IP of the PC which is running PVD
	int             port        = 5425;         // TCP port to connect to, where PVD is listening
	unsigned int    timeout     = 100;          // timeout in milliseconds to wait for PVD to respond,
												// consoles and remote PCs need a higher timeout.
	PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerExt::getAllConnectionFlags();
	_pvdConnection = PxVisualDebuggerExt::createConnection(_physics->getPvdConnectionManager(),
		pvd_host_ip, port, timeout, connectionFlags);
}


GameController::~GameController()
{
	if (_pvdConnection)
	{
		_pvdConnection->release();
	}
	_cooking->release();
	_physics->release();
	_foundation->release();
}