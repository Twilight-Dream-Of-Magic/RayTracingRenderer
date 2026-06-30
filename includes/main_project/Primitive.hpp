#pragma once
#include "Ray.hpp"

class SceneObject;

class Primitive
{
public:
	Primitive(SceneObject* owner);
	virtual ~Primitive();

	SceneObject* GetOwnerSceneObject() const;

	virtual bool Intersect(Ray ray, Intersection& intersect) const = 0;

private:
	SceneObject* owner = nullptr;
};