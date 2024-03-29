#include "stdafx.h"
#include "common.h"
#include "World.h"
#include "Monolith.h"
#include "Ground.h"
#include "MonolithGenerator.h"
#include "BoxObject.h"
#include "Robot.h"
#include "Player.h"

#include "NavMeshDebugObject.h"
#include "FollowAndKillStrategy.h"

using namespace Ogre;

extern "C" {
	void LOG(String msg)
	{
		LogManager::getSingleton().getDefaultLog()->logMessage(msg);
		msg+="\n";
		OutputDebugString(msg.c_str());
	}
}

void World::createLights()
{
	_sceneManager->setAmbientLight(ColourValue(0.1f, 0.1f,0.1f));

	/*Light* light0 = _sceneManager->createLight("pointLight");
	light0->setType(Ogre::Light::LT_POINT);
	light0->setDiffuseColour(1, 1, 1);
	light0->setSpecularColour(0.5, 0.5, 0.5);
	light0->setAttenuation(100000, 0.003, 0.005, 0.0002);
	light0->setDebugDisplayEnabled(true);
	light0->setPosition( Ogre::Vector3(0, 150, 250));*/

	Light* light1 = _sceneManager->createLight("dirLight1");
	light1->setType(Light::LT_DIRECTIONAL);
	light1->setDirection(1.2, -0.4, 0.8);
	light1->setDiffuseColour(1, 1, 1);
	light1->setSpecularColour(0.3, 0.3, 0.3);
	light1->setCastShadows(false);
	//light1->setShadowFarDistance(1000);

	Light* light2 = _sceneManager->createLight("dirLight2");
	light2->setType(Light::LT_DIRECTIONAL);
	light2->setDirection(-1, -0.4, -0.3);
	light2->setDiffuseColour(0.3, 0.3, 0.3);
	light2->setSpecularColour(0.1, 0.1, 0.1);
	light2->setCastShadows(false);

	
	//_sceneManager->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
	//_sceneManager->setShadowFarDistance(10000);
}

void World::showOnly(ElementType elementType)
{
	for (auto i=_gameObjects.begin();i!=_gameObjects.end();i++)
	{
		(*i)->showOnly(elementType);
	}
}

void World::createGameObjects()
{
	
	float avgDistance = 2000;

	int n = (_mapSize*_mapSize)/(avgDistance*avgDistance);


	MonolithGenerator gen(_mapSize, 200, 100, 1000, 300, 0.9, 1.5, 0.07, 150);

	for (int i = 0; i < n; i++)
	{
		Monolith* monolith = gen.createMonolith();

		if (monolith->checkPosition(_mapSize))
		{
			addGameObject(monolith);
		}
		else
		{
			delete monolith;
		}
	}
	
	_ground = new Ground(_mapSize, 500);
	addGameObject(_ground);

	_player = new Player(-500*Ogre::Vector3::UNIT_Z);
	addGameObject(_player);

	//addEnemyRobot(Ogre::Vector2(5000,5000));

	_puppie = new Robot();
#ifdef _DEBUG
	addGameObject(_puppie);
	addGameObject(new BoxObject(Ogre::Vector3(500, 50, 0)));
	addGameObject(new BoxObject(Ogre::Vector3(0, 50, 500)));
	addGameObject(new BoxObject(Ogre::Vector3(0, 50, 1000)));
#endif
}

void World::addEnemyRobot(const Ogre::Vector2& position, float angle)
{
	Ogre::Vector3 pos(position.x, 0, position.y);
	Robot* robot = new Robot(pos, angle);
	addGameObject(robot);
	robot->setStrategy(new FollowAndKillStrategy(_player, this));
}

void World::initNavMesh()
{
	for (auto i = _gameObjects.begin(); i != _gameObjects.end(); i++)
	{
		(*i)->collectNavmeshElements(&_navMesh);
	}
	_navMesh.init(Ogre::Vector2(_mapSize, _mapSize));
}

void World::initDebugObjects()
{
	addGameObject(new NavMeshDebugObject(&_navMesh));
}


void World::shoot(Character* shootingCharacter)
{
	Ogre::Vector3 position = shootingCharacter->getGunPosition();
	Ogre::Vector3 direction = shootingCharacter->getLookingDirection();

	Ogre::Ray ray(position, direction);

	Ogre::Vector3 impact;
	GameObject* obj = pickNearest(ray, impact);
	if (obj != NULL)
	{
		obj->shoot(this, shootingCharacter->getGunDamage(position.distance(impact)));
	}
}