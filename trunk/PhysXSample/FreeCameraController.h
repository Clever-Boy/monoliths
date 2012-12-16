#pragma once
#include "stdafx.h"
#include "GameController.h"
#include "InputListener.h"

using namespace Ogre;
using namespace OIS;

class FreeCameraController : public InputListener
{
	OgreBites::SdkCameraMan* _cameraMan;
	Camera* _camera;

public:
	FreeCameraController(GameController* gameController)
	{
		SceneManager* sceneManager = gameController->getSceneManager();
		_camera = sceneManager->createCamera(gameController->generateId());
		_camera->setPosition(Ogre::Vector3(0,200, -1000 ));
		_camera->setDirection(Ogre::Vector3::UNIT_Z); 
		_camera->setNearClipDistance(5);
		_camera->setFarClipDistance(30000);
		_cameraMan = new OgreBites::SdkCameraMan(_camera);
	}

	virtual Camera* getCamera()
	{
		return _camera;
	}

	virtual bool frameRenderingQueued(const FrameEvent& evt)
	{ 
		_cameraMan->frameRenderingQueued(evt);
		return true;
	}

	virtual bool frameStarted(const FrameEvent& evt)
	{
		return true;
	}
	
	virtual bool keyPressed(const KeyEvent &evt)
	{
		_cameraMan->injectKeyDown(evt);		
		return true;
	}

	virtual bool keyReleased(const KeyEvent &evt)
	{
		_cameraMan->injectKeyUp(evt);
		return true;
	}

	virtual bool mouseMoved(const MouseEvent &evt )
	{
		_cameraMan->injectMouseMove(evt);
		return true;
	}

	virtual bool mousePressed(const MouseEvent &evt, MouseButtonID id )
	{
		_cameraMan->injectMouseDown(evt, id);
		return true;
	}

	virtual bool mouseReleased(const MouseEvent &evt, MouseButtonID id )
	{
		_cameraMan->injectMouseUp(evt, id);
		return true;
	}
};