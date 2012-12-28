#pragma once
#include "stdafx.h"
#include "Game.h"
#include "GameController.h"
#include "Action.h"
#include "Character.h"


using namespace Ogre;

class TPSCharacterController : public GameController
{
	Camera* _camera;
	Character* _character;
	Action* _idleAction;
	Action* _walkAction;

public:

	TPSCharacterController(World* world, Character* character, Action* idleAction, Action* walkAction)
		: _character(character),
		  _idleAction(idleAction),
		  _walkAction(walkAction)
	{
		_camera = world->getSceneManager()->createCamera(world->getNextId("puppie_camera"));
		setCameraFromCharacter();
	}

	void setCameraFromCharacter()
	{
		Ogre::Vector3 lookDir = _character->getLookDirection();
		Ogre::Vector3 camPos = _character->getElement().position + Ogre::Vector3::UNIT_Y*100-lookDir*300;
		_camera->setPosition(camPos);
		_camera->setDirection(lookDir);
	}

	virtual Camera* getCamera() { return _camera; }

	virtual void onUpdating(const FrameEvent& evt, Game* game) 
	{ 
		_character->clearActions();
		if (game->getKeyboard()->isKeyDown(KC_W))
		{
			_character->addAction(_walkAction);
		}
		else
		{
			_character->addAction(_idleAction);
		}
		setCameraFromCharacter();
	};
};