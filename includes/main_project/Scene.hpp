#pragma once
#include "SceneObject.hpp"
#include "Camera.hpp"

class Scene
{
public:
	Scene();
	~Scene();

	void SetCamera(const Camera& camera);
	const Camera& GetCamera() const;

	SceneObject* CreateSceneObject(const Vector3DFloat& position, const Vector3DFloat& euler, float scale);
	SceneObject* Intersect(Ray ray, Intersection& intersect) const;

private:
	Camera camera;
	std::vector<SceneObject*> scene_objects;
};