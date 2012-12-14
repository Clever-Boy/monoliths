#pragma once

#include "stdafx.h"
#include "World.h"

using namespace Ogre;
using namespace OIS;


class GameController : public OIS::KeyListener, public OIS::MouseListener, public FrameListener
{
private:
	Ogre::Root* _root;
	RenderWindow* _window;
	SceneManager* _sceneManager;
	InputManager* _inputManager;
	Keyboard* _keyboard;
	Mouse* _mouse;

	Camera* _camera;
	OgreBites::SdkCameraMan* _cameraMan;

	float _time;

	Light* _pointLight;
	Light* _dirLight0;

	Ogre::Vector3 _lightOrigo;

	RenderTexture* _sanyiRenderTarget;
	Rectangle2D* _miniScreen;
	CompositorInstance* _compie;
	
	World* _scene;

	void setupScene();
	void setupRenderTargets();


public:
	GameController()
		: _time(0), _pointLight(NULL)
	{
	}

	~GameController()
	{
		//delete _root;
	}

	void initInputSystem()
	{
		OIS::ParamList pl;
		size_t windowHnd = 0;
		_window->getCustomAttribute("WINDOW", &windowHnd);
		std::ostringstream windowHndStr;
		windowHndStr << windowHnd;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

		_inputManager = InputManager::createInputSystem(pl);

		_keyboard = static_cast<OIS::Keyboard*>(_inputManager->createInputObject( OIS::OISKeyboard, true ));
		_mouse = static_cast<OIS::Mouse*>(_inputManager->createInputObject( OIS::OISMouse, true ));

		_keyboard->setEventCallback(this);
		_mouse->setEventCallback(this);
	}

	void start()
	{
		String pluginsCfg =
#ifdef _DEBUG
			"plugins_d.cfg";
#else
			"plugins.cfg";
#endif
		_root = new Ogre::Root(pluginsCfg);
		
		RenderSystemList list = _root->getAvailableRenderers(); 
		_root->setRenderSystem(list.at(0));
		_root->initialise(false);

		_window = _root->createRenderWindow("|| MONOLITHS ||", 1024, 768, false);
		_sceneManager = _root->createSceneManager(0, "Default");
		ResourceGroupManager::getSingleton().addResourceLocation("media","FileSystem","General", false);
		ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

		initInputSystem();
		setupScene();
		setupRenderTargets();
		
		_root->addFrameListener(this);
		_root->startRendering();
	}


	virtual bool frameStarted(const FrameEvent& evt);

	virtual bool frameRenderingQueued(const FrameEvent& evt)
	{ 
		_cameraMan->frameRenderingQueued(evt);
		return true;
	}

	virtual bool keyPressed(const KeyEvent &evt)
	{
		_cameraMan->injectKeyDown(evt);		
		return true;
	}

	virtual bool keyReleased(const KeyEvent &evt)
	{
		_cameraMan->injectKeyUp(evt);
		return true;
	}

	virtual bool mouseMoved(const MouseEvent &evt )
	{
		_cameraMan->injectMouseMove(evt);
		return true;
	}

	virtual bool mousePressed(const MouseEvent &evt, MouseButtonID id )
	{
		_cameraMan->injectMouseDown(evt, id);
		return true;
	}

	virtual bool mouseReleased(const MouseEvent &evt, MouseButtonID id )
	{
		_cameraMan->injectMouseUp(evt, id);
		return true;
	}

	void addEntity(Entity* entity, String materialName, float scale = 1, float tx = 0, float ty = 0, float tz = 0)
	{
		entity->setMaterialName(materialName);
		addEntity((MovableObject*)entity, materialName, scale, tx, ty, tz);
	}

	void addEntity(MovableObject* entity, String materialName, float scale = 1, float tx = 0, float ty = 0, float tz = 0)
	{
		SceneNode* child = _sceneManager->getRootSceneNode()->createChildSceneNode();
		
		child->scale(scale, scale, scale);
		child->translate(tx,ty,tz);
		child->attachObject(entity);
	}

	void V(ManualObject* q, float x, float y, float z, Ogre::Vector3 n, float u, float v)
	{
		q->position(x,y,z); q->normal(n); q->textureCoord(u,v);
	}

	ManualObject* createCubeMesh(Ogre::String name, Ogre::String matName, float a=1, float b=1, float c=1) 
	{
		ManualObject* q = new ManualObject(name);
		
		q->begin(matName);
		

		Ogre::Vector3 frontNormal = -Ogre::Vector3::UNIT_Y; 
		Ogre::Vector3 backNormal = Ogre::Vector3::UNIT_Y;
		Ogre::Vector3 leftNormal = -Ogre::Vector3::UNIT_X;
		Ogre::Vector3 rightNormal = Ogre::Vector3::UNIT_X;
		Ogre::Vector3 topNormal = Ogre::Vector3::UNIT_Z;
		Ogre::Vector3 bottomNormal = -Ogre::Vector3::UNIT_Z;
		
		float u1 = a;
		float u2 = u1 + b;
		float u3 = u2 + u1;
        float u4 = u3 + b;
		float v1 = c;
		float v2 = b;

		V(q,0,0,0,frontNormal,0,v1); V(q,a,0,0,frontNormal,u1,v1); V(q,a,0,c,frontNormal,u1,0);V(q,0,0,c,frontNormal,0,0); // front
		V(q,a,0,0,rightNormal,u1,v1); V(q,a,b,0,rightNormal,u2,v1); V(q,a,b,c,rightNormal,u2,0); V(q,a,0,c,rightNormal,u1,0); // right
        V(q,a,b,0,backNormal,u2,v1); V(q,0,b,0,backNormal,u3,v1); V(q,0,b,c,backNormal,u3,0); V(q,a,b,c,backNormal,u2,0); // back
        V(q,0,b,0,leftNormal, u3,v1); V(q,0,0,0,leftNormal,u4,v1); V(q,0,0,c,leftNormal,u4,0); V(q,0,b,c,leftNormal,u3,0); // left
        V(q,0,0,c,topNormal,0,v2); V(q,a,0,c,topNormal,u1,v2); V(q,a,b,c,topNormal,u1,0); V(q,0,b,c,topNormal,0,0); // top
        V(q,0,b,0,bottomNormal,0,0); V(q,a,b,0,bottomNormal,u1,0); V(q,a,0,0,bottomNormal,u1,v2); V(q,0,0,0,bottomNormal,0,v2); // bottom

		q->triangle(0,1,2);      q->triangle(2,3,0);
		q->triangle(4,5,6);      q->triangle(6,7,4);
		q->triangle(8,9,10);      q->triangle(10,11,8);
		q->triangle(12,13,14);   q->triangle(14,15,12);
		q->triangle(16,17,18);   q->triangle(18,19,16);
		q->triangle(20,21,22);   q->triangle(22,23,20);
		
		q->end(); 

		return q;

	}
};