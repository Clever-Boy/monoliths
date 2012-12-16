#pragma once
#include "stdafx.h"
#include "GameController.h"

using namespace physx;
using namespace Ogre;

enum NinaState
{
	NS_IDLE,
	NS_WALK_FORWARD,
	NS_WALK_BACKWARDS
};

class NinjaObject
{
private:
	Entity* _entity;
	SceneNode *_node;
	PxCapsuleController* _physController;
	PxPhysics* _physics;
	Ogre::Vector3 _initialPosition;

	Radian _lookingAngle;

	AnimationState* _idleAnim;
	AnimationState* _walkAnim;

	AnimationState* _currentAnim;
	//bool _walking;
	
	NinaState _state;

	void updateOrientation()
	{
		_node->setOrientation(Quaternion(Radian(_lookingAngle),Ogre::Vector3::UNIT_Y));
	}

	void setAnimation(AnimationState* anim, bool loop = true)
	{
		if (_currentAnim != NULL)
		{
			_currentAnim->setEnabled(false);
		}
		
		_currentAnim = anim;
		_currentAnim->setEnabled(true);
		_currentAnim->setLoop(loop);
	}

public:
	NinjaObject(float x, float y, float z, GameController* controller)
		: _currentAnim(NULL)
	{
		
		_initialPosition = Ogre::Vector3(x,y,z);
		SceneManager* mgr = controller->getSceneManager();
		_entity = mgr->createEntity(controller->generateId(), "ninja.mesh");
		_node = controller->addEntity(_entity, "BuziNinnya");
		
		_idleAnim = _entity->getAnimationState("Idle2");
		_walkAnim = _entity->getAnimationState("Walk");
		setAnimation(_idleAnim);
		//_walking = false;
		_state = NS_IDLE;

		_lookingAngle = Radian(0);
		updateOrientation();
		_node->setPosition(_initialPosition);

		_physics = controller->getPhysics();
		PxControllerManager* pxMgr = controller->getPxControllerManager();
		PxCapsuleControllerDesc desc;
		desc.setToDefault();
		desc.radius = 30;
		desc.height = 200;
		desc.upDirection = PxVec3(0, 1, 0);

		desc.slopeLimit = 0.707;
		desc.stepOffset = 0.5;
		desc.callback = NULL;
		desc.position.set(0,0,0);
		desc.material = _physics->createMaterial(0.5, 0.5, 0.5);
		
		if (!desc.isValid())
		{
			std::cerr << "BUDOS RIBANCZ";
		}

		PxController* c = pxMgr->createController(*_physics, controller->getPhysicsScene(), desc);
		
		_physController = (PxCapsuleController*)c;
		
	}

	void update(float dt)
	{
		if (_state != NS_IDLE)
		{
			setAnimation(_walkAnim);
			

			Ogre::Vector3 dv = getLookingDirection()*dt*140.0f;

			if (_state == NS_WALK_BACKWARDS)
			{
				dv*=-1;
			}

			PxControllerFilters filters;
			_physController->move(PxVec3(dv.x, dv.y, dv.z), 1, dt, filters);
			
			PxExtendedVec3 pos = _physController->getPosition();
			_node->setPosition(pos.x, pos.y, pos.z);

			//Ogre::Vector3 pos = _node->getPosition()+dv;
			//_node->setPosition(pos);
			
		}
		else
		{
			setAnimation(_idleAnim);
		}
		_currentAnim->addTime(dt);
	}

	const Ogre::Quaternion& getOrientation() const
	{
		return _node->getOrientation();
	}

	const Ogre::Vector3& getPosition() const
	{
		return _node->getPosition();
	}

	const Ogre::Vector3& getLookingDirection()
	{
		return Ogre::Vector3(-Math::Sin(_lookingAngle), 0, -Math::Cos(_lookingAngle));
	}
	
	void turn(float value)
	{
		_lookingAngle+=Radian(value);
		updateOrientation();
	}

	void setState(NinaState state)
	{
		_state = state;
	}
};