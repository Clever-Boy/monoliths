#include "stdafx.h"
#include "Game.h"
#include "Character.h"
#include "Action.h"
#include "FreeCameraController.h"
#include "TPSCharacterController.h"

//#include <OgreTextAreaOverlayElement.h>

using namespace Ogre;

const float Game::STEP_TIME = 0.001;

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
	//_sceneManager = _root->createSceneManager(0, "Default");
	_sceneManager = _root->createSceneManager("OctreeSceneManager");
	ResourceGroupManager::getSingleton().addResourceLocation("media","FileSystem","General", false);
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	
	_physicsManager = new PhysicsManager();

	setupRenderSystem();
	setupWorld();
	setupOverlays();
	setupInputSystem();
	_physicsManager->setupPVD();

	_root->addFrameListener(this);
	doRenderLoop();
}

void Game::setupOverlays()
{
	Ogre::FontManager::getSingleton().getByName("TrebuchetMSBold")->load();

	Ogre::OverlayManager& mgr = Ogre::OverlayManager::getSingleton();
	Ogre::Overlay* overlay = mgr.create("kaksi");
	_guiPanel = (OverlayContainer*)mgr.createOverlayElement("Panel", "pocs");
	_guiPanel->setMetricsMode(GMM_PIXELS);
	_guiPanel->setPosition(10,10);
	_guiPanel->setDimensions(50,30);
	//panel->setMaterialName("Panel");
	//panel->setColour(ColourValue(0,0,0,0));
	overlay->add2D(_guiPanel);

	_fpsElement = (TextAreaOverlayElement*)mgr.createOverlayElement("TextArea", "fos");
	_fpsElement->setMetricsMode(GMM_PIXELS);
	_fpsElement->setPosition(0,0);
	_fpsElement->setDimensions(50,30);
	//_fpsElement->setCaption("CSICSKA!");
	_fpsElement->setCharHeight(16);
	_fpsElement->setFontName("TrebuchetMSBold");
	//textArea->setColour(ColourValue(1,0,0,1));
	_guiPanel->addChild(_fpsElement);
	overlay->show();

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
#ifndef BASIC_GRAPHICS
	_compie->setEnabled(true);
#endif
}

void Game::setupWorld()
{
	_viewport->setBackgroundColour(ColourValue(0.8f,0.8f,0.8f));
	_world = new World(_sceneManager, _physicsManager, 5000); 
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

	_keyboard =(OIS::Keyboard*)_inputManager->createInputObject(OIS::OISKeyboard, true);
	_mouse = (OIS::Mouse*)_inputManager->createInputObject(OIS::OISMouse, true);

	FreeCameraController* freeController = new FreeCameraController(_freeCamera);
	addController(freeController);
	
	TPSCharacterController* puppieController = new TPSCharacterController(_world, _world->getPuppie(), 
		Action::ROBOT_IDLE, Action::ROBOT_WALK);
	addController(puppieController);

	setActiveController(0);
	
}

void Game::doRenderLoop()
{
	//_root->startRendering();


	while(true)
	{
		if(_window->isClosed())
		{
			_root->shutdown();
			break;
		}
		WindowEventUtilities::messagePump();
		_mouse->capture();
		_keyboard->capture();
		

		if(!_root->renderOneFrame())
		{
			_root->shutdown();
			break;
		}
	}

	/*while(true)
	{
		if(_window->isClosed())
		{
			_root->shutdown();
			break;
		}
		WindowEventUtilities::messagePump();
		_mouse->capture();
		_keyboard->capture();
		

		if(!_root->renderOneFrame())
		{
			_root->shutdown();
			break;
		}
	}*/
}


bool Game::doUpdate(const FrameEvent& evt)
{ 
	
	//_mouse->capture();
	//_keyboard->capture();
	
	//_window->getLastFPS();

	String fpsStr = StringConverter::toString(_window->getLastFPS(), 4);
	_fpsElement->setCaption("FPS: "+fpsStr);

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

	
#if 1
	_simulationAccumulator += evt.timeSinceLastFrame;
	
	if (_simulationAccumulator >= STEP_TIME)
	{
		float elap = Math::Floor(_simulationAccumulator/STEP_TIME);
		elap*=STEP_TIME;

		_simulationAccumulator -= elap;
		_world->act(_totalTime, elap);
		_physicsManager->simulate(elap);
		_world->update(_totalTime, elap);
	}
#else
	
	_world->act(_totalTime, evt.timeSinceLastFrame);
	_physicsManager->simulate(evt.timeSinceLastFrame);
	_world->update(_totalTime, evt.timeSinceLastFrame);
#endif

	
	_activeController->onUpdated(evt, this);


	return true; 
}

Game::~Game()
{
	delete _physicsManager;
}