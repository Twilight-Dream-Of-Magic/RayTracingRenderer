#include "Camera.hpp"
#include <print>

Camera::Camera( const Vector3DFloat& source_position, const Vector3DFloat& target_position, const Vector3DFloat& up_vector, float fov, float near, float far, uint32_t width, uint32_t height ) 
{
	this->Initialize
	(
		source_position,
		target_position,
		up_vector,
		fov, near, far,
		width, height
	);
}

void Camera::Initialize
(
	const Vector3DFloat& source_position_input,
	const Vector3DFloat& target_position_input,
	const Vector3DFloat& up_vector_input,
	float fov_input, float near_input, float far_input,
	uint32_t width_input, uint32_t height_input
)
{
	/*Input Safety Layer*/

	Vector3DFloat source_position = GeometrySafetyUtilities::MakeFiniteVectorOrDefault
	(
		source_position_input,
		Vector3DFloat(0.0f, 0.0f, 0.0f)
	);

	Vector3DFloat target_position = GeometrySafetyUtilities::MakeFiniteVectorOrDefault
	(
		target_position_input,
		source_position + Vector3DFloat(0.0f, 0.0f, 1.0f)
	);

	target_position = GeometrySafetyUtilities::MakeSeparatedPointOrDefaultDirection
	(
		source_position,
		target_position,
		Vector3DFloat(0.0f, 0.0f, 1.0f)
	);

	Vector3DFloat forward_direction_for_safety = target_position - source_position;

	Vector3DFloat up_vector = GeometrySafetyUtilities::MakeFiniteNonZeroVectorOrDefault
	(
		up_vector_input,
		Vector3DFloat(0.0f, 1.0f, 0.0f)
	);

	up_vector = GeometrySafetyUtilities::MakeDirectionNotParallelTo
	(
		forward_direction_for_safety,
		up_vector
	);

	// FOV must be expressed in radians.
	// The renderer should convert degrees to radians before calling Camera::Initialize().
	float fov = GeometrySafetyUtilities::MakePositiveFloatOrDefault(fov_input, 1.0471975512f);
	if(fov >= 3.1415926535f)
	{
		fov = 1.0471975512f;
	}

	float near = GeometrySafetyUtilities::MakePositiveFloatOrDefault(near_input, 0.001f);
	float far = GeometrySafetyUtilities::MakePositiveFloatOrDefault(far_input, near + 1000.0f);

	if(far <= near)
	{
		far = near + 1000.0f;
	}

	uint32_t width = GeometrySafetyUtilities::MakePositiveSizeOrDefault(width_input, 1);
	uint32_t height = GeometrySafetyUtilities::MakePositiveSizeOrDefault(height_input, 1);

	bool is_zero_up_direction = (up_vector.x == 0) && (up_vector.y == 0) && (up_vector.z == 0);
	Vector3DFloat right;
	Vector3DFloat up;

	this->position = source_position;

	/*Viewing Matrix*/
	Matrix4D view;

	//Viewing Direction
	
	Vector3DFloat forward = glm::normalize(target_position - source_position);

	//Keep this manual math progress!!!
	if constexpr(false)
	{
		//Use left-hand
		if(is_zero_up_direction)
		{
			up = Vector3DFloat(0.0f, 1.0f, 0.0f);
			right = glm::normalize(glm::cross(up, forward));
		}
		else
		{
			right = glm::normalize(glm::cross(up_vector, forward));
		}

		Vector3DFloat up = glm::cross(forward, right);

		Matrix4D rotated_homogeneous_coordinates = Matrix4D
		(
			right.x, right.y, right.z, 0.0f,
			up.x, up.y, up.z, 0.0f,
			forward.x, forward.y, forward.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		view = rotated_homogeneous_coordinates * MakeTranslationMatrix(-source_position);
	}
	else
	{
		//Use left-hand
		view = glm::lookAtLH(source_position, target_position, up_vector);
	}

	/*Projection Matrix*/

	//Zero to One
	Matrix4D projection = glm::perspectiveFovLH_ZO(fov, static_cast<float>(width), static_cast<float>(height), near, far);

	Matrix4D viewport = Matrix4D
	(
		static_cast<float>(width) / 2.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -static_cast<float>(height) / 2.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		static_cast<float>(width) / 2.0f, static_cast<float>(height) / 2.0f, 0.0f, 1.0f
	);

	this->combined_matrix = viewport * projection * view;
	this->inverse_combined_matrix = glm::inverse(combined_matrix);
}

Ray Camera::GetRay( uint32_t x, uint32_t y )
{
	Ray ray;
	ray.source = this->position;
	
	//homogeneous coordinates: point
	Vector4DFloat point(x, y, 0.0f, 1.0f);
	Vector4DFloat world_position = this->inverse_combined_matrix * point;
	
	//world position = homogeneous coordinates / homogeneous coordinates(w) 
	world_position /= world_position.w;

	//from camera position pointer to direction vector of the world coordinates
	ray.destination = glm::normalize(Vector3DFloat(world_position) - this->position);

	return ray;
}
