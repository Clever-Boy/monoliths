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

	
	virtual bool keyPressed(const KeyEvent &evt)
	{
		return true;
	}

	virtual bool keyReleased(const KeyEvent &evt)
	{
		return true;
	}

	virtual bool mouseMoved(const MouseEvent &evt )
	{
		return true;
	}

	virtual bool mousePressed(const MouseEvent &evt, MouseButtonID id )
	{
		return true;
	}

	virtual bool mouseReleased(const MouseEvent &evt, MouseButtonID id )
	{
		return true;
	}
};