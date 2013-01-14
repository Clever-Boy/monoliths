#pragma once
#include "stdafx.h"
#include "common.h"
#include "GameObject.h"
//#include "Ground.h"
#include "PhysicsManager.h";
//#include "Character.h"
#include "NavMesh.h"
#include "PathFinder.h"	

using namespace Ogre;

extern "C" {
	void LOG(String msg);
}

class Ground;
class Character;

class World
{
private:
	SceneManager* _sceneManager;
	PhysicsManager* _physicsManager;
	SceneNode* _rootNode;
	std::vector<GameObject*> _gameObjects;
	int _idCounter;

	Ground* _ground;
	Character* _puppie;
	float _mapSize;
	NavMesh _navMesh;
	PathFinder _pathFinder;

	//typedef union
	//{
	//	float f;
	//	int i;
	//} ShaderObjectID;

	float _shaderObjectIdCounter;

	void createGameObjects();
	void createLights();
	void initNavMesh();
	void initDebugObjects();

/*
	void initGameObjects()
	{
		for (int i = 0; i < _gameObjects.size(); i++)
		{
			_gameObjects[i]->init(this);
		}
	}
*/

public:


	World(SceneManager* sceneManager, PhysicsManager* physicsManager, float mapSize) 
		: _sceneManager(sceneManager),
		_physicsManager(physicsManager),
		_idCounter(0), 
		_mapSize(mapSize),
		_pathFinder(&_navMesh)
	{
		_physicsManager = physicsManager;
		_shaderObjectIdCounter = 0;
		_rootNode = _sceneManager->getRootSceneNode();
	}
	
	NavMesh& getNavMesh()
	{
		return _navMesh;
	}

	PathFinder* getPathFinder()
	{
		return &_pathFinder;
	}

	Character* getPuppie()
	{
		return _puppie;
	}
	
	void addGameObject(GameObject* gameObject)
	{
		_gameObjects.push_back(gameObject); 
		gameObject->init(this);
	}

	void removeGameObject(GameObject* gameObject)
	{
		//std::find(_gameObjects.begin(), _gameObjects.end(), gameObject);
		auto i = std::find(_gameObjects.begin(), _gameObjects.end(), gameObject);
		_gameObjects.erase(i);
		_sceneManager->getRootSceneNode()->removeChild(gameObject->getNode());
		
		//std::remove(_gameObjects.
		//_gameObjects.erase(
	}

	void act(float totalTime, float dt)
	{
		for (auto i = _gameObjects.begin();i != _gameObjects.end(); i++)
		{
			(*i)->act(this, totalTime, dt);
		}
	}

	void update(float totalTime, float dt)
	{
		for (auto i = _gameObjects.begin(); i != _gameObjects.end(); i++)
		{
			(*i)->update(this, totalTime, dt);
		}
	}

	void showOnly(ElementType elementType);

	void init()
	{
		createLights();
		createGameObjects();
		initNavMesh();
		initDebugObjects();
	}

	String getNextId(String prefix = "object")
	{
		return prefix+ String("_")+StringConverter::toString(_idCounter++,3);
	}

	float getNexShaderObjectId()
	{
		//_shaderObjectIdCounter.i++;
		_shaderObjectIdCounter+=0.001;
		return _shaderObjectIdCounter;
	}

	PhysicsManager* getPhysicsManager()
	{
		return _physicsManager;
	}

	SceneManager* getSceneManager()
	{
		return _sceneManager;
	}

	Ground* getGround()
	{
		return _ground;
	}

	GameObject* pickNearest(const Ogre::Ray& ray)
	{
		return pickNearest(ray, Ogre::Vector3());
	}

	
	GameObject* pickNearest(const Ogre::Ray& ray, Ogre::Vector3& impact)
	{
		PxActor* actor = _physicsManager->pickNearestActor(ray, impact);
		if (actor != NULL)
		{
			return (GameObject*)actor->userData;
		}
		else
		{
			return NULL;
		}
	}

	std::vector<GameObject*>::iterator gameObjectsBegin()
	{
		return _gameObjects.begin();
	}

	std::vector<GameObject*>::iterator gameObjectsEnd()
	{
		return _gameObjects.end();
	}
};