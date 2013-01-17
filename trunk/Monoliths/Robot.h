#pragma once
#include "stdafx.h"
#include "World.h"
#include "Action.h"
#include "Character.h"
#include <random>

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
		  _initialLookingAngle(initialLookingAngle),
		  Character(DEFAULT_CHARACTER_RADIUS, 200, Ogre::Quaternion::ZERO, 100, Action::ROBOT_DYING)
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

	virtual float getGunDamage(float distance) const 
	{ 
		return std::ceil(6*std::exp(-distance*0.0002));
	}
};
