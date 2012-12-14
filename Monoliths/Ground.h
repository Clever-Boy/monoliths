#pragma once
#include "stdafx.h"
#include "GameObject.h"

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
	
	virtual std::vector<MovableObject*> createEntitiesImpl(SceneManager* sceneManager)
	{
		float tile = _size / _textureSize;
		int segments = (int)(tile*_segmentMultiplicator);


		Plane plane(Ogre::Vector3::UNIT_Y, 0);
		MeshPtr mp = MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			plane, _size, _size, segments, segments, true, 1, tile, tile, Ogre::Vector3::UNIT_Z);
		Entity* entity = sceneManager->createEntity(mp);
		entity->setMaterialName("Ground");

		std::vector<MovableObject*> result;
		result.push_back(entity);
		return result;
	}
};
