#include "stdafx.h"
#include "Game.h"

extern "C" {
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
    {
        try 
		{
			Game().start();
        } 
		catch( Ogre::Exception& e ) 
		{

            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
        }

        return 0;
    }

}