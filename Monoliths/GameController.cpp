#include "stdafx.h"
#include "GameController.h"
#include "ControlObjectVisibilityListener.h"

using namespace Ogre;



void GameController::setupRenderTargets()
{
	_compie = CompositorManager::getSingleton().addCompositor(_window->getViewport(0), "Global");	
	_compie->setEnabled(true);
	RenderTarget* tata =_compie->getRenderTarget("toonshaded");
	
	//_compie->getRenderTarget("originalNoGround")->addListener(new ControlObjectVisibilityListener(_scene->getGround()));
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
	

	_scene = new World(_sceneManager, 40000);
	_scene->init();
	
	Ogre::Entity* entNinja = _sceneManager->createEntity("Ninja", "ninja.mesh");
	addEntity(entNinja, "BuziNinnya");

	Material* mat = (Material*) MaterialManager::getSingleton().getByName("Ground").get();
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

	if (_keyboard->isKeyDown(KC_SPACE))
	{
		_sanyiRenderTarget->writeContentsToFile("sanyi.png");
	}

	return true; 
}

