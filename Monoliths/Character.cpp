#include "stdafx.h"
#include "Character.h"
#include "Action.h"

Action* Character::getInitialAction()
{
	return const_cast<Action*>(Action::DEFAULT);
}


void Character::setActionsImpl()
{
	std::vector<Action*> oldActions;
	std::vector<Action*> newActions;

	for (auto i = _updatedActions.begin(); i != _updatedActions.end(); i++)
	{
		Action* old = NULL;
		for (auto j = _actions.begin(); j != _actions.end(); j++)
		{
			if ((*i)->getActionType() == (*j)->getActionType())
			{
				old = *i;
				break;
			}
		}
		if (old == NULL) newActions.push_back(*i);
		else oldActions.push_back(old);
	}
	_actions.clear();

	for (auto i = oldActions.begin(); i != oldActions.end(); i++)
	{
		_actions.push_back(*i);
	}

	for (auto i = newActions.begin(); i != newActions.end(); i++)
	{
		Action* action = (*i)->cloneFromPrototype(this);
		_actions.push_back(action);
		actionAdded(action);
	}
}


void Character::actionAdded(Action* action)
{
	//_actions.push_back(action);
	
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
				_currentAnimState->setLoop(action->loopAnimation());
				_currentAnimState->setTimePosition(action->animationStart(_currentAnimState));
				_currentAnimState->setEnabled(true);
				_currentAnimSpeed = action->getAnimSpeed();
			}	
		}
	}
}

void Character::initCapsuleControllerMisc(PxCapsuleControllerDesc& desc, PhysicsManager* physicsMgr)
{
	desc.slopeLimit = 0.707;
	desc.stepOffset = 0.05;
}

void Character::initImpl(World* world)
{
	Entity* entity = createEntity(world);
	Ogre::Vector3 position = Ogre::Vector3::ZERO;
	
	getInitialPose(position, _lookingAngle);

	Ogre::Quaternion orientation = Ogre::Quaternion(_lookingAngle, Ogre::Vector3::UNIT_Y);

	addElement(position, orientation, entity);

	PhysicsManager* physMgr = world->getPhysicsManager();

	PxCapsuleControllerDesc desc;
	desc.setToDefault();

	desc.upDirection = PxVec3(0, 1, 0);
	desc.callback = NULL;
	desc.height = _height / PHYSICS2WORLD_SCALE;
	desc.radius = _radius / PHYSICS2WORLD_SCALE;
	desc.position.set(0,0,0);
	desc.material = physMgr->getDefaultMaterial();
	
	
	initCapsuleControllerMisc(desc, physMgr);
	
	_physController = physMgr->createCapsuleController(desc);
	_physController->getActor()->userData = this;

	position= position/PHYSICS2WORLD_SCALE + _capsuleOffsetY*Ogre::Vector3::UNIT_Y;

	PxExtendedVec3 pos = PxExtendedVec3(position.x, position.y, position.z);
	_physController->setPosition(pos);

	Action* initialAction = getInitialAction();
	if (initialAction != NULL)
	{
		setAction(initialAction);
	}
}

void Character::act(World* world, float totalTime, float dt) 
{
	if (_actionsChanged)
	{
		setActionsImpl();
		_actionsChanged = false;
	}

	if (!isDead())
	{
		NavMeshTriangle* tri = world->getNavMesh().findTriangleOfPoint(getPos2d());	
		if (tri != NULL)
		{
			_currentTriangle = tri;
		}
		//_currentTriangle = world->getNavMesh().findTriangleOfPoint(getPos2d());	
		_strategy->control(this, world, totalTime, dt);
	}

	for (auto i = _actions.begin(); i != _actions.end(); i++)
	{
		(*i)->apply(this, world, totalTime, dt);
	}
}

void Character::update(World* world, float totalTime, float dt)
{
	if (_physController != NULL)
	{
		PxExtendedVec3 pos = _physController->getPosition();
		GameObjectElement& e= getElement();
		e.position = Ogre::Vector3(pos.x, pos.y-_capsuleOffsetY, pos.z)*PHYSICS2WORLD_SCALE;
		e.orientation = Ogre::Quaternion(_lookingAngle, Ogre::Vector3::UNIT_Y);
		e.updateNode();
	}

	if (_currentAnimState != NULL)
	{
		float tp = _currentAnimState->getTimePosition();
		_currentAnimState->addTime(dt*_currentAnimSpeed);
	}
}