#pragma once

#include "Common.hpp"
#include <limits>

struct Ray
{
	//Start point
	Vector3DFloat source;

	//End point
	Vector3DFloat destination;

	Ray();
	~Ray() = default;

	float min_limited = 0.0f;
	float max_limited = std::numeric_limits<float>::max();
};

inline Ray operator*(const Matrix4D& transformation, const Ray& ray)
{
	Ray result;

	result.source = Vector4DFloat(transformation * Vector4DFloat(ray.source, 1.0f));
	result.destination = Vector4DFloat(transformation * Vector4DFloat(ray.destination, 0.0f));
	result.min_limited = ray.min_limited;
	result.max_limited = ray.max_limited;

	return result;
}