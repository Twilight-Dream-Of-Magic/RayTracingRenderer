#pragma once
#include "Ray.hpp"

class Camera
{
public:
	Camera() = default;
	~Camera() = default;

	Camera
	(
		const Vector3DFloat& source_position,
		const Vector3DFloat& target_position,
		const Vector3DFloat& up_vector,
		float fov, float near, float far,
		uint32_t width, uint32_t height
	);

	void Initialize
	(
		const Vector3DFloat& source_point,
		const Vector3DFloat& target_point,
		const Vector3DFloat& up,
		float fov, float near, float far,
		uint32_t width, uint32_t height
	);

	Ray GetRay(float x, float y) const;

private:
	Vector3DFloat position;
	Matrix4D combined_matrix;
	Matrix4D inverse_combined_matrix;
};