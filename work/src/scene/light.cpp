
// glm
#include <gtc/constants.hpp>

// project
#include "light.hpp"


bool DirectionalLight::isOccluded(Scene *scene, const glm::vec3 &point) const {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Determine whether the given point is being occluded from
	// this directional light by an object in the scene.
	// Remember that directional lights are "infinitely" far away
	// so any object in the way would cause an occlusion.
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	// ...

	// CGRA_BEGIN_UNCOMMENT
	// return false;
	// CGRA_END_UNCOMMENT
	// CGRA_BEGIN_REMOVE
	Ray ray(point, -m_direction);
	RayIntersection intersect = scene->intersect(ray);
	return intersect.m_valid;
	// CGRA_END_REMOVE
}


glm::vec3 DirectionalLight::incidentDirection(const glm::vec3 &) const {
	return m_direction;
}


glm::vec3 DirectionalLight::irradiance(const glm::vec3 &) const {
	return m_irradiance;
}


bool PointLight::isOccluded(Scene *scene, const glm::vec3 &point) const {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Determine whether the given point is being occluded from
	// this directional light by an object in the scene.
	// Remember that point lights are somewhere in the scene and
	// an occulsion has to occur somewhere between the light and 
	// the given point.
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	// ...

	// CGRA_BEGIN_UNCOMMENT
	// return false;
	// CGRA_END_UNCOMMENT
	// CGRA_BEGIN_REMOVE
	glm::vec3 point_to_light = m_position - point;
	float l = glm::length(point_to_light);

	if (l > 0) {
		Ray ray(point, point_to_light / l);
		RayIntersection intersect = scene->intersect(ray);
		if (intersect.m_valid) {
			return intersect.m_distance < l;
		}
	}
	return false;
	// CGRA_END_REMOVE
}


glm::vec3 PointLight::incidentDirection(const glm::vec3 &point) const {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Return the direction of the incoming light (light to point)
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	// ...

	// CGRA_BEGIN_UNCOMMENT
	// return glm::vec3(0);
	// CGRA_END_UNCOMMENT
	// CGRA_BEGIN_REMOVE
	glm::vec3 light_to_point = point - m_position;
	float l = glm::length(light_to_point);
	if (l > 0)
		return light_to_point / l;
	return glm::vec3(0, 0, 0);
	// CGRA_END_REMOVE
}


glm::vec3 PointLight::irradiance(const glm::vec3 &point) const {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Return the total irradiance on the given point.
	// This requires you to convert the flux of the light into
	// irradiance by dividing it by the surface of the sphere
	// it illuminates. Remember that the surface area increases
	// as the sphere gets bigger, ie. the point is further away.
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	// ...

	// CGRA_BEGIN_UNCOMMENT
	// return glm::vec3(0);
	// CGRA_END_UNCOMMENT
	// CGRA_BEGIN_REMOVE
	glm::vec3 light_dir = m_position - point;
	float d = glm::length(light_dir);
	if (d > 0) {
		return m_flux / (4 * glm::pi<float>() * d * d);
	}
	return m_flux;
	// CGRA_END_REMOVE
}

