#pragma once
#include "stdafx.h"
#include "common.h"
#include "World.h"

class Character;

class Strategy
{
public:
	virtual void control(Character* character, World* world, float t, float dt) { }
	virtual bool isActive() const { return true; }

	static Strategy* const NOTHING;
};

