#include "stdafx.h"
#include "Character.h"
#include "Action.h"

Action* Character::getInitialAction()
{
	return const_cast<Action*>(Action::DEFAULT);
}

void Character::addAction(Action* action)
{
	_actions.push_back(action);
	
	Entity* e = getElement().entity;
	if (e != NULL)
	{
		String animName;
		if (action->tryGetAnimationName(animName))
		{
			AnimationState* newAnimState = e->getAnimationState(animName);
			if (_currentAnimState != newAnimState)
			{
				if (_currentAnimState != NULL)
				{
					_currentAnimState->setEnabled(false);
				}
				_currentAnimState = newAnimState;
				_currentAnimState->setEnabled(true);
			}	
		}
	}
}

void Character::clearActions()
{
	_actions.clear();
}

void Character::act(World* world, float totalTime, float dt) 
{
	for (auto i = _actions.begin(); i != _actions.end(); i++)
	{
		(*i)->apply(this, world, totalTime, dt);
	}
}

void Character::initCapsuleControllerDesc(PxCapsuleControllerDesc& desc, PhysicsManager* physicsMgr)
{
	desc.radius = 0.3;
	desc.height = 2;
	desc.upDirection = PxVec3(0, 1, 0);

	desc.slopeLimit = 0.707;
	desc.stepOffset = 0.5;
		
	desc.position.set(0,0,0);
	desc.material = physicsMgr->getDefaultMaterial();
}

void Character::initImpl(World* world)
{
	Entity* entity = createEntity(world);
	Ogre::Vector3 position = Ogre::Vector3::ZERO;
	
	getInitialPose(position, _lookingAngle);

	Ogre::Quaternion orientation = Ogre::Quaternion(_lookingAngle, Ogre::Vector3::UNIT_Y);

	addElement(position, orientation, entity);

	PxCapsuleControllerDesc desc;
	desc.setToDefault();
	desc.callback = NULL;

	PhysicsManager* physMgr = world->getPhysicsManager();
	initCapsuleControllerDesc(desc, physMgr);
	
	_physController = physMgr->createCapsuleController(desc);

	PxExtendedVec3 pos = PxExtendedVec3(position.x, position.y, position.z);
	_physController->setPosition(pos);

	Action* initialAction = getInitialAction();
	if (initialAction != NULL)
	{
		addAction(initialAction);
	}
}

void Character::update(World* world, float totalTime, float dt)
{
	PxExtendedVec3 pos = _physController->getPosition();
	GameObjectElement& e= getElement();
	e.position = Ogre::Vector3(pos.x, pos.y, pos.z)*GAMEOBJECT_PHYSICS_SCALE;
	e.orientation = Ogre::Quaternion(_lookingAngle, Ogre::Vector3::UNIT_Y);
	e.updateNode();

	AnimationState* newAnim = NULL;
	for (auto i=_actions.begin(); i!=_actions.end(); i++)
	{
		String animName;
		if ((*i)->tryGetAnimationName(animName))
		{
			newAnim = e.entity->getAnimationState(animName);
		}
	}

	if (newAnim != NULL && _currentAnimState != newAnim)
	{
		_currentAnimState->setEnabled(false);
		_currentAnimState->setTimePosition(0);
		_currentAnimState = newAnim;
		
		_currentAnimState->setLoop(true);
		_currentAnimState->setEnabled(true);
	}

	if (newAnim == NULL && _currentAnimState != NULL &&
		_currentAnimState->getTimePosition() > _currentAnimState->getLength())
	{
		_currentAnimState->setEnabled(false);
		_currentAnimState->setTimePosition(0);
		_currentAnimState = NULL;
	}

	if (_currentAnimState != NULL)
	{
		_currentAnimState->addTime(dt);
	}
}