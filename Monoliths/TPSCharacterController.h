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
	Ogre::Vector2 _mouseMove;
	float _mouseHeight;
	int _frameCounter;

public:

	TPSCharacterController(World* world, Character* character, Action* idleAction, Action* walkAction)
		: _character(character),
		  _idleAction(idleAction),
		  _walkAction(walkAction),
		  _mouseMove(Ogre::Vector2::ZERO),
		  _mouseHeight(0),
		  _frameCounter(0)
	{
		_camera = world->getSceneManager()->createCamera(world->getNextId("puppie_camera"));
		setCameraFromCharacter();
	}

	void setCameraFromCharacter()
	{
		if (_character->hasElement())
		{
			Ogre::Vector3 lookDir = _character->getLookingDirection();
			Ogre::Vector3 pos = _character->getElement().position + Ogre::Vector3::UNIT_Y*100;
			_camera->setPosition(pos-lookDir*300+Ogre::Vector3::UNIT_Y*_mouseHeight);
			//_camera->setDirection(lookDir);
			_camera->lookAt(pos+lookDir*300);
		}
	}

	virtual Camera* getCamera() { return _camera; }

	virtual void onUpdating(const FrameEvent& evt, Game* game) 
	{ 
		float dt = evt.timeSinceLastFrame;
		_character->turn(-_mouseMove.x*dt*0.5);
		
		_mouseHeight+=_mouseMove.y*dt*70;

		_character->clearActions();
		//String zzz = StringConverter::toString(_frameCounter++)+String("\n");
		//OutputDebugString(zzz.c_str());
		if (game->getKeyboard()->isKeyDown(KC_W))
		{
			//zzz = "NNNnnnnyy "+zzz;
			//OutputDebugString(zzz.c_str());
			_character->addAction(_walkAction);
		}
		else
		{
			_character->addAction(_idleAction);
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

};