#pragma once
#include "stdafx.h"
#include "GameObject.h"
#include "World.h"

class Ground : public GameObject
{
private:
	float _size, _textureSize, _segmentMultiplicator;

public:
	Ground(float size, float textureSize, float segmentMultiplicator = 0.1f)
	{
		_size = size;
		_textureSize = textureSize;
		_segmentMultiplicator = segmentMultiplicator;
	}
	

protected:
	
	virtual void initImpl(World* world)
	{
		float tile = _size / _textureSize;
		int segments = (int)(tile*_segmentMultiplicator);
		
		SceneManager* sceneManager = world->getSceneManager();

		Plane plane(Ogre::Vector3::UNIT_Y, 0);
		MeshPtr mp = MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			plane, _size, _size, segments, segments, true, 1, tile, tile, Ogre::Vector3::UNIT_Z);
		Entity* entity = sceneManager->createEntity(mp);
		entity->setMaterialName("Ground");

		PhysicsManager* physMgr = world->getPhysicsManager();

		PxRigidStatic* pxPlane = PxCreatePlane(*physMgr->getPhysics(), PxPlane(PxVec3(0,1,0), 0), *physMgr->getDefaultMaterial());
		
		// Ogre::Quaternion(Radian(0), Ogre::Vector3(0,0,1))
		addElement(Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY, entity, pxPlane);
	}

	virtual float getCtrDetectionExponent()
	{
		return 2.2;
	}
};
