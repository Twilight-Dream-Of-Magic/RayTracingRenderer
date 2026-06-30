#include "Disk.hpp"
#include "SceneObject.hpp"

Disk::Disk( SceneObject* owner, float radius ) : Primitive(owner), size_radius(radius)
{
	
}

bool Disk::Intersect( Ray ray, Intersection& intersect ) const
{
	SceneObject* onwer_scene_object = this->GetOwnerSceneObject();

	//Ray (Local Space)
	Ray local_ray =  onwer_scene_object->GetObjectFromWorld() * ray;

	//Ray is parallel to the disk plane and do not intersect
	if(std::abs(local_ray.line.z) < std::numeric_limits<float>::epsilon())
		return false;
	
	//How many times larger should the grid be? (Disk plane z=0)
	float value = -local_ray.origin.z / local_ray.line.z;

	//Intersect position is in the local space ray range?
	if(value < local_ray.min_limited || value > local_ray.max_limited)
		return false; //No

	Vector3DFloat local_position = local_ray.origin + value * local_ray.line;

	//Is the local ray intersection position inside the disk?
	if(glm::dot(local_position, local_position) > this->size_radius * this->size_radius)
		return false; //No

	//Intersection position (World Space)
	intersect.position = Vector3DFloat(onwer_scene_object->GetObjectToWorld() * Vector4DFloat(local_position, 1.0f));
	//Disk normal (World Space)
	intersect.normal = glm::normalize(Vector3DFloat(onwer_scene_object->GetObjectToWorld() * Vector4DFloat(0,0,1,0)));
	intersect.distance = value;

	return true;
}
