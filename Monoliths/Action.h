#pragma once
#include "stdafx.h"

#include "Character.h"

using namespace Ogre;
using namespace physx;

class Action
{
public:

	Action() { }

	virtual bool tryGetAnimationName(Ogre::String& name) const { return false; }
	virtual void apply(Character* character, World* world, float t, float dt) { }

	static Action* const DEFAULT;
	static Action* const ROBOT_IDLE;
	static Action* const ROBOT_WALK;
	//const static Action 
};

class Idle : public Action
{
	String _animName;
public:

	Idle(Ogre::String animName = "") : _animName(animName)
	{
	}

	virtual bool tryGetAnimationName(Ogre::String& name) const 
	{ 
		if (!_animName.empty())
		{
			name = _animName;
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual void apply(Character* character, World* world, float t, float dt) { }
};

class Movement : public Idle
{
	float _displacement;
	
public:
	Movement(float displacement, String animName) :
		_displacement(displacement/GAMEOBJECT_PHYSICS_SCALE),
		Idle(animName)
	{
	}

	virtual void apply(Character* character, World* world, float t, float dt) 
	{ 
		PxCapsuleController* controller = character->getPhysController();
		float look = character->getLookingAngle();
		Ogre::Vector3 lookDir = character->getLookDirection();
		PxVec3 disp = PxVec3(lookDir.x, lookDir.y, lookDir.z) * _displacement;
		controller->move(disp, 1, dt, PxControllerFilters());
	}
};