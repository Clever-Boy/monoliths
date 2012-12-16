#include "stdafx.h"
#include "GameController.h"
#include "NinjaObject.h"

using namespace Ogre;
using namespace OIS;

static const float NC_LOOKAT_HEIGHT = 150;

class NinjaController : public InputListener
{
private:
	NinjaObject* _ninnya;
	GameController* _gameController;
	Camera* _camera;
	OgreBites::SdkCameraMan* _cameraMan;

	Ogre::Vector2 _mouseMove;
	float _mouseHeight;

public:

	NinjaController(NinjaObject* ninnya, GameController* gameController)
	{
		_ninnya = ninnya;
		_gameController = gameController;

		_camera = gameController->getSceneManager()->createCamera(gameController->generateId());
		_mouseHeight=0;
		updateCameraFromNinnya();
		_cameraMan = new OgreBites::SdkCameraMan(_camera);
		
		_mouseMove = Ogre::Vector2::ZERO;
	}

	void updateCameraFromNinnya()
	{
		Ogre::Vector3 dir = _ninnya->getLookingDirection();
		Ogre::Vector3 position = _ninnya->getPosition();
		Ogre::Vector3 lookatDv = Ogre::Vector3::UNIT_Y*NC_LOOKAT_HEIGHT;
		_camera->setPosition(position-dir*200+lookatDv+Ogre::Vector3::UNIT_Y*_mouseHeight);
		_camera->lookAt(position+lookatDv);
	}

	void update(float dt)
	{
		//ang+=Radian(_mouseMove.x*dt);
		//_ninnya->setOrientation(Quaternion(ang, axis));
		_ninnya->turn(-_mouseMove.x*dt*0.5);
		_ninnya->update(dt);
		_mouseHeight+=_mouseMove.y*dt*70;
		updateCameraFromNinnya();
		_mouseMove = Ogre::Vector2::ZERO;
	}
	
	virtual bool mouseMoved(const MouseEvent &evt )
	{
		/*Ogre::Vector2 mousepos = Ogre::Vector2((float)evt.state.X.abs/(float)evt.state.width, (float)evt.state.Y.abs/(float)evt.state.height);
		if (_lastMousepos.x == Math::POS_INFINITY)
		{
			_lastMousepos = mousepos;
			return true;
		}*/

		_mouseMove = Ogre::Vector2(evt.state.X.rel, evt.state.Y.rel);

		//_mouseMove = mousepos - _lastMousepos;
		
		//_cameraMan->injectMouseMove(evt);
		return true;
	}

	
	virtual Camera* getCamera()
	{
		return _camera;
	}

	virtual bool frameRenderingQueued(const FrameEvent& evt)
	{ 
		return true;
	}

	virtual bool frameStarted(const FrameEvent& evt)
	{
		return true;
	}
	
	virtual bool keyPressed(const KeyEvent &evt)
	{
		if (evt.key == OIS::KeyCode::KC_W)
		{
			_ninnya->setState(NS_WALK_FORWARD);
		}
		else if (evt.key == OIS::KeyCode::KC_S)
		{
			_ninnya->setState(NS_WALK_BACKWARDS);
		}
		return true;
	}

	virtual bool keyReleased(const KeyEvent &evt)
	{
		if (evt.key == OIS::KeyCode::KC_W || evt.key == OIS::KeyCode::KC_S)
		{
			_ninnya->setState(NS_IDLE);
		}
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