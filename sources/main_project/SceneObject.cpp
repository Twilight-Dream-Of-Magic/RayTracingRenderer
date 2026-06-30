#include "SceneObject.hpp"

SceneObject::SceneObject( const Vector3DFloat& position, const Vector3DFloat& euler, float scale )
{
	this->object_to_world = MakeWorldTransform(position, euler, scale);
	this->object_from_world = glm::inverse(this->object_to_world);
}

SceneObject::~SceneObject()
{
	for(const auto* primitive : this->geometry_set)
	{
		if(primitive != nullptr)
		{
			delete primitive;
		}
	}
}

bool SceneObject::Intersect( Ray ray, Intersection& intersect ) const
{
	bool ray_hit = false;
	for(const auto* primitive : this->geometry_set)
	{
		if(primitive != nullptr && primitive->Intersect(ray, intersect))
		{
			ray.max_limited = intersect.distance;
			ray_hit = true;
		}
	}

	return ray_hit;
}

void SceneObject::AddPrimitive( Primitive* primitive )
{
	if(primitive != nullptr && primitive->GetOwnerSceneObject() == this)
	{
		this->geometry_set.push_back(primitive);
	}
}

Matrix4D SceneObject::GetObjectToWorld() const
{
	return this->object_to_world;
}

Matrix4D SceneObject::GetObjectFromWorld() const
{
	return this->object_from_world;
}
