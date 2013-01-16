#pragma once
#include "stdafx.h"
//#define BASIC_GRAPHICS

const float PHYSICS2WORLD_SCALE = 100;

const float DEFAULT_CHARACTER_RADIUS = 50;
const float MAX_CHARACTER_HEIGHT = 200;

enum ElementType
{
	ET_NONE				= 0,
	ET_STANDARD			= 1 << 0,
	ET_MONOLITH_DBG		= 1 << 1,
	ET_PATH_DBG			= 1 << 2
};
