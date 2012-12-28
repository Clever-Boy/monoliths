#pragma once
#include "stdafx.h"
#include "GameObject.h"
#include "PhysicsManager.h"
#include "World.h"

using namespace Ogre;
using namespace physx;

class Action;

class Character : public GameObject
{
	PxCapsuleController* _physController;
	AnimationState* _currentAnimState;
	std::vector<Action*> _actions;
	Radian _lookingAngle;

public:

	GameObjectElement& getElement()
	{
		return _elements[0];
	}

	Character() : _currentAnimState(NULL), _lookingAngle(0)
	{
	}

	void addAction(Action* action);
	void clearActions();
	/*template<class TActionsEnumeration> void setActions(TActionsEnumeration actions);*/

	//void setAction(Action* action);
	void turn(float angle)
	{
		_lookingAngle += Radian(angle);
	}

	float getLookingAngle()
	{
		return _lookingAngle.valueRadians();
	}

	PxCapsuleController* getPhysController()
	{
		return _physController;
	}

	Ogre::Vector3 getLookDirection()
	{
		return Ogre::Vector3(-Math::Sin(_lookingAngle), 0, -Math::Cos(_lookingAngle));
	}

	virtual Entity* createEntity(World* world) { return NULL; }
	virtual Action* getInitialAction();
	virtual void getInitialPose(Ogre::Vector3& position, Radian& initialAngle) { }
	
	virtual void initCapsuleControllerDesc(PxCapsuleControllerDesc& desc, PhysicsManager* physicsMgr);

	virtual void initImpl(World* world);
	virtual void act(World* world, float totalTime, float dt);
	
	virtual void update(World* world, float totalTime, float dt);
};