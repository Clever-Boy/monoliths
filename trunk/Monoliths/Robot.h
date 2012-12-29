#pragma once
#include "stdafx.h"
#include "World.h"
#include "Action.h"
#include "Character.h"

using namespace Ogre;

class Robot : public Character
{
	Ogre::Vector3 _initialPosition;
	float _initialLookingAngle;

public:

	virtual float getScale() const 
	{ 
		return 2;
	}

	Robot(Ogre::Vector3 initialPosition = Ogre::Vector3::ZERO, float initialLookingAngle = 0.01)
		: _initialPosition(initialPosition),
		  _initialLookingAngle(initialLookingAngle)
	{
	}

	virtual Entity* createEntity(World* world) 
	{ 
		Entity* entity = world->getSceneManager()->
			createEntity(world->getNextId("robot"), "robot.mesh");
#ifndef BASIC_GRAPHICS
		entity->setMaterialName("Robot");
#else
		entity->setMaterialName("Robot_basic");
#endif
		return entity;
	}

	virtual void getInitialPose(Ogre::Vector3& position, Radian& initialAngle) 
	{ 
		position = _initialPosition;
		initialAngle = Radian(_initialLookingAngle);
	}

	virtual Action* getInitialAction()
	{
		return Action::ROBOT_IDLE;
	}
};
