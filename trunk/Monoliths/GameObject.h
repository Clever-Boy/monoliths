#pragma once
#include "stdafx.h"
#include "common.h"
#include "NavMesh.h"

using namespace Ogre;
using namespace physx;

class World;

struct GameObjectElement
{
	Ogre::Vector3 position;
	Ogre::Quaternion orientation;
	Entity* entity;
	PxRigidActor* actor;
	SceneNode* node;
	ElementType type;

	void init(SceneNode* node, World* world);

	void updateFromActor();
	void updateNode();
};

extern const float PHYSICS2WORLD_SCALE;

class GameObject
{
private:
	SceneManager* _sceneManager;
	SceneNode* _node;
	
protected:
	std::vector<GameObjectElement> _elements;

	GameObject()
	{
	}

	virtual void initImpl(World* world) = 0;

	virtual float getCtrDetectionExponent()
	{
		return 1;
	}

	virtual float getScale() const 
	{ 
		return 1;
	}

protected:

	void addElement(Ogre::Entity* entity = NULL, PxRigidActor* actor = NULL, ElementType type = ET_STANDARD)
	{
		addElement(Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY, entity, actor, type);
	}

	void addElement(const Ogre::Vector3& position, const Ogre::Quaternion& orientation,
		Ogre::Entity* entity = NULL, PxRigidActor* actor = NULL, ElementType type = ET_STANDARD)
	{
		GameObjectElement element;
		element.position = position;
		element.orientation = orientation;
		element.entity = entity;
		element.actor = actor;
		element.type = type;

		_elements.push_back(element);
	}

	virtual void postInit() { }

public:

	SceneNode* getNode()
	{
		return _node;
	}

	void showOnly(ElementType type)
	{
		for (auto i = _elements.begin(); i != _elements.end(); i++)
		{
			i->node->setVisible((i->type & type) != 0);
		}
	}

	void init(World* world);
	
	virtual void act(World* world, float totalTime, float dt) { }
	virtual void update(World* world, float totalTime, float dt);

	virtual void collectNavmeshElements(NavMesh* navMesh) { }
	//virtual void collectPolygons(ClipperLib::Polygons& polygons) { }
};