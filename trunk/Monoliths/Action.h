#pragma once
#include "stdafx.h"
#include "common.h"
#include "World.h"
//#include "Character.h"

using namespace Ogre;
using namespace physx;

class Character;

enum ActionType
{
	AT_DEFAULT = 0,
	AT_IDLE,
	AT_WALK,
	AT_SHOOT,
	AT_DYING,
	AT_DEAD
};

class Action
{
	float _animSpeed;
	ActionType _actionType;
	bool _cloneOverriden;

public:

	Action(float animSpeed = 1, ActionType actionType = AT_DEFAULT) 
		: _animSpeed(animSpeed), 
		_actionType(actionType), 
		_cloneOverriden(true)
	{ 
	}

	ActionType getActionType() const { return _actionType; }
	float getAnimSpeed() const { return _animSpeed; }

	virtual bool tryGetAnimationName(Ogre::String& name) const { return false; }
	virtual void apply(Character* character, World* world, float t, float dt) { }
	virtual Action* cloneFromPrototype(Character* character) { _cloneOverriden = false; return this; }

	virtual bool loopAnimation() const { return true; }
	virtual float animationStart(AnimationState* animState) const { return 0; }

	static Action* const DEFAULT;
	static Action* const ROBOT_IDLE;
	static Action* const ROBOT_WALK;
	static Action* const ROBOT_SHOOT;
	static Action* const ROBOT_DYING;
	static Action* const ROBOT_DEAD;

	static Action* const PLAYER_IDLE;
	static Action* const PLAYER_WALKINGS[8];
	static Action* const PLAYER_SHOOT;
	static Action* const PLAYER_DYING;
	static Action* const PLAYER_DEAD;
	
	//const static Action 
};

class Idle : public Action
{
	String _animName;
public:

	Idle(Ogre::String animName = "", float animSpeed = 1, ActionType actionType = AT_IDLE) 
		: _animName(animName), Action(animSpeed, actionType)
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
	static const float PI_OVER_4;

	float _displacement;
	int _directionIndex;
	Ogre::Matrix3 _rot;

public:
	Movement(float displacement, int directionIndex = 0, float animSpeed = 1, String animName = "") :
		_displacement(displacement/PHYSICS2WORLD_SCALE),
		_directionIndex(directionIndex),
		Idle(animName, animSpeed, AT_WALK)
	{
		_rot.FromAngleAxis(Ogre::Vector3::UNIT_Z, Radian(-PI_OVER_4*(float)directionIndex));
	}

	virtual void apply(Character* character, World* world, float t, float dt);
};

class Shoot : public Idle
{
	float _timer;
	float _shootingDelay;
public:
	Shoot(const String& animName = "", float shootingDelay = 1, float animSpeed = 1)
		: Idle(animName, animSpeed, AT_SHOOT),
		  _timer(0),
		  _shootingDelay(shootingDelay)
	{
	}

	virtual Action* cloneFromPrototype(Character* character)
	{
		String animName;
		tryGetAnimationName(animName);
		return new Shoot(animName, _shootingDelay, getAnimSpeed());
	}

	virtual void apply(Character* character, World* world, float t, float dt);
};

class Dying : public Idle
{
	Action* _deadAction;
	float _timer;
public:
	Dying(Action* deadAction, const String& animName = "", float animSpeed = 1) 
		: Idle(animName, animSpeed, AT_DYING),
		_timer(0),
		_deadAction(deadAction)
	{
	}

	virtual Action* cloneFromPrototype(Character* character);
	virtual void apply(Character* character, World* world, float t, float dt);

	virtual bool loopAnimation() const { return false; }
};

class Dead : public Idle
{
public:
	Dead(String animName = "")
		: Idle(animName,0, AT_DEAD)
	{
	}

	virtual void apply(Character* character, World* world, float t, float dt);
	virtual bool loopAnimation() const { return false; }
	virtual float animationStart(AnimationState* animState) const;

	virtual Action* cloneFromPrototype(Character* character);
};
