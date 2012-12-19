#include "stdafx.h"
#include "Game.h"
#include "FreeCameraController.h"

using namespace Ogre;

const float Game::STEP_TIME = 0.01;

void Game::start()
{
		String pluginsCfg =
#ifdef _DEBUG
			"plugins_d.cfg";
#else
			"plugins.cfg";
#endif
		_root = new Ogre::Root(pluginsCfg);
		
		RenderSystemList list = _root->getAvailableRenderers(); 
		_root->setRenderSystem(list.at(0));
		_root->initialise(false);

		NameValuePairList misc;
		misc["FSAA"] = "8";
		_window = _root->createRenderWindow("|| MONOLITHS ||", 1024, 768, false);
		_sceneManager = _root->createSceneManager(0, "Default");
		ResourceGroupManager::getSingleton().addResourceLocation("media","FileSystem","General", false);
		ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

		_physicsManager = new PhysicsManager();

		setupRenderSystem();
		setupWorld();
		setupInputSystem();
		_physicsManager->setupPVD();

		_root->addFrameListener(this);
		doRenderLoop();
	}


void Game::setupRenderSystem()
{
	_freeCamera =  _sceneManager->createCamera("FreeCam");
	_freeCamera->setPosition(Ogre::Vector3(0,150, -200 ));
	//_camera->lookAt(Ogre::Vector3(0,0,0));
	_freeCamera->setDirection(Ogre::Vector3::UNIT_Z); 
	_freeCamera->setNearClipDistance(5);
	_freeCamera->setFarClipDistance(40000);
	_viewport = _window->addViewport(_freeCamera);
	_window->setActive(true);
	_compie = CompositorManager::getSingleton().addCompositor(_window->getViewport(0), "Global");	
	_compie->setEnabled(true);
}

void Game::setupWorld()
{
	_viewport->setBackgroundColour(ColourValue(0.8f,0.8f,0.8f));
	_world = new World(_sceneManager, _physicsManager, 40000);
	_world->init();
}


void Game::setupInputSystem()
{
	OIS::ParamList pl;
	size_t windowHnd = 0;
	_window->getCustomAttribute("WINDOW", &windowHnd);
	std::ostringstream windowHndStr;
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	_inputManager = InputManager::createInputSystem(pl);

	_keyboard = static_cast<OIS::Keyboard*>(_inputManager->createInputObject( OIS::OISKeyboard, true ));
	_mouse = static_cast<OIS::Mouse*>(_inputManager->createInputObject( OIS::OISMouse, true ));

	FreeCameraController* freeController = new FreeCameraController(_freeCamera);
	addController(freeController);
	setActiveController(0);
	//_keyboard->setEventCallback(this);
	//_mouse->setEventCallback(this);
}

bool Game::doUpdate(const FrameEvent& evt)
{ 
	_mouse->capture();
	_keyboard->capture();
	
	_totalTime += evt.timeSinceLastEvent;

	if (_keyboard->isKeyDown(KC_ESCAPE))
	{
		return false;
	}

	for (int i = 0; i < _controllers.size(); i++)
	{
		if (_keyboard->isKeyDown((OIS::KeyCode)(KC_F1+i)))
		{
			setActiveController(i);
			break;
		}
	}

	_activeController->onUpdating(evt, this);

	
	_simulationAccumulator += evt.timeSinceLastFrame;
	
	if (_simulationAccumulator >= STEP_TIME)
	{
		_simulationAccumulator -= STEP_TIME;
		_world->act(_totalTime, STEP_TIME);
		_physicsManager->simulate(STEP_TIME);
		_world->update(_totalTime, STEP_TIME);
	}

	_world->update(_totalTime, evt.timeSinceLastEvent);
	_activeController->onUpdated(evt, this);

	return true; 
}

Game::~Game()
{
	delete _physicsManager;
}