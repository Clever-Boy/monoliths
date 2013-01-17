#pragma once
#include "stdafx.h"
#include "common.h"
#include <random>
#include "World.h"
#include "Player.h"
#include "Robot.h"
#include "NavMesh.h"


class EnemyGenerator
{
	typedef std::uniform_real_distribution<float> UniformDistribution;

	UniformDistribution _positionDistribution;
	std::poisson_distribution<float> _delayDistribution;
	float _delayMultiplier;

	NavMesh* _navMesh;
	World* _world;
	Character* _player;
	std::default_random_engine _engine;
	
	
	float _timer;

	float _nextSpawn;

	int _counter;
	float _minDistanceSquared;
	float _maxDistanceSquared;
public:

	EnemyGenerator(World* world)
		: _world(world), _navMesh(&world->getNavMesh()), _player(world->getPlayer()),
		_engine(std::random_device()()),
		_positionDistribution(-world->getMapSize()*0.45f, world->getMapSize()*0.45f),
		_delayDistribution(10),
		_delayMultiplier(10),
		_timer(0),
		_counter(0),
	    _nextSpawn(0),
		_minDistanceSquared(12000*12000),
		_maxDistanceSquared(20000*20000)
	{
	}

	float getNextSpawnTime(float t)
	{
		if (t < 30) return 5;
		return 15*std::exp(-t*0.001);
	}

	void doWork(float t, float dt)
	{
		_timer+=dt;

		if (_timer > _nextSpawn)
		{
			Ogre::Vector2 pos;
			do
			{
				pos.x = _positionDistribution(_engine);
				pos.y = _positionDistribution(_engine);
			} 
			while(_player->getPos2d().squaredDistance(pos) > _minDistanceSquared && _navMesh->findTriangleOfPoint(pos) == NULL);
			_world->addEnemyRobot(pos);
			_timer = 0;
			_nextSpawn = getNextSpawnTime(t);

			LOG("SPAWN! "+StringConverter::toString(pos)+" next after "+StringConverter::toString(_nextSpawn)+" s");
		}
	}
};
