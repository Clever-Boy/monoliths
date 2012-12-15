#include "stdafx.h"
#include "GameController.h"

using namespace physx;
using namespace Ogre;

class BoxObject
{
private:
	Entity* _entity;
	SceneNode *_node;
	PxRigidDynamic* _actor;
	PxPhysics* _physics;

	Ogre::Vector3 _initialPosition;

public:

	BoxObject(float x, float y, float z, GameController* controller)
	{
		_initialPosition = Ogre::Vector3(x,y,z);
		SceneManager* mgr = controller->getSceneManager();
		_entity = mgr->createEntity(controller->generateId(), "doboz.mesh");
		_node = controller->addEntity(_entity, "lambert2");
		_node->scale(100,100,100);
		//_node->setDebugDisplayEnabled(true);

		_node->setPosition(_initialPosition);
		_physics= controller->getPhisics();
		PxMaterial* matcsi = _physics->createMaterial(0.5, 0.5, 0.5);
		_actor = PxCreateDynamic(*_physics, PxTransform(PxVec3(x,y,z)), PxBoxGeometry(50,50,50), *matcsi, 10 /*,PxTransform(PxVec3(50,50,50))*/);
		
		//PxRigidBodyExt::updateMassAndInertia(*_actor,
		
		controller->getPhisicsScene()->addActor(*_actor);
		//_actor->addForce(PxVec3(1000000,0,0));
		//_actor->setLinearVelocity(PxVec3(100,0,0));
	}

	void reset()
	{
		_node->setPosition(_initialPosition);
		_node->setOrientation(Quaternion::IDENTITY);
		PxTransform transform(PxVec3(_initialPosition.x, _initialPosition.y, _initialPosition.z));
		_actor->setGlobalPose(transform);
		//PxTransform transform = _actor->getGlobalPose();
	}

	void update()
	{
		PxTransform transform = _actor->getGlobalPose();
		PxQuat q = transform.q;
		PxVec3 p = transform.p;
		_node->setOrientation(q.w, q.x, q.y, q.z);
		_node->setPosition(p.x, p.y, p.z);
	}
};