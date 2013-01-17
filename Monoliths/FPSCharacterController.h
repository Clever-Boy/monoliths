#pragma once
#include "stdafx.h"
#include "Game.h"
#include "GameController.h"
#include "Action.h"
#include "Character.h"

using namespace Ogre;

class FPSCharacterController : public GameController
{
	Camera* _camera;
	Character* _character;
	Action* _idleAction;
	Action* _walkActions[8];
	Ogre::Vector2 _mouseMove;
	int _frameCounter;
	float _mouseSensitivity;
	float _lookingHeight;
	Action* _shootingAction;
	bool _shooting;

public:

	FPSCharacterController(World* world, Character* character, Action* idleAction, Action* const walkActions[8], Action* shootingAction,
		float lookingHeight = 120, float mouseSensitivity = 0.1)
		: _character(character),
		  _idleAction(idleAction),
		  _mouseMove(Ogre::Vector2::ZERO),
		  _lookingHeight(lookingHeight),
		  _mouseSensitivity(mouseSensitivity),
		  _frameCounter(0),
		  _walkActions(),
		  _shootingAction(shootingAction),
		  _shooting(false)
	{
		_camera = world->getSceneManager()->createCamera(world->getNextId("puppie_camera"));
		_camera->setNearClipDistance(10);

		for (int i = 0; i<8; i++)
		{
			_walkActions[i] = walkActions[i];
		}
		
		setCameraFromCharacter();
	}

	void setCameraFromCharacter()
	{
		if (_character->hasElement())
		{
			Ogre::Vector3 lookDir = _character->getLookingDirection();
			
			Ogre::Vector3 pos = _character->getElement().position + Ogre::Vector3::UNIT_Y*_lookingHeight;
			_camera->setPosition(pos);
			_camera->lookAt(pos+lookDir);
		}
	}

	virtual Camera* getCamera() { return _camera; }

	virtual void onUpdating(const FrameEvent& evt, Game* game) 
	{ 
		float dt = evt.timeSinceLastFrame;



		Ogre::Vector2 turnValue = _mouseMove * dt * _mouseSensitivity;
		_character->turn(turnValue);
		//_cameraAngle+=Radian(_mouseMove.y * dt * _mouseSensitivity);
		
		//_mouseHeight+=_mouseMove.y*dt*40;

		//_character->clearActions();
		
		std::vector<Action*> allDaActions;

		if (!_character->isDead())
		{
			int dirIdx = -1;
			OIS::Keyboard* k = game->getKeyboard();

			if (k->isKeyDown(KC_W))
			{
				if (k->isKeyDown(KC_A)) dirIdx = 1;
				else if (k->isKeyDown(KC_D)) dirIdx = 7;
				else dirIdx = 0;
			}
			else if (k->isKeyDown(KC_S))
			{
				if (k->isKeyDown(KC_A)) dirIdx = 3;
				else if (k->isKeyDown(KC_D)) dirIdx = 5;
				else dirIdx = 4;
			}
			else if (k->isKeyDown(KC_A))
			{
				dirIdx = 2;
			}
			else if (k->isKeyDown(KC_D))
			{
				dirIdx = 6;
			}
		
			if (dirIdx>=0)
			{
				allDaActions.push_back(_walkActions[dirIdx]);
			}
			else
			{
				allDaActions.push_back(_idleAction);
			}

			if (_shooting)
			{
				allDaActions.push_back(_shootingAction);
				_shooting = false;
			}

			_character->setActions(allDaActions);
		}

		setCameraFromCharacter();
		_mouseMove = Ogre::Vector2::ZERO;
	}

	virtual void onUpdated(const FrameEvent& evt, Game* game) 
	{
		
	}

	virtual bool keyPressed(const KeyEvent &evt)
	{
		//String sajt = String("PRESSED @ ")+StringConverter::toString(_frameCounter)+"\n";
		//OutputDebugString(sajt.c_str());
		return true;
	}
	
	virtual bool mouseMoved(const MouseEvent &evt )
	{
		_mouseMove = Ogre::Vector2(evt.state.X.rel, evt.state.Y.rel);
		return true;
	}

	virtual bool mousePressed(const MouseEvent &evt, MouseButtonID id )
	{
		if (id == OIS::MB_Left)
		{
			_shooting = true;
		}
		return true;
	}
};