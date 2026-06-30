#pragma once

#include "Common.hpp"
#include <limits>

struct Intersection
{
	Vector3DFloat position; // The exact 3D coordinate where the ray hits position the object surface.
	Vector3DFloat normal;   // The surface perpendicular direction vector at the hit point, used to compute lighting and reflection.
	float distance;         // The actual straight-line distance from the ray's start point to this intersection point.
};

struct Ray
{
	//Origin point
	Vector3DFloat origin;

	//Direction vector
	Vector3DFloat line;

	Ray();
	~Ray() = default;

	float min_limited = 0.0f;
	float max_limited = std::numeric_limits<float>::max();
};

inline Ray operator*(const Matrix4D& transformation, const Ray& ray)
{
	Ray result;

	result.origin = Vector4DFloat(transformation * Vector4DFloat(ray.origin, 1.0f));
	result.line = Vector4DFloat(transformation * Vector4DFloat(ray.line, 0.0f));
	result.min_limited = ray.min_limited;
	result.max_limited = ray.max_limited;

	return result;
}