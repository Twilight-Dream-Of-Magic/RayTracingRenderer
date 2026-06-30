#pragma once
#include "Ray.hpp"
#include "Primitive.hpp"

class SceneObject
{
public:
	SceneObject(const Vector3DFloat& position, const Vector3DFloat& euler, float scale);
	virtual ~SceneObject();

	bool Intersect(Ray ray, Intersection& intersect) const;
	void AddPrimitive(Primitive* primitive);

	template<typename PrimitiveType, typename...Args>
	PrimitiveType* CreatePrimitive(Args&&... args)
	{
		PrimitiveType* primitive = new PrimitiveType(this, std::forward<Args>(args)...);
		this->geometry_set.push_back(primitive);
		return primitive;
	}

	Matrix4D GetObjectToWorld() const;
	Matrix4D GetObjectFromWorld() const;

private:

	/* homogeneous coordinates */

	//Transformation matrix: Object local space to world global space
	Matrix4D object_to_world;

	//Transformation matrix: object local space from World global space
	Matrix4D object_from_world;

	std::vector<Primitive*> geometry_set;
};