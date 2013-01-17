#pragma once
#include "stdafx.h"
#include "common.h"
#include "Character.h"
#include "Action.h"
#include "Strategy.h"
#include "NavMesh.h"
#include "PathFinder.h"

class MoveToPointStrategy : public Strategy
{
	Ogre::Vector2 _dest;
	NavMeshTriangle* _destTriangle;
	TrianglePath* _currentPath;
	Ogre::Vector2 _nextCorner;
	bool _isActive;

public:

	TrianglePath* currentPath() const
	{
		return _currentPath;
	}

	const Ogre::Vector2& nextCorner() const
	{
		return _nextCorner;
	}

	const Ogre::Vector2& destination() const
	{
		return _dest;
	}

	MoveToPointStrategy(const Ogre::Vector2& dest, NavMesh* navMesh)
		: _dest(dest), _isActive(true), _currentPath(NULL)
	{
		_destTriangle = navMesh->findTriangleOfPoint(dest);
	}

	virtual bool isActive() const { return _isActive; }

	virtual void control(Character* character, World* world, float t, float dt)
	{
		Ogre::Vector2 pos = character->getPos2d();
		//character->clearActions();
		if ((pos-_dest).squaredLength() < 10)
		{
			character->setStrategy(Strategy::NOTHING);
			//character->addAction(Action::ROBOT_IDLE);
			character->setAction(Action::ROBOT_IDLE);
			_isActive = false;
		}
		else
		{
			_currentPath = world->getPathFinder()->findPathBeetween(character->currentNavMeshTriangle(), _destTriangle);
			_nextCorner = _currentPath->nextCorner(pos, _dest);
			Ogre::Vector2 dir = _nextCorner-pos;
			character->setDirection(dir);
			//character->addAction(Action::ROBOT_WALK);
			character->setAction(Action::ROBOT_WALK);
		}
	}
};