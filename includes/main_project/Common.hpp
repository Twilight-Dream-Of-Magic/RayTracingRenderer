#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/ext.hpp>

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

/*Geometry Safety Utilities*/

struct GeometrySafetyUtilities final
{
	GeometrySafetyUtilities() = delete;

	static bool IsInvalidFloat(float value)
	{
		return std::isnan(value) || std::isinf(value);
	}

	static bool IsInvalidVector3D(const Vector3DFloat& vector)
	{
		return
			IsInvalidFloat(vector.x) ||
			IsInvalidFloat(vector.y) ||
			IsInvalidFloat(vector.z);
	}

	static bool IsNearlyZeroVector
	(
		const Vector3DFloat& vector,
		float distance_tolerance = 0.000001f
	)
	{
		if(IsInvalidVector3D(vector))
		{
			return true;
		}

		float squared_length = glm::dot(vector, vector);
		float squared_distance_tolerance = distance_tolerance * distance_tolerance;

		return squared_length < squared_distance_tolerance;
	}

	static bool ArePointsNearlyOverlapping
	(
		const Vector3DFloat& first_position,
		const Vector3DFloat& second_position,
		float distance_tolerance = 0.000001f
	)
	{
		if(IsInvalidVector3D(first_position) || IsInvalidVector3D(second_position))
		{
			return true;
		}

		Vector3DFloat position_difference = second_position - first_position;

		float squared_distance = glm::dot(position_difference, position_difference);
		float squared_distance_tolerance = distance_tolerance * distance_tolerance;

		return squared_distance < squared_distance_tolerance;
	}

	static bool AreDirectionsNearlyParallel
	(
		const Vector3DFloat& first_direction,
		const Vector3DFloat& second_direction,
		float parallel_tolerance = 0.0001f
	)
	{
		if(IsNearlyZeroVector(first_direction) || IsNearlyZeroVector(second_direction))
		{
			return true;
		}

		Vector3DFloat normalized_first_direction = glm::normalize(first_direction);
		Vector3DFloat normalized_second_direction = glm::normalize(second_direction);

		float direction_dot = glm::dot(normalized_first_direction, normalized_second_direction);

		return std::abs(direction_dot) > 1.0f - parallel_tolerance;
	}

	static Vector3DFloat MakeFiniteVectorOrDefault
	(
		const Vector3DFloat& vector,
		const Vector3DFloat& default_vector
	)
	{
		if(IsInvalidVector3D(vector))
		{
			return default_vector;
		}

		return vector;
	}

	static Vector3DFloat MakeFiniteNonZeroVectorOrDefault
	(
		const Vector3DFloat& vector,
		const Vector3DFloat& default_vector,
		float distance_tolerance = 0.000001f
	)
	{
		if(IsNearlyZeroVector(vector, distance_tolerance))
		{
			return default_vector;
		}

		return vector;
	}

	static Vector3DFloat MakeSeparatedPointOrDefaultDirection
	(
		const Vector3DFloat& source_position,
		const Vector3DFloat& target_position,
		const Vector3DFloat& default_direction,
		float distance_tolerance = 0.000001f,
		float default_distance = 1.0f
	)
	{
		if(ArePointsNearlyOverlapping(source_position, target_position, distance_tolerance))
		{
			Vector3DFloat safe_default_direction = MakeFiniteNonZeroVectorOrDefault
			(
				default_direction,
				Vector3DFloat(0.0f, 0.0f, 1.0f),
				distance_tolerance
			);

			return source_position + glm::normalize(safe_default_direction) * default_distance;
		}

		return target_position;
	}

	static Vector3DFloat MakeDirectionNotParallelTo
	(
		const Vector3DFloat& reference_direction,
		const Vector3DFloat& direction,
		float parallel_tolerance = 0.0001f
	)
	{
		Vector3DFloat safe_reference_direction = MakeFiniteNonZeroVectorOrDefault
		(
			reference_direction,
			Vector3DFloat(0.0f, 0.0f, 1.0f)
		);

		if(IsNearlyZeroVector(direction) || AreDirectionsNearlyParallel(safe_reference_direction, direction, parallel_tolerance))
		{
			Vector3DFloat normalized_reference_direction = glm::normalize(safe_reference_direction);

			if(std::abs(normalized_reference_direction.y) < 0.9f)
			{
				return Vector3DFloat(0.0f, 1.0f, 0.0f);
			}
			else
			{
				return Vector3DFloat(1.0f, 0.0f, 0.0f);
			}
		}

		return direction;
	}

	static float MakeFiniteFloatOrDefault(float value, float default_value)
	{
		if(IsInvalidFloat(value))
		{
			return default_value;
		}

		return value;
	}

	static float MakePositiveFloatOrDefault(float value, float default_value)
	{
		if(IsInvalidFloat(value) || value <= 0.0f)
		{
			return default_value;
		}

		return value;
	}

	static uint32_t MakePositiveSizeOrDefault(uint32_t value, uint32_t default_value)
	{
		if(value == 0)
		{
			return default_value;
		}

		return value;
	}
};