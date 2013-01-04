// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#undef UNICODE
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <Ogre.h>
#include <OIS.h>
//#include <Terrain/OgreTerrain.h>
//#include <Terrain/OgreTerrainGroup.h>
#include <SdkCameraMan.h>
#include <PxPhysicsAPI.h>

#include <OgreFontManager.h>
#include <OgreTextAreaOverlayElement.h>

#include "../Common/clipper/clipper.hpp"
#include "../Common/poly2tri/poly2tri.h"

