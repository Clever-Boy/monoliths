#pragma once
#include "stdafx.h"

using namespace Ogre;
using namespace physx;

class World;

struct GameObjectElement
{
	Ogre::Vector3 position;
	Ogre::Quaternion orientation;
	Entity* entity;
	PxRigidActor* actor;
	SceneNode* node;

	void init(SceneNode* node, World* world);
	void updateNode();
};

class GameObject
{
private:
	

	SceneManager* _sceneManager;
	/*std::vector<Entity*> _entities;
	std::vector<PxActor*> _actors;*/
	std::vector<GameObjectElement> _elements;

	SceneNode* _node;
	
protected:

	GameObject()
	{
	}

	virtual void initImpl(World* world) = 0;

	virtual float getCtrDetectionExponent()
	{
		return 1;
	}
protected:

	void addElement(const Ogre::Vector3& position, const Ogre::Quaternion& orientation,
		Ogre::Entity* entity = NULL, PxRigidActor* actor = NULL)
	{
		GameObjectElement element;
		element.position = position;
		element.orientation = orientation;
		element.entity = entity;
		element.actor = actor;

		_elements.push_back(element);
	}

public:
	/*std::vector<MovableObject*> getEntities()
	{
		return _entities;
	}
*/
	//typedef std::vector<Entity*>::const_iterator EntityIterator;

	/*EntityIterator getEntitiesIterator()
	{
		return _entities.begin();
	}*/

	SceneNode* getNode()
	{
		return _node;
	}

	void init(World* world);
	
	virtual void act(World* world, float totalTime, float dt) { }
	virtual void update(World* world, float totalTime, float dt);

	virtual bool isStatic() { return true; }

	/*
	void setPosition(float x, float y, float z)
	{
		_position = Ogre::Vector3(x, y, z);
		_node->setPosition(_position);
	}

	Ogre::Vector3 getPosition()
	{
		return _position;
	}*/
};