#pragma once
#include "stdafx.h"
#include "Game.h"
#include "GameController.h"
#include "World.h"
#include "BoxObject.h"
#include "NavMesh.h"
#include "NavMeshDebugObject.h"
#include <random>
#include "MoveToPointStrategy.h"

using namespace Ogre;
using namespace OIS;



class FreeCameraController : public GameController
{
	enum State
	{
		ST_NORMAL,
		ST_NAVMESH,
		ST_PATH
	};

	OgreBites::SdkCameraMan _cameraMan;
	Camera* _camera;
	float _lastBoxTime;
	State _state;
	World* _world;
	PathDebugObject* _kakkancs;
	std::default_random_engine _rnd;
	std::vector<Ray> _rajz;
	MoveToPointStrategy* _strat;

public:
	FreeCameraController(Camera* camera, World* world)
		: _cameraMan(camera), _lastBoxTime(0), _state(ST_NORMAL), _kakkancs(NULL), _strat(NULL),
		_rnd(std::random_device()())
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

#ifdef _DEBUG
		if (_strat != NULL && _strat->isActive() && _strat->currentPath() != NULL)
		{
			if (_kakkancs != NULL)
			{
				_world->removeGameObject(_kakkancs);
			}
			
			Ogre::Vector2 pos = _world->getPuppie()->getPos2d();
			
			_kakkancs = new PathDebugObject(_strat->currentPath(), pos, _strat->destination(), _strat->nextCorner());
			_world->addGameObject(_kakkancs);
		}
#endif
		if (_state == ST_NORMAL)
		{
			_world->showOnly(ET_STANDARD);
		}
		else if (_state == ST_NAVMESH)
		{
			_world->showOnly(ET_MONOLITH_DBG);
		}
		else
		{
			_world->showOnly(ET_PATH_DBG);
		}
	}

	virtual bool keyPressed(const KeyEvent &evt)
	{
		OutputDebugStringA("keyPressed\n");
		_cameraMan.injectKeyDown(evt);
		if (evt.key == KC_X)
		{
			_state=(State)((_state + 1) % 3);
		}
		else if (evt.key == KC_P)
		{
			const std::vector<NavMeshTriangle>& triangles = _world->getNavMesh().getTriangles();

			int triIdx1 = _rnd() % triangles.size();
			int triIdx2 = _rnd() % triangles.size();
			doShowPath(triIdx1, triIdx2);
		}
		else if (evt.key == KC_U)
		{
			doShowPath(2, 6);
		}
		else if (evt.key >= KC_1 && evt.key <= KC_0)
		{
			int idx = evt.key-KC_1;
			const Ray& raj = _rajz[idx];
			doPickRay(raj);
		}
		return true;
	}

	void doShowPath(int triIdx1, int triIdx2)
	{
		NavMesh& navMesh = _world->getNavMesh();
		const std::vector<NavMeshTriangle>& triangles = navMesh.getTriangles();
		
		const NavMeshTriangle& tri1 = triangles[triIdx1];
		const NavMeshTriangle& tri2 = triangles[triIdx2];

		//std::vector<NavMeshTriangle*> path = navMesh.findPathBetween(&tri1, &tri2);
		TrianglePath* path = _world->getPathFinder()->findPathBeetween(&tri1, &tri2);
		if (_kakkancs != NULL)
		{
			_world->removeGameObject(_kakkancs);
		}
		
		_kakkancs = new PathDebugObject(path, tri1.center, tri2.center, path->nextCorner(tri1.center, tri2.center));
		_world->addGameObject(_kakkancs);
		_state == ST_PATH;
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

		Ogre::Ray ray = Ogre::Ray(_camera->getPosition(),  _camera->getDirection());

		if (id == OIS::MB_Right)
		{	
			_rajz.push_back(ray);
			doPickRay(ray);
		}
		else if (id == OIS::MB_Left)
		{
			Ogre::Vector3 impact;
			Character* pup = _world->getPuppie();
			GameObject* obj = _world->pickNearest(ray, impact);
			_strat = new MoveToPointStrategy(Vector2(impact.x, impact.z),&_world->getNavMesh());
			_world->getPuppie()->setStrategy(_strat);
			//Ogre::Vector3 dir = impact-pup->getPosition();
			//_world->getPuppie()->turn(dir);
		}
		
		return true;
	}

	void doPickRay(const Ray& ray)
	{
		
		LOG("******************* "+StringConverter::toString(ray.getOrigin())+" / "+StringConverter::toString(ray.getDirection()));
		
		Ogre::Vector3 impact;
		GameObject* obj = _world->pickNearest(ray, impact);
		if (obj == NULL) return;

		if (_state == ST_NAVMESH)
		{
			Ogre::Vector2 p = Ogre::Vector2(impact.x, impact.z);
			NavMeshTriangle* tri = _world->getNavMesh().findTriangleOfPoint(p);
			if (tri != NULL)
			{
				TriangleDebugObject* obj = new TriangleDebugObject(tri, &_world->getNavMesh());
				_world->addGameObject(obj);
			}
		}
		else
		{
			for (auto i = _world->gameObjectsBegin(); i != _world->gameObjectsEnd(); i++)
			{
				(*i)->setDebugMarked(false);
			}
			obj->setDebugMarked(true);
		}
	}

	virtual bool mouseReleased(const MouseEvent &evt, MouseButtonID id )
	{
		_cameraMan.injectMouseUp(evt, id);
		return true;
	}
};