#pragma once
#include "stdafx.h"
#include "World.h"
#include "Action.h"
#include "Character.h"

using namespace Ogre;

class Player : public Character
{
	Ogre::Vector3 _initialPosition;
	float _initialLookingAngle;

public:

	Player(Ogre::Vector3 initialPosition = Ogre::Vector3::ZERO, float initialLookingAngle = 0.01)
		: _initialPosition(initialPosition),
		  _initialLookingAngle(initialLookingAngle)
	{
	}

	virtual void getInitialPose(Ogre::Vector3& position, Radian& initialAngle) 
	{ 
		position = _initialPosition;
		initialAngle = Radian(_initialLookingAngle);
	}

	virtual void act(World* world, float totalTime, float dt) 
	{
		Character::act(world, totalTime, dt);
	}

	virtual void setActions(const std::vector<Action*>& actions)
	{
		Character::setActions(actions);
	}

	virtual void setDead(World* world) 
	{
		Character::setDead(world);
		world->getGameStateListener()->notifyGameOver();
	}

	virtual void shoot(World* world, float damage) 
	{ 
		Character::shoot(world, damage);
		world->getGameStateListener()->notifyShot();
	}
};