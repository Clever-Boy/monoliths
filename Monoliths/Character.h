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
	Ogre::Quaternion _meshOrientation;

	float _radius;
	float _height;
	float _capsuleOffsetY;

public:

	bool hasElement()
	{
		return _elements.size() > 0;
	}

	GameObjectElement& getElement()
	{
		return _elements[0];
	}

	Character(float radius = 30, float height = 200, Ogre::Quaternion meshOrientation = Ogre::Quaternion::ZERO) 
		: _radius(radius),
		  _height(height),
		  _capsuleOffsetY(height*0.5f/PHYSICS2WORLD_SCALE + 0.01f),
		  _currentAnimState(NULL), 
		  _lookingAngle(0),
		  _meshOrientation(meshOrientation)
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

	Ogre::Vector3 getLookingDirection()
	{
		return Ogre::Vector3(Math::Cos(_lookingAngle), 0, -Math::Sin(_lookingAngle));
		//return Ogre::Vector3(-Math::Sin(_lookingAngle), 0, -Math::Cos(_lookingAngle));
	}

	virtual Entity* createEntity(World* world) { return NULL; }
	virtual Action* getInitialAction();
	virtual void getInitialPose(Ogre::Vector3& position, Radian& initialAngle) { }
	
	virtual void initCapsuleControllerMisc(PxCapsuleControllerDesc& desc, PhysicsManager* physicsMgr);

	virtual void initImpl(World* world);
	virtual void act(World* world, float totalTime, float dt);
	
	virtual void update(World* world, float totalTime, float dt);
};