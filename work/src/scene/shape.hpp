
#pragma once

// glm
#include <glm.hpp>

// project
#include "ray.hpp"
#include "scene.hpp"


class Shape {
public:
	virtual RayIntersection intersect(const Ray &ray) = 0;
};


class AABB : public Shape {
private:
	glm::vec3 m_center;
	glm::vec3 m_halfsize;

public:
	AABB(const glm::vec3 &c, float hs) : m_center(c), m_halfsize(hs) { }
	AABB(const glm::vec3 &c, const glm::vec3 &hs) : m_center(c), m_halfsize(hs) { }
	virtual RayIntersection intersect(const Ray &ray) override;
};


class Sphere : public Shape {
private:
	glm::vec3 m_center;
	float m_radius;

public:
	Sphere(const glm::vec3 &c, float radius) : m_center(c), m_radius(radius) { }
	virtual RayIntersection intersect(const Ray &ray) override;
};

//-------------------------------------------------------------
// [Assignment 4] :
// Implement the following additional Shapes :
// - Plane
// - Disk
// - Triangle
// Follow the pattern shown by AABB and Sphere for implementing
// a class that subclasses Shape making sure that you implement
// the intersect method for each new Shape.
//-------------------------------------------------------------

// YOUR CODE GOES HERE
// ...

// CGRA_BEGIN_REMOVE
class Plane : public Shape {
private:
	glm::vec3 m_point; // a point that lies on a plane
	glm::vec3 m_normal; // the normal of the plane

public:	
	Plane(const glm::vec3 &p0, const glm::vec3 &n) : m_point(p0), m_normal(n) { }
	virtual RayIntersection intersect(const Ray &ray) override;
};


class Disk : public Shape {
private:
	glm::vec3 m_center; // a point that lies on a disk
	glm::vec3 m_normal; // the normal of the disk
	float m_radius;

public:
	Disk(const glm::vec3 &c, const glm::vec3 &n, float r) : m_center(c), m_normal(n), m_radius(r) { }
	virtual RayIntersection intersect(const Ray &ray) override;
};


class Triangle : public Shape {
private:
	glm::vec3 m_v0;
	glm::vec3 m_v1;
	glm::vec3 m_v2;

public:
	Triangle(const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2) : m_v0(v0), m_v1(v1), m_v2(v2) { }
	virtual RayIntersection intersect(const Ray &ray) override;
};
// CGRA_END_REMOVE