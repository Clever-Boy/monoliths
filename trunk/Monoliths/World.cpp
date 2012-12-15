#include "stdafx.h"
#include "World.h"
#include "Monolith.h"
#include "Ground.h"
#include "MonolithGenerator.h"

using namespace Ogre;

extern "C" {
	void KAKA(String msg)
	{
		LogManager::getSingleton().getDefaultLog()->logMessage(msg);
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

	Light* light2 = _sceneManager->createLight("dirLight2");
	light2->setType(Light::LT_DIRECTIONAL);
	light2->setDirection(-1, -0.4, -0.3);
	light2->setDiffuseColour(0.3, 0.3, 0.3);
	light2->setSpecularColour(0.1, 0.1, 0.1);
}

void World::createGameObjects()
{
	_ground = new Ground(_mapSize, 500);
	addGameObject(_ground);

	float avgDistance = 2000;

	int n = (_mapSize*_mapSize)/(avgDistance*avgDistance);


	MonolithGenerator gen(_mapSize, 200, 100, 1000, 300, 0.9, 1.5, 0.05, 150);

	for (int i = 0; i < n; i++)
	{
		Monolith* monolith = gen.createMonolith();
		addGameObject(monolith);
	}


	std::rand();
}

