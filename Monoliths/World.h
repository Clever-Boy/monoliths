#pragma once
#include "stdafx.h"
#include "GameObject.h"
#include "Ground.h"

using namespace Ogre;

extern "C" {
	void KAKA(String msg);
}

class World
{
private:
	SceneManager* _sceneManager;
	SceneNode* _rootNode;
	std::vector<GameObject*> _gameObjects;
	int _idCounter;

	Ground* _ground;
	float _mapSize;

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
		_rootNode = _sceneManager->getRootSceneNode();
	}

	void init()
	{
		createLights();
		createGameObjects();
		initGameObjects();
	}

	String getNextId()
	{
		return String("gameObject")+StringConverter::toString(_idCounter++,2);
	}

	SceneManager* getSceneManager()
	{
		return _sceneManager;
	}

	Ground* getGround()
	{
		return _ground;
	}
	
	std::vector<GameObject*> getAllExceptGround()
	{
		std::vector<GameObject*> result;
		for (int i = 0; i < _gameObjects.size(); i++)
		{
			if (_gameObjects[i] != _ground)
			{
				result.push_back(_gameObjects[i]);
			}
		}
		return result;
	}
};