#pragma once
#include "stdafx.h"

using namespace Ogre;

class World;

class GameObject
{
private:
	SceneManager* _sceneManager;
	std::vector<Entity*> _entities;
	SceneNode* _node;
	Ogre::Vector3 _position;
	Ogre::Quaternion _orientation;
protected:

	GameObject(Ogre::Vector3 position = Ogre::Vector3::ZERO, Ogre::Quaternion orientation = Ogre::Quaternion::IDENTITY)
	{
		_position = position;
		_orientation = orientation;
	}

	virtual void createEntitiesImpl(std::vector<Entity*>& entities, World* world) = 0;
	virtual float getCtrDetectionExponent()
	{
		return 1;
	}

public:
	/*std::vector<MovableObject*> getEntities()
	{
		return _entities;
	}
*/
	typedef std::vector<Entity*>::const_iterator EntityIterator;

	EntityIterator getEntitiesIterator()
	{
		return _entities.begin();
	}

	SceneNode* getNode()
	{
		return _node;
	}

	void init(World* world);
	
	void setPosition(float x, float y, float z)
	{
		_position = Ogre::Vector3(x, y, z);
		_node->setPosition(_position);
	}

	Ogre::Vector3 getPosition()
	{
		return _position;
	}
};