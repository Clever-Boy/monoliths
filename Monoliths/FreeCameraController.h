#pragma once
#include "stdafx.h"
#include "Game.h"
#include "GameController.h"

using namespace Ogre;
using namespace OIS;

class FreeCameraController : public GameController
{
	OgreBites::SdkCameraMan _cameraMan;
	Camera* _camera;

public:
	FreeCameraController(Camera* camera)
		: _cameraMan(camera)
	{
		_camera = camera;
	}

	virtual Camera* getCamera()
	{
		return _camera;
	}

	virtual void onUpdating(const FrameEvent& evt, Game* game)
	{ 
		_cameraMan.frameRenderingQueued(evt);
	}

	virtual bool keyPressed(const KeyEvent &evt)
	{
		_cameraMan.injectKeyDown(evt);		
		return true;
	}

	virtual bool keyReleased(const KeyEvent &evt)
	{
		_cameraMan.injectKeyUp(evt);
		return true;
	}

	virtual bool mouseMoved(const MouseEvent &evt )
	{
		_cameraMan.injectMouseMove(evt);
		return true;
	}

	virtual bool mousePressed(const MouseEvent &evt, MouseButtonID id )
	{
		_cameraMan.injectMouseDown(evt, id);
		return true;
	}

	virtual bool mouseReleased(const MouseEvent &evt, MouseButtonID id )
	{
		_cameraMan.injectMouseUp(evt, id);
		return true;
	}
};