#pragma once
#include "stdafx.h"
#include "GameObject.h"
#include "World.h"

void GameObject::init(World* world)
{
	_sceneManager = world->getSceneManager();
	_entities = createEntitiesImpl(_sceneManager);
	_node = _sceneManager->getRootSceneNode()->createChildSceneNode();
		
	for (int i = 0; i < _entities.size(); i++)
	{
		MovableObject* obj = _entities[i];
		SceneNode* child = _node->createChildSceneNode(world->getNextId());
		child->attachObject(obj);
	}
		
	_node->setPosition(_position);
	_node->setOrientation(_orientation);
}