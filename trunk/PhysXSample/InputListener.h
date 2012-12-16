#pragma once
#include "stdafx.h"

class InputListener : public OIS::MouseListener, public OIS::KeyListener,  public Ogre::FrameListener
{
public:
	virtual Ogre::Camera* getCamera() = 0;
};