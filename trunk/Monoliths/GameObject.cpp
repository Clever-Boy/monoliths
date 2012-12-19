#pragma once
#include "stdafx.h"
#include "GameObject.h"
#include "World.h"

const float GAMEOBJECT_PHYSICS_SCALE = 100;

void GameObjectElement::init(SceneNode* node, World* world)
{
	this->node = node;
	if (entity != NULL)
	{
		node->attachObject(entity);
	}
	node->setPosition(position);
	node->setOrientation(orientation);

	if (actor != NULL)
	{
		//PxQuat pxOrientation = PxQuat::createIdentity();
		PxQuat pxOrientation =  PxQuat(orientation.x, orientation.y, orientation.z, orientation.w);
		PxTransform transform = PxTransform(PxVec3(position.x, position.y, position.z)/GAMEOBJECT_PHYSICS_SCALE, pxOrientation);
		actor->setGlobalPose(transform);

		world->getPhysicsManager()->addActor(actor);
	}
}

void GameObjectElement::updateNode()
{
	if (actor != NULL)
	{
		PxTransform &transform = actor->getGlobalPose();
		PxQuat q = transform.q;
		PxVec3 p = transform.p * GAMEOBJECT_PHYSICS_SCALE;
		node->setOrientation(q.w, q.x, q.y, q.z);
		node->setPosition(p.x, p.y, p.z);
	}
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
			SceneNode* child = _node->createChildSceneNode(world->getNextId("object_"));
			child->setScale(scale);
			(*i).init(child, world);

			for (int j = 0; j < (*i).entity->getNumSubEntities();j++)
			{
				(*i).entity->getSubEntity(j)->setCustomParameter(0, Ogre::Vector4(objectId));
			}
		}
	}

	postInit();
}

void GameObject::update(World* world, float totalTime, float dt)
{
	for (auto i = _elements.begin(); i != _elements.end(); i++)
	{
		(*i).updateNode();
	}
}