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
	float scale;

	void init(SceneNode* node, World* world);

	void updateFromActor();
	void updateNode();
};



class GameObject
{
private:
	SceneManager* _sceneManager;
	SceneNode* _node;
	
	bool _dbgMarked;
	std::unordered_map<GameObjectElement*, String> _dbgElementMaterials;

protected:
	std::vector<GameObjectElement> _elements;

	GameObject() : _dbgMarked(false)
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

	void addElement(Ogre::Entity* entity = NULL, PxRigidActor* actor = NULL, ElementType type = ET_STANDARD, float scale = 1)
	{
		addElement(Ogre::Vector3::ZERO, Ogre::Quaternion::IDENTITY, entity, actor, type, scale);
	}

	void addElement(const Ogre::Vector3& position, const Ogre::Quaternion& orientation,
		Ogre::Entity* entity = NULL, PxRigidActor* actor = NULL, ElementType type = ET_STANDARD, float scale = 1)
	{
		GameObjectElement element;
		element.position = position;
		element.orientation = orientation;
		element.entity = entity;
		element.actor = actor;
		element.type = type;
		element.scale = scale;

		_elements.push_back(element);

		if (entity != NULL)
		{
			GameObjectElement& ellie = _elements.back();
			_dbgElementMaterials[&ellie] = entity->getSubEntity(0)->getMaterialName();
		}
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



	void setDebugMarked(bool marked)
	{
		_dbgMarked = marked;

		for (auto i = _elements.begin(); i != _elements.end(); i++)
		{
			String materialName = _dbgMarked ? "Marked" : _dbgElementMaterials[&(*i)];
			Entity* entity = (*i).entity;

			if (entity != NULL)
			{
				entity->setMaterialName(materialName);
			}
		}
	}
	//virtual void collectPolygons(ClipperLib::Polygons& polygons) { }
};