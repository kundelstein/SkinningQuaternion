#pragma once


#define GLM_FORCE_RADIANS
#define GLM_FORCE_PURE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>


// Computes the W component of the quaternion based on the X, Y, and Z components.
// This method assumes the quaternion is of unit length.

inline float quat_w (const glm::quat& quat)
{
	const float t = 1.0f - (quat.x * quat.x) - (quat.y * quat.y) - (quat.z * quat.z);
	if (t < 0.0f)
	{
		return 0.0f;
	}
	else
	{
		return -sqrtf (t);
	}
}

