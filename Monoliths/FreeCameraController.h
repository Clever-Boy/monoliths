#pragma once
#include "stdafx.h"
#include "Game.h"
#include "GameController.h"
#include "World.h"
#include "BoxObject.h"
#include "NavMesh.h"

using namespace Ogre;
using namespace OIS;

class FreeCameraController : public GameController
{
	OgreBites::SdkCameraMan _cameraMan;
	Camera* _camera;

	float _lastBoxTime;
	bool _showDbg;
	World* _world;

public:
	FreeCameraController(Camera* camera, World* world)
		: _cameraMan(camera), _lastBoxTime(0), _showDbg(false)
	{
		_camera = camera;
		_world = world;
	}

	virtual Camera* getCamera()
	{
		return _camera;
	}

	virtual void onUpdating(const FrameEvent& evt, Game* game)
	{ 
		//OutputDebugStringA("frameRenderingQueued\n");
		_cameraMan.frameRenderingQueued(evt);

		if (game->getKeyboard()->isKeyDown(KC_SPACE) && game->getTotalTime() - _lastBoxTime > 0.2)
		{
			OutputDebugString("PTYUUUUUUU\n");
			_lastBoxTime = game->getTotalTime();
			BoxObject* box = new BoxObject(_camera->getPosition(), _camera->getDirection().normalisedCopy()*30);
			game->getWorld()->addGameObject(box);
		}

		game->getWorld()->showOnly( _showDbg ? ET_MONOLITH_DBG : ET_STANDARD);
	}

	virtual bool keyPressed(const KeyEvent &evt)
	{
		OutputDebugStringA("keyPressed\n");
		_cameraMan.injectKeyDown(evt);
		if (evt.key == KC_X)
		{
			_showDbg = !_showDbg;
		}
		else if (evt.key == KC_P)
		{
			NavMesh& navMesh = _world->getNavMesh();
			const std::vector<NavMeshTriangle>& triangles = navMesh.getTriangles();
			int triIdx1 = std::rand() / triangles.size();
			int triIdx2 = std::rand() / triangles.size();

			const NavMeshTriangle& tri1 = triangles[triIdx1];
			const NavMeshTriangle& tri2 = triangles[triIdx2];

			std::vector<NavMeshTriangle*> path = navMesh.findPathBetween(&tri1, &tri2);
		}
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