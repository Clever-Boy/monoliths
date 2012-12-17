#pragma once
#include "stdafx.h"
#include "GameObject.h"
#include "World.h"

void GameObject::init(World* world)
{
	_sceneManager = world->getSceneManager();
	createEntitiesImpl(_entities, world);
	_node = _sceneManager->getRootSceneNode()->createChildSceneNode();
	
	float objectId = world->getNexShaderObjectId();

	for (auto i = _entities.begin(); i != _entities.end(); i++)
	{
		SceneNode* child = _node->createChildSceneNode(world->getNextId());
		//(*i)->getSubEntity(0)->setCustomParameter(0, Ogre::Vector4(getCtrDetectionExponent()));

		for (int j = 0; j < (*i)->getNumSubEntities();j++)
		{
			(*i)->getSubEntity(j)->setCustomParameter(0, Ogre::Vector4(objectId));
		}
		child->attachObject(*i);
		(*i)->setCastShadows(true);
		
	}

	//Entity*e;
	
	_node->setPosition(_position);
	_node->setOrientation(_orientation);
}