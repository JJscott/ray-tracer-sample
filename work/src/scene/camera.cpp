
#include <iostream>

// glm
#include <gtc/matrix_transform.hpp>

// project
#include "camera.hpp"
#include "opengl.hpp"

using namespace std;


void Camera::setPositionOrientation(const glm::vec3 &pos, float yaw, float pitch) {
	m_position = pos;
	m_yaw = yaw;
	m_pitch = pitch;

	// update rotation matrix (based on yaw and pitch)
	m_rotation = glm::rotate(glm::mat4(1), m_yaw, glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1), m_pitch, glm::vec3(1, 0, 0));
	// CGRA_BEGIN_REMOVE
	auto view = glm::inverse(m_rotation) * glm::translate(glm::mat4(1), -m_position);
	auto proj = glm::perspectiveFov<float>(m_fovy, m_image_size.x, m_image_size.y, 0.1f, 1000.f);
	m_inv_view_proj = glm::inverse(proj * view);
	// CGRA_END_REMOVE
}

Ray Camera::generateRay(const glm::vec2 &pixel) {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Generate a ray in the scene using the camera position,
	// rotation field of view on the y axis (fovy) and the image
	// size. The pixel is given in image coordinates [0, imagesize]
	// This COULD be done by first creating the ray in ViewSpace
	// then transforming it by the position and rotation to get
	// it into worldspace.
	//-------------------------------------------------------------
	
	// YOUR CODE GOES HERE
	// ...
	
	Ray ray;
	// CGRA_BEGIN_REMOVE
	glm::vec2 pos_p = 2.0f * (pixel/ m_image_size) - 1.0f; // NDC coord

	// near plane position
	glm::vec3 pos0_p = glm::vec3(pos_p, -1.0);
	glm::vec4 pos0_vh = m_inv_view_proj * glm::vec4(pos0_p, 1.0);
	glm::vec3 pos0_v = glm::vec3(pos0_vh) / pos0_vh.w;
	// not-far plane position
	glm::vec3 pos1_p = glm::vec3(pos_p, 0.0);
	glm::vec4 pos1_vh = m_inv_view_proj * glm::vec4(pos1_p, 1.0);
	glm::vec3 pos1_v = glm::vec3(pos1_vh) / pos1_vh.w;

	// positions transformed back to world coords
	ray.direction = normalize(pos1_v - pos0_v);
	ray.origin = pos0_v;
	// CGRA_END_REMOVE
	return ray;
}
