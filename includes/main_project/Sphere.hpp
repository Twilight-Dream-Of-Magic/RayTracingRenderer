#pragma once
#include "Primitive.hpp"

class Sphere : public Primitive
{
public:
	Sphere(SceneObject* owner, float radius);
	virtual bool Intersect(Ray ray, Intersection& intersect) const override;

private:
	float size_radius;
};