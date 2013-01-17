#include "stdafx.h"
#include "Action.h"
#include "Character.h"

const float Movement::PI_OVER_4 = Ogre::Math::PI/4;

Action* const Action::DEFAULT = new Action();

Action* const Action::ROBOT_IDLE = new Idle("Idle");
Action* const Action::ROBOT_WALK = new Movement(200, 0, 1.2, "Walk");
Action* const Action::ROBOT_SHOOT = new Shoot("Shoot", 0.5);
Action* const Action::ROBOT_DEAD = new Dead("Die");
Action* const Action::ROBOT_DYING = new Dying(Action::ROBOT_DEAD, "Die", 0.5);


Action* const Action::PLAYER_IDLE = new Idle();
Action* const Action::PLAYER_WALKINGS[8] = {
	new Movement(300, 0),
	new Movement(300, 1),
	new Movement(300, 2),
	new Movement(300, 3),
	new Movement(300, 4),
	new Movement(300, 5),
	new Movement(300, 6),
	new Movement(300, 7)
};

Action* const Action::PLAYER_SHOOT = new Shoot("", 1);
Action* const Action::PLAYER_DEAD = new Dead();
Action* const Action::PLAYER_DYING = new Dying(Action::PLAYER_DEAD);



void Movement::apply(Character* character, World* world, float t, float dt) 
{ 
	PxCapsuleController* controller = character->getPhysController();
	float look = character->getLookingAngle();
	Ogre::Vector3 lookDirOrig = character->getLookingDirection();
		
	Ogre::Vector3 lookDirTr = Ogre::Vector3(lookDirOrig.x, lookDirOrig.z, 0);
	lookDirTr = _rot*lookDirTr;
		
	PxVec3 disp = PxVec3(lookDirTr.x, 0, lookDirTr.y) * dt* _displacement;
	//LOG(String("DT: ")+StringConverter::toString(dt) + String(" MOV: ") +StringConverter::toString(Ogre::Vector3(disp.x, disp.y, disp.z) ));
	PxControllerFilters filters;
	controller->move(disp, 0.0001, dt, filters);
}

void Shoot::apply(Character* character, World* world, float t, float dt) 
{ 
	if (_timer==0)
	{
		world->shoot(character);
	}
	_timer+=dt;
	if (_timer>_shootingDelay)
	{
		_timer = 0;
	}
}

Action* Dying::cloneFromPrototype(Character* character)
{
	String animName;
	tryGetAnimationName(animName);
	return new Dying(_deadAction, animName, getAnimSpeed());
}

void Dying::apply(Character* character, World* world, float t, float dt) 
{ 
	_timer+=dt*getAnimSpeed();
	if (_timer > character->getAnimLength())
	{
		character->setAction(_deadAction);
	}
}

void Dead::apply(Character* character, World* world, float t, float dt)
{
}

float Dead::animationStart(AnimationState* animState) const
{
	return animState->getLength()*0.99f;
}

Action* Dead::cloneFromPrototype(Character* character)
{
	character->releasePhysicsController();
	return Action::cloneFromPrototype(character);
}