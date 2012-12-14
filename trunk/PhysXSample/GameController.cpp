#include "stdafx.h"
#include "GameController.h"

using namespace Ogre;



void GameController::setupRenderTargets()
{
	
}

void GameController::setupScene()
{
	_camera =  _sceneManager->createCamera("PlayerCam");
	_camera->setPosition(Ogre::Vector3(0,150, -200 ));
	//_camera->lookAt(Ogre::Vector3(0,0,0));
	_camera->setDirection(Ogre::Vector3::UNIT_Z); 
	_camera->setNearClipDistance(5);
	_camera->setFarClipDistance(30000);
	_cameraMan = new OgreBites::SdkCameraMan(_camera);

	Viewport* viewport = _window->addViewport(_camera);
	viewport->setBackgroundColour(ColourValue(0.8f,0.8f,0.8f));
	_window->setActive(true);
	
	Plane plane(Ogre::Vector3::UNIT_Y, 0);
	MeshPtr mp = MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane, 1000, 1000, 1, 1, true, 1, 10, 10, Ogre::Vector3::UNIT_Z);
	Entity* entGround = _sceneManager->createEntity(mp);
	addEntity(entGround, "Rocky");

	Ogre::Entity* entNinja = _sceneManager->createEntity("Ninja", "ninja.mesh");
	addEntity(entNinja, "BuziNinnya");

}

bool GameController::frameStarted(const FrameEvent& evt)
{ 
	_mouse->capture();
	_keyboard->capture();
	

	_time += evt.timeSinceLastEvent;

	if (_pointLight != NULL)
	{
		float dz = sin(_time*20) * 100;
		_pointLight->setPosition(_lightOrigo + Ogre::Vector3(-dz*0.6+100, 0, dz-200));
	}
	

	if (_keyboard->isKeyDown(KC_ESCAPE))
	{
		return false;
	}

	return true; 
}

