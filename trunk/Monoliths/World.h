#pragma once
#include "stdafx.h"
#include "GameObject.h"
//#include "Ground.h"
#include "PhysicsManager.h";

using namespace Ogre;

extern "C" {
	void LOG(String msg);
}

class Ground;

class World
{
private:
	SceneManager* _sceneManager;
	PhysicsManager* _physicsManager;
	SceneNode* _rootNode;
	std::vector<GameObject*> _gameObjects;
	int _idCounter;

	Ground* _ground;
	float _mapSize;

	//typedef union
	//{
	//	float f;
	//	int i;
	//} ShaderObjectID;

	float _shaderObjectIdCounter;

	void createGameObjects();
	void createLights();

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
		_mapSize(mapSize)
	{
		_physicsManager = physicsManager;
		_shaderObjectIdCounter = 0;
		_rootNode = _sceneManager->getRootSceneNode();
	}

	
	void addGameObject(GameObject* gameObject)
	{
		_gameObjects.push_back(gameObject); 
		gameObject->init(this);
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

	void init()
	{
		createLights();
		createGameObjects();
		//initGameObjects();
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
};