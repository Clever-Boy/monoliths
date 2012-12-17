#pragma once

#include "stdafx.h"
#include "World.h"
#include "GameController.h"

using namespace Ogre;
using namespace OIS;


class Game : public FrameListener
{
private:
	Ogre::Root* _root;
	RenderWindow* _window;
	SceneManager* _sceneManager;
	InputManager* _inputManager;
	Keyboard* _keyboard;
	Mouse* _mouse;

	Viewport* _viewport;
	Camera* _freeCamera;
	OgreBites::SdkCameraMan* _cameraMan;

	float _totalTime;

	Light* _dirLight0;

	CompositorInstance* _compie;
	
	World* _world;
	std::vector<GameController*> _controllers;
	GameController* _activeController;

	void setupRenderSystem();
	void setupWorld();
	void setupInputSystem();

	void setActiveController(int controllerIdx)
	{
		_activeController = _controllers[controllerIdx];
		_keyboard->setEventCallback(_activeController);
		_mouse->setEventCallback(_activeController);
		_viewport->setCamera(_activeController->getCamera());
	}

public:
	Game()
		: _totalTime(0),
		  _activeController(NULL)
	{
	}

	~Game()
	{
		//delete _root;
	}

	World* getWorld()
	{
		return _world;
	}

	String getNextId()
	{
		return _world->getNextId();
	}

	float getTotalTime()
	{
		return _totalTime;
	}

	OIS::Keyboard* getKeyboard()
	{
		return _keyboard;
	}

	OIS::Mouse* getMouse()
	{
		return _mouse;
	}

	void addController(GameController* controller)
	{
		_controllers.push_back(controller);
	}
	
	void start()
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

		

		setupRenderSystem();
		setupWorld();
		setupInputSystem();
		
		_root->addFrameListener(this);
		doRenderLoop();
	}

	void doRenderLoop()
	{
		while(true)
		{
			if(_window->isClosed())
			{
				_root->shutdown();
				break;
			}
			WindowEventUtilities::messagePump();
		
			if(!_root->renderOneFrame())
			{
				_root->shutdown();
				break;
			}
		}
	}

	virtual bool frameStarted(const FrameEvent& evt) { return true; }

	bool doUpdate(const FrameEvent& evt);

	virtual bool frameRenderingQueued(const FrameEvent& evt)
	{ 
		return doUpdate(evt);
	}
};