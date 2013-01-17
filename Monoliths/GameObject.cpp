#pragma once
#include "stdafx.h"
#include "GameObject.h"
#include "World.h"

void GameObjectElement::init(SceneNode* node, World* world)
{
	this->node = node;
	if (entity != NULL)
	{
		node->attachObject(entity);
	}

	updateNode();
	
	if (actor != NULL)
	{
		PxQuat pxOrientation =  PxQuat(orientation.x, orientation.y, orientation.z, orientation.w);
		PxTransform transform = PxTransform(PxVec3(position.x, position.y, position.z)/PHYSICS2WORLD_SCALE, pxOrientation);
		actor->setGlobalPose(transform);

		world->getPhysicsManager()->addActor(actor);
	}
}

void GameObjectElement::updateFromActor()
{
	if (actor != NULL)
	{
		PxTransform t = actor->getGlobalPose();
		position = Ogre::Vector3(t.p.x, t.p.y, t.p.z) * PHYSICS2WORLD_SCALE;
		orientation = Ogre::Quaternion(t.q.w, t.q.x, t.q.y, t.q.z);
	}
}

void GameObjectElement::updateNode()
{
	node->setOrientation(orientation);
	node->setPosition(position);
}

void GameObject::init(World* world)
{
	_sceneManager = world->getSceneManager();
	initImpl(world);
	float s = getScale();
	Ogre::Vector3 scale = Ogre::Vector3(s,s,s);
	if (_elements.size() > 0)
	{
		_node = _sceneManager->getRootSceneNode()->createChildSceneNode();
		float objectId = world->getNexShaderObjectId();

		for (auto i = _elements.begin(); i != _elements.end(); i++)
		{
			#ifndef _DEBUG
			if (i->type != ET_STANDARD) continue;
			#endif

			SceneNode* child = _node->createChildSceneNode(world->getNextId("object_"));
			child->setScale(scale * i->scale);
			i->init(child, world);

			if (i->actor != NULL)
			{
				i->actor->userData = this;
			}

			if (i->entity != NULL)
			{
				for (int j = 0; j < i->entity->getNumSubEntities();j++)
				{
					i->entity->getSubEntity(j)->setCustomParameter(0, Ogre::Vector4(objectId));
				}
			}
		}
	}

	postInit();
}

void GameObject::update(World* world, float totalTime, float dt)
{
	for (auto i = _elements.begin(); i != _elements.end(); i++)
	{
		(*i).updateFromActor();
		(*i).updateNode();
	}
}