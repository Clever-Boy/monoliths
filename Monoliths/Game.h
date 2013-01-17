#pragma once
#include "stdafx.h"

#include "common.h"
#include "World.h"
#include "GameController.h"
#include "PhysicsManager.h"
#include "EnemyGenerator.h"

using namespace Ogre;
using namespace OIS;

class Game : public FrameListener, public GameStateListener
{
private:
	//SceneManager* _sceneManager;
	EnemyGenerator* _enemyGenerator;
	Ogre::Root* _root;
	RenderWindow* _window;
	SceneManager* _sceneManager;
	InputManager* _inputManager;
	Keyboard* _keyboard;
	Mouse* _mouse;

	Viewport* _viewport;
	Camera* _freeCamera;
	OgreBites::SdkCameraMan* _cameraMan;

	static const float STEP_TIME;
	float _simulationAccumulator;
	float _totalTime;
	float _playerShotTimer;

	Light* _dirLight0;

	CompositorInstance* _compie;
	
	PhysicsManager* _physicsManager;
	World* _world;
	std::vector<GameController*> _controllers;
	GameController* _activeController;
	
	OverlayContainer* _guiPanel;
	TextAreaOverlayElement* _fpsElement;
	TextAreaOverlayElement* _hpElement;
	TextAreaOverlayElement* _gameOverElement;

	void setupRenderSystem();
	void setupWorld();
	void setupInputSystem();
	void setupOverlays();

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
		  _simulationAccumulator(0),
		  _activeController(NULL),
		  _playerShotTimer(0)
	{
	}

	~Game();

	Viewport* getViewport() const
	{
		return _viewport;
	}

	PhysicsManager* getPhysicsManager() const
	{
		return _physicsManager;
	}

	World* getWorld() const
	{
		return _world;
	}

	String getNextId()
	{
		return _world->getNextId();
	}

	float getTotalTime() const
	{
		return _totalTime;
	}

	OIS::Keyboard* getKeyboard() const
	{
		return _keyboard;
	}

	OIS::Mouse* getMouse() const
	{
		return _mouse;
	}

	void addController(GameController* controller)
	{
		_controllers.push_back(controller);
	}
	
	void start();
	void doRenderLoop();

	virtual bool frameStarted(const FrameEvent& evt) 
	{ 
		return true; 
	}

	bool doUpdate(const FrameEvent& evt);

	virtual bool frameRenderingQueued(const FrameEvent& evt)
	{ 
		return doUpdate(evt);
		//return true;
	}

	virtual void notifyGameOver();

	virtual void notifyShot()
	{
		_playerShotTimer = 0;
	}
};
