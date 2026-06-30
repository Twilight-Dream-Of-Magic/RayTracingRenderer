#include "Scene.hpp"

Scene::Scene() {}

Scene::~Scene() 
{
	for(SceneObject* scene_object : this->scene_objects)
		delete scene_object;
}

void Scene::SetCamera( const Camera& camera )
{
	this->camera = camera;
}

const Camera& Scene::GetCamera() const
{
	return this->camera;
}

SceneObject* Scene::CreateSceneObject( const Vector3DFloat& position, const Vector3DFloat& euler, float scale )
{
	SceneObject* scene_object = new SceneObject(position, euler, scale);
	this->scene_objects.push_back(scene_object);
	return scene_object;
}

SceneObject* Scene::Intersect( Ray ray, Intersection& intersect ) const
{
	SceneObject* hit_scene_object = nullptr;
	for(const auto scene_object : this->scene_objects)
	{
		if(scene_object->Intersect(ray, intersect))
		{
			ray.max_limited = intersect.distance;
			hit_scene_object = scene_object;
		}
	}

	return hit_scene_object;
}
