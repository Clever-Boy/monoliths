#pragma once
#include "stdafx.h"
#include "common.h"
#include "Character.h"
#include "Action.h"
#include "Strategy.h"
#include "NavMesh.h"
#include "PathFinder.h"

class FollowAndKillStrategy : public Strategy
{
	Character* _victim;
	World* _world;
	NavMesh* _navMesh;
	TrianglePath* _currentPath;
	Ogre::Vector2 _nextCorner;
	float _squaredShootingDistance;

public:

	FollowAndKillStrategy(Character* victim, World* world, float shootingDistance = 7000)
		: _world(world),
		_navMesh(&world->getNavMesh()),
		_victim(victim), 
		_squaredShootingDistance(shootingDistance*shootingDistance)
	{
		
	}

	virtual bool isActive() const { return true; }

	bool canShootVictim(Character* character, Ogre::Vector3& shootDir)
	{
		Ogre::Vector3 gunPos = character->getGunPosition();
		shootDir = _victim->getHeadPosition()-gunPos;
		return _world->pickNearest(Ogre::Ray(gunPos, shootDir)) == _victim;
	}

	virtual void control(Character* character, World* world, float t, float dt)
	{
		if (_victim->isDead())
		{
			character->setAction(Action::ROBOT_IDLE);
			return;
		}

		Ogre::Vector2 dest = _victim->getPos2d();
		Ogre::Vector2 pos = character->getPos2d();
		//character->clearActions();
		Ogre::Vector3 shootDir;
		if ((pos-dest).squaredLength() < _squaredShootingDistance && canShootVictim(character, shootDir))
		{
			Ogre::Vector2 dir(shootDir.x, shootDir.z);
			character->setDirection(dir);
			character->setAction(Action::ROBOT_SHOOT);
		}
		else
		{
			_currentPath = NULL;
			if (character->currentNavMeshTriangle() != NULL &&  _victim->currentNavMeshTriangle() != NULL)
			{
				_currentPath = world->getPathFinder()->findPathBeetween(character->currentNavMeshTriangle(), _victim->currentNavMeshTriangle());
			}
			if (_currentPath != NULL)
			{
				_nextCorner = _currentPath->nextCorner(pos, dest);
				Ogre::Vector2 dir = _nextCorner-pos;
				character->setDirection(dir);
				character->setAction(Action::ROBOT_WALK);
			}
		}
	}
};