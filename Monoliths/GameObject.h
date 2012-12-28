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

	void updateFromActor();
	void updateNode();
};

extern const float GAMEOBJECT_PHYSICS_SCALE;

class GameObject
{
private:
	SceneManager* _sceneManager;
	SceneNode* _node;
	
protected:
	std::vector<GameObjectElement> _elements;

	GameObject()
	{
	}

	virtual void initImpl(World* world) = 0;

	virtual float getCtrDetectionExponent()
	{
		return 1;
	}

	virtual float getScale() const 
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

	virtual void postInit() { }

public:

	SceneNode* getNode()
	{
		return _node;
	}

	void init(World* world);
	
	virtual void act(World* world, float totalTime, float dt) { }
	virtual void update(World* world, float totalTime, float dt);
};