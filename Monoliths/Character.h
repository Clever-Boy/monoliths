#pragma once
#include "stdafx.h"
#include "common.h"
#include "GameObject.h"
#include "PhysicsManager.h"
#include "World.h"
#include "Strategy.h"
#include "NavMesh.h"
#include "Action.h"

using namespace Ogre;
using namespace physx;

class Action;

class Character : public GameObject
{
	PxCapsuleController* _physController;
	AnimationState* _currentAnimState;
	float _currentAnimSpeed;
	std::vector<Action*> _actions;
	Radian _lookingAngle;
	Radian _elevationAngle;
	Ogre::Quaternion _meshOrientation;

	float _radius;
	float _height;
	float _capsuleOffsetY;
	Strategy* _strategy;
	NavMeshTriangle* _currentTriangle;

	bool _dead;
	float _health;
	Action* _dyingAction;
	
	bool _actionsChanged;
	std::vector<Action*> _updatedActions;

	void setActionsImpl();
	void actionAdded(Action* action);
	

public:

	float getHealth() const
	{
		return _health;
	}

	NavMeshTriangle* currentNavMeshTriangle() const
	{
		return _currentTriangle;
	}

	void setStrategy(Strategy* strategy)
	{
		_strategy = strategy;
	}

	const Strategy* getStrategy() const
	{
		return _strategy;
	}

	bool hasElement()
	{
		return _elements.size() > 0;
	}

	GameObjectElement& getElement()
	{
		return _elements[0];
	}

	
	const Ogre::Vector3& getPosition() const
	{
		return _elements[0].position;
	}

	Ogre::Vector2 getPos2d() const
	{
		const Ogre::Vector3& pos = getPosition();
		return Ogre::Vector2(pos.x, pos.z);
	}

	Character(float radius = DEFAULT_CHARACTER_RADIUS, float height = 200, Ogre::Quaternion meshOrientation = Ogre::Quaternion::ZERO, float initialHealth = 100, Action* dyingAction = Action::PLAYER_DYING) 
		: _radius(radius),
		  _height(height),
		  _capsuleOffsetY(height*0.5f/PHYSICS2WORLD_SCALE + 0.01f),
		  _currentAnimState(NULL), 
		  _lookingAngle(0),
		  _elevationAngle(0),
		  _meshOrientation(meshOrientation),
		  _strategy(Strategy::NOTHING),
		  _currentTriangle(NULL),
		  _dead(false),
		  _health(initialHealth),
		  _dyingAction(dyingAction),
		  _actionsChanged(false)
	{
	}

	virtual void setActions(const std::vector<Action*>& actions)
	{
		_updatedActions = actions;
		_actionsChanged = true;
	}

	void setAction(Action* action)
	{
		std::vector<Action*> actions;
		actions.push_back(action);
		setActions(actions);
	}
	
	bool isDead() const { return _dead; }

	virtual void setDead(World* world) 
	{ 
		_dead = true;
	}
	
	void turn(const Ogre::Vector2& angles)
	{
		turn(angles.x, angles.y);
	}

	void turn(float deltaAzimuthal, float deltaElevation = 0)
	{
		_lookingAngle -= Radian(deltaAzimuthal);
		_elevationAngle -= Radian(deltaElevation);
	}

	void setDirection(const Ogre::Vector3& dir)
	{
		setDirection(Vector2(dir.x, dir.z));
	}

	void setDirection(const Ogre::Vector2& dir)
	{
		_lookingAngle = Ogre::Math::ATan2(-dir.y, dir.x);
	}

	float getLookingAngle() const
	{
		return _lookingAngle.valueRadians();
	}

	PxCapsuleController* getPhysController() const
	{
		return _physController;
	}

	Ogre::Vector3 getLookingDirection() const
	{
		return Ogre::Vector3(Math::Cos(_lookingAngle), 
			Math::Sin(_elevationAngle), 
			-Math::Sin(_lookingAngle));
		//return Ogre::Vector3(-Math::Sin(_lookingAngle), 0, -Math::Cos(_lookingAngle));
	}

	virtual Ogre::Vector3 getGunPosition() const
	{
		Ogre::Vector3 lookingDir = getLookingDirection();
		return getPosition()+Ogre::Vector3::UNIT_Y*_height*0.7+lookingDir*_radius*1.1f;
	}

	virtual float getGunDamage(float distance) const { return 50.0f; }

	virtual Ogre::Vector3 getHeadPosition() const
	{
		return getPosition()+Ogre::Vector3::UNIT_Y*_height*0.8;
	}

	virtual Entity* createEntity(World* world) { return NULL; }
	virtual Action* getInitialAction();
	virtual void getInitialPose(Ogre::Vector3& position, Radian& initialAngle) { }
	
	virtual void initCapsuleControllerMisc(PxCapsuleControllerDesc& desc, PhysicsManager* physicsMgr);

	virtual void initImpl(World* world);
	virtual void act(World* world, float totalTime, float dt);
	
	virtual void update(World* world, float totalTime, float dt);

	float getAnimLength() const
	{
		if (_currentAnimState != NULL)
		{
			return _currentAnimState->getLength();
		}
		else
		{
			return 0;
		}
	}

	float getAnimTime() const
	{
		if (_currentAnimState != NULL)
		{
			return _currentAnimState->getTimePosition();
		}
		else
		{
			return FLT_MAX;;
		}
	}

	virtual void shoot(World* world, float damage) 
	{ 
		if (isDead()) return;
		_health-=damage;
		if (_health <= 0)
		{
			_health = 0;
			setDead(world);
			setAction(_dyingAction);
		}
		//LOG(" ***********************  SHOT!");
	}

	void moveAnimToEnd()
	{
		if (_currentAnimState != NULL)
		{
			_currentAnimState->setTimePosition(_currentAnimState->getLength()*0.99f);
		}
	}

	void releasePhysicsController()
	{
		_physController->release();
		_physController = NULL;
	}
};