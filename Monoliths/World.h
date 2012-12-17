#pragma once
#include "stdafx.h"
#include "GameObject.h"
//#include "Ground.h"

using namespace Ogre;

extern "C" {
	void LOG(String msg);
}

class Ground;

class World
{
private:
	SceneManager* _sceneManager;
	SceneNode* _rootNode;
	std::vector<GameObject*> _gameObjects;
	int _idCounter;

	Ground* _ground;
	float _mapSize;

	typedef union
	{
		float f;
		int i;
	} ShaderObjectID;

	float _shaderObjectIdCounter;

	void createGameObjects();
	void createLights();


	void initGameObjects()
	{
		for (int i = 0; i < _gameObjects.size(); i++)
		{
			_gameObjects[i]->init(this);
		}
	}

	void addGameObject(GameObject* gameObject)
	{
		_gameObjects.push_back(gameObject);
	}

public:

	World(SceneManager* sceneManager, float mapSize) 
		: _sceneManager(sceneManager), _idCounter(0), _mapSize(mapSize)
	{
		_shaderObjectIdCounter = 0;
		_rootNode = _sceneManager->getRootSceneNode();
	}

	void update(float totalTime, float dt)
	{
	}

	void init()
	{
		createLights();
		createGameObjects();
		initGameObjects();
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

	SceneManager* getSceneManager()
	{
		return _sceneManager;
	}

	Ground* getGround()
	{
		return _ground;
	}
};