
// std
#include <algorithm>
#include <utility>

// glm
#include <glm.hpp>
#include <gtc/constants.hpp>

// project
#include "shape.hpp"

// CGRA_BEGIN_REMOVE
// create "some" local coordinate system transform "m" from the normal
// coverts local to world space
// ie will convert (0, 1, 0) to the normal
glm::mat3 coordinateSystem(const glm::vec3 &normal) {
	glm::vec3 tangent(1, 0, 0);
	if (glm::abs(normal.x) > 0.7331f) tangent = glm::vec3(0, 0, 1);
	tangent = glm::normalize(glm::cross(tangent, normal));
	glm::vec3 bitangent = glm::cross(tangent, normal);
	return glm::mat3(tangent, normal, bitangent);
}
// CGRA_END_REMOVE

RayIntersection AABB::intersect(const Ray &ray) {
	RayIntersection intersect;
	glm::vec3 rel_origin = ray.origin - m_center;

	// start magic
	// x
	float rdx_inv = 1 / ray.direction.x;
	float tx1 = (-m_halfsize.x - rel_origin.x) * rdx_inv;
	float tx2 = (m_halfsize.x - rel_origin.x) * rdx_inv;

	float tmin = std::min(tx1, tx2);
	float tmax = std::max(tx1, tx2);

	// y
	float rdy_inv = 1 / ray.direction.y;
	float ty1 = (-m_halfsize.y - rel_origin.y) * rdy_inv;
	float ty2 = (m_halfsize.y - rel_origin.y) * rdy_inv;

	tmin = std::max(tmin, std::min(ty1, ty2));
	tmax = std::min(tmax, std::max(ty1, ty2));

	// z
	float rdz_inv = 1 / ray.direction.z;
	float tz1 = (-m_halfsize.z - rel_origin.z) * rdz_inv;
	float tz2 = (m_halfsize.z - rel_origin.z) * rdz_inv;

	tmin = std::max(tmin, std::min(tz1, tz2));
	tmax = std::min(tmax, std::max(tz1, tz2));

	if (tmax < tmin) return intersect;
	// end magic

	intersect.m_distance = tmin < 0 ? tmax : tmin;
	intersect.m_position = ray.origin + intersect.m_distance * ray.direction;
	intersect.m_valid = tmax >= 0;
	glm::vec3 work_out_a_name_for_it_later = glm::abs((intersect.m_position - m_center) / m_halfsize);
	float max_v = std::max(work_out_a_name_for_it_later[0], std::max(work_out_a_name_for_it_later[1], work_out_a_name_for_it_later[2]));
	intersect.m_normal = glm::normalize(glm::mix(intersect.m_position - m_center, glm::vec3(0), glm::lessThan(work_out_a_name_for_it_later, glm::vec3(max_v))));
	intersect.m_uv_coord = (glm::abs(intersect.m_normal.x) > 0) ?
		glm::vec2(intersect.m_position.y, intersect.m_position.z) :
		glm::vec2(intersect.m_position.x, intersect.m_position.y + intersect.m_position.z);
	intersect.m_shape = this;

	return intersect;
}


RayIntersection Sphere::intersect(const Ray &ray) {
	RayIntersection intersect;
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Implement the intersection method for Sphere that returns
	// a RayIntersection object with valid == false if the 
	// ray doesn't intersect and true otherwise. If true, then
	// remember to fill out each feild in the object correctly:
	// - m_valid : true if object is itersected
	// - m_distance : distance along the ray of the intersection
	// - m_position : position on the surface of the intersection
	// - m_normal : normal on the surface of the intersection
	// - m_shape : set to "this"
	// - m_uv_coord : texture coordinates (challenge only)
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	// ...

	// CGRA_BEGIN_REMOVE
	glm::vec3 rel_origin = ray.origin - m_center;
	
	float b = 2.0f * dot(ray.direction, rel_origin);
	float c = dot(rel_origin, rel_origin) - m_radius * m_radius;
	float disc = b * b - 4.0f * c;

	if (disc < 0) return intersect;

	float sqrt_disc = sqrt(disc);
	float t1 = (-b - sqrt_disc) * 0.5f;
	float t2 = (-b + sqrt_disc) * 0.5f;

	intersect.m_distance = t1 < 0 ? t2 : t1;
	intersect.m_position = ray.origin + intersect.m_distance * ray.direction;
	intersect.m_valid = t2 >= 0;
	intersect.m_normal = glm::normalize(intersect.m_position - m_center) * (t1 < 0 ? -1.f : 1.f);
	intersect.m_uv_coord = glm::vec2(glm::atan(intersect.m_normal.z, intersect.m_normal.x) / (2 * glm::pi<float>()) + 0.5, glm::acos(intersect.m_normal.y) / glm::pi<float>() + 0.5);
	intersect.m_shape = this;
	// CGRA_END_REMOVE
	return intersect;
}

// CGRA_BEGIN_REMOVE
RayIntersection Plane::intersect(const Ray &ray) {
	RayIntersection intersect;
	float denom = glm::dot(m_normal, ray.direction);
	if (std::abs(denom) < 1e-5) {
		return intersect;
	}

	glm::vec3 l = m_point - ray.origin;
	float t = glm::dot(l, m_normal) / denom;

	intersect.m_distance = t;
	intersect.m_position = ray.origin + intersect.m_distance * ray.direction;
	intersect.m_valid = t >= 0;
	intersect.m_normal = (denom < 0) ? m_normal : -m_normal;

	glm::vec3 u_v = coordinateSystem(intersect.m_normal) * intersect.m_position;
	intersect.m_uv_coord = glm::vec2(u_v.x, u_v.z);

	intersect.m_shape = this;

	return intersect;
}

RayIntersection Disk::intersect(const Ray &ray) {
	RayIntersection intersect;
	float denom = glm::dot(m_normal, ray.direction);
	if (glm::abs(denom) < 1e-5) {
		return intersect;
	}

	glm::vec3 l = m_center - ray.origin;
	float t = glm::dot(l, m_normal) / denom;

	glm::vec3 p = ray.origin + t * ray.direction;

	if (glm::dot(p - m_center, p - m_center) > m_radius * m_radius)
		return intersect;

	intersect.m_distance = t;
	intersect.m_position = p;
	intersect.m_valid = t >= 0;
	intersect.m_normal = (denom < 0) ? m_normal : -m_normal;

	glm::vec3 u_v = coordinateSystem(intersect.m_normal) * intersect.m_position;
	intersect.m_uv_coord = glm::vec2(u_v.x, u_v.z);

	intersect.m_shape = this;

	return intersect;
}


RayIntersection Triangle::intersect(const Ray &ray) {
	RayIntersection intersect;

	glm::vec3 v0v1 = m_v1 - m_v0;
	glm::vec3 v0v2 = m_v2 - m_v0;
	glm::vec3 pvec = glm::cross(ray.direction, v0v2);
	float det = glm::dot(v0v1, pvec);

	// ray and triangle are parallel if det is close to 0
	if (glm::abs(det) < 1e-5) return intersect;

	float invDet = 1 / det;

	glm::vec3 tvec = ray.origin - m_v0;
	float u = dot(tvec, pvec) * invDet;
	if (u < 0 || u > 1) return intersect;

	glm::vec3 qvec = glm::cross(tvec, v0v1);
	float v = glm::dot(ray.direction, qvec) * invDet;
	if (v < 0 || u + v > 1) return intersect;

	float t = dot(v0v2, qvec) * invDet;
	if (t < 1e-5) return intersect;

	intersect.m_distance = t;
	intersect.m_position = ray.origin + intersect.m_distance * ray.direction;
	intersect.m_valid = t >= 0;
	intersect.m_normal = glm::normalize(glm::cross(v0v1, v0v2)) * glm::sign(det);
	intersect.m_uv_coord = u * glm::vec2(0, 0) + v * glm::vec2(1, 0) + (1 - u - v) * glm::vec2(0, 1);
	intersect.m_shape = this;

	return intersect;
}
// CGRA_END_REMOVE