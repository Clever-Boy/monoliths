#include "stdafx.h"
#include "Character.h"

using namespace Ogre;

class Action
{
public:
	virtual bool tryGetAnimationName(Ogre::String& name) { return false; }
};