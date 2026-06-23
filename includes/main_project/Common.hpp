#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//std::number::pi
//#include <numbers>

using Vector2DFloat = glm::vec2;
using Vector3DFloat = glm::vec3;
using Vector4DFloat = glm::vec4;

using Vector2DInteger = glm::ivec2;
using Vector3DInteger = glm::ivec3;
using Vector4DInteger = glm::ivec4;

using Matrix3D = glm::mat3x3;
using Matrix4D = glm::mat4x4;

const float PI = glm::pi<float>();

// rows-major
// |1 0 0 x|
// |0 1 0 y|
// |0 0 1 z|
// |0 0 0 1|
inline Matrix4D MakeTranslationMatrix(const Vector3DFloat& translation)
{
	//Construct from columns-major, is not math columns
	Matrix4D matrix
	(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		translation.x, translation.y, translation.z, 1.0f
	);

	return matrix;
}

inline Matrix4D MakeRotationMatrix(const Vector3DFloat& euler)
{
	float cosx = cos(euler.x);
	float sinx = sin(euler.x);
	float cosy = cos(euler.y);
	float siny = sin(euler.y);
	float cosz = cos(euler.z);
	float sinz = sin(euler.z);

	Matrix4D rotation_x
	(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cosx, sinx, 0.0f,
		0.0f, -sinx, cosx, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	Matrix4D rotation_y
	(
		cosy, 0.0f, -siny, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		siny, 0.0f, cosy, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	Matrix4D rotation_z
	(
		cosz, sinz, 0.0f, 0.0f,
		-sinz, cosz, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	return rotation_z * rotation_y * rotation_x;
}

inline Matrix4D MakeScaleMatrix(const float scale) 
{
	//Construct from columns-major, is not math columns
	Matrix4D matrix
	(
		scale, 0.0f, 0.0f, 0.0f,
		0.0f, scale, 0.0f, 0.0f,
		0.0f, 0.0f, scale, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	return matrix;
}

inline Matrix3D MakeWorldToLocalCoordinateSystem(const Vector3DFloat& right_w) 
{
	Vector3DFloat up(1.0f, 0.0f, 0.0f);

	if(glm::abs(glm::dot(right_w, up) > 0.99f))
	{
		up = Vector3DFloat(0.0f, 1.0f, 0.0f);
	}

	Vector3DFloat forward_v = glm::cross(right_w, up);
	up = glm::cross(forward_v, right_w);

	up = glm::normalize(up);
	forward_v = glm::normalize(forward_v);

	//columns-major
	Matrix3D matrix(up, forward_v, right_w);

	return glm::transpose(matrix);
}

inline Matrix3D MakeLocalFromWorldCoordinateSystem(const Vector3DFloat& right_w) 
{
	Vector3DFloat up(1.0f, 0.0f, 0.0f);

	if(glm::abs(glm::dot(right_w, up) > 0.99f))
	{
		up = Vector3DFloat(0.0f, 1.0f, 0.0f);
	}

	Vector3DFloat forward_v = glm::cross(right_w, up);
	up = glm::cross(forward_v, right_w);

	up = glm::normalize(up);
	forward_v = glm::normalize(forward_v);

	//columns-major
	return Matrix3D(up, forward_v, right_w);
}

inline Matrix4D MakeWorldTransform
(
	const Vector3DFloat& position_point,
	const Vector3DFloat& rotation_euler,
	const float scale
)
{
	Matrix4D translation_matrix = MakeTranslationMatrix(position_point);
	Matrix4D rotation_matrix = MakeRotationMatrix(rotation_euler);
	Matrix4D scale_matrix = MakeScaleMatrix(scale);
	return translation_matrix * rotation_matrix * scale_matrix;
}