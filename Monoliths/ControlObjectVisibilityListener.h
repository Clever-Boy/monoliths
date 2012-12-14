#pragma once
#include "stdafx.h"
#include "GameObject.h"
using namespace Ogre;

class ControlObjectVisibilityListener : public RenderTargetListener
{
private:
	std::vector<MovableObject*> _objects;
	//MovableObject* _object;

public:

	ControlObjectVisibilityListener(GameObject* gameObject)
	{
		_objects = gameObject->getEntities();
	}

	ControlObjectVisibilityListener(MovableObject* object)
	{
		_objects.push_back(object);
	}

	ControlObjectVisibilityListener(std::vector<MovableObject*> objects)
	{
		_objects = objects;
	}

protected:

	virtual void preRenderTargetUpdate(const RenderTargetEvent& evt)
	{
		for (int i = 0; i < _objects.size(); i++)
		{
			_objects[i]->setVisible(false);
		}
	}

	virtual void postRenderTargetUpdate(const RenderTargetEvent& evt)
	{
		for (int i = 0; i < _objects.size(); i++)
		{
			_objects[i]->setVisible(true);
		}
	}
};