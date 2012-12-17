#pragma once
#include "stdafx.h"

using namespace Ogre;
using namespace OIS;

class Game;

class GameController : public MouseListener, public KeyListener
{
public:
	virtual Ogre::Camera* getCamera() = 0;
	virtual void onUpdating(const FrameEvent& evt, Game* game) { };
	virtual void onUpdated(const FrameEvent& evt, Game* game) { };
};