#pragma once
#include "Primitive.hpp"

class Triangle : public Primitive
{
public:
	Triangle(SceneObject* owner, const Vector3DFloat& point1, const Vector3DFloat& point2, const Vector3DFloat& point3);
	virtual bool Intersect(Ray ray, Intersection& intersect) const override;
private:
	/*World Space*/
	std::array<Vector3DFloat, 3> vertices;
	Vector3DFloat normal;
};