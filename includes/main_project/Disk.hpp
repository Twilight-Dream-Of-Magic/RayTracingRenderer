#pragma once
#include "Primitive.hpp"

class Disk : public Primitive
{
public:
	Disk(SceneObject* owner, float radius);
	virtual bool Intersect(Ray ray, Intersection& intersect) const override;

private:
	float size_radius;
};