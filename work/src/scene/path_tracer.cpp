
// glm
#include <gtc/constants.hpp>
#include <gtc/random.hpp>

// std
#include <random>

// project
#include "scene.hpp"
#include "shape.hpp"
#include "light.hpp"
#include "material.hpp"
#include "path_tracer.hpp"

// CGRA_BEGIN_REMOVE
namespace {

	// create "some" local coordinate system transform "m" from the normal
	// coverts local to world space
	glm::mat3 coordinateSystem(const glm::vec3 &normal) {
		glm::vec3 tangent(1, 0, 0);
		if (glm::abs(normal.x) > 0.7331f) tangent = glm::vec3(0, 0, 1);
		tangent = glm::normalize(glm::cross(tangent, normal));
		glm::vec3 bitangent = glm::cross(tangent, normal);
		return glm::mat3(tangent, normal, bitangent);
	}


	glm::vec3 uniformSampleHemisphereNormal(const glm::vec3 &normal) {
		static std::minstd_rand rd{std::random_device()()};
		std::uniform_real_distribution<float> dist(0, 1);

		float r1 = dist(rd), r2 = dist(rd);
		// cos(theta) = r1 = y
		// cos^2(theta) + sin^2(theta) = 1 -> sin(theta) = srtf(1 - cos^2(theta))
		float phi = 2 * glm::pi<float>() * r1;
		float sinTheta = glm::sqrt(1 - r2 * r2);
		float x = glm::cos(phi) * sinTheta;
		float y = r2;
		float z = glm::sin(phi) * sinTheta;
		return coordinateSystem(normal) * glm::vec3(x, y, z);
	}

	glm::vec3 uniformSampleConeNormal(const glm::vec3 &normal, float halfangle) {
		static std::minstd_rand rd{std::random_device()()};
		std::uniform_real_distribution<float> dist1(0, 1);
		std::uniform_real_distribution<float> dist2(glm::cos(halfangle), 1);
		float r1 = dist1(rd);
		float r2 = dist2(rd);
		// cos(theta) = r1 = y
		// cos^2(theta) + sin^2(theta) = 1 -> sin(theta) = srtf(1 - cos^2(theta))
		float phi = 2 * glm::pi<float>() * r1;
		float sinTheta = glm::sqrt(1 - r2 * r2);
		float x = glm::cos(phi) * sinTheta;
		float y = r2;
		float z = glm::sin(phi) * sinTheta;
		return coordinateSystem(normal) * glm::vec3(x, y, z);
	}
}
// CGRA_END_REMOVE


glm::vec3 SimplePathTracer::sampleRay(const Ray &ray, int) {
	// intersect ray with the scene
	RayIntersection intersect = m_scene->intersect(ray);

	// if ray hit something
	if (intersect.m_valid) {

		// simple grey shape shading
		float f = glm::abs(glm::dot(-ray.direction, intersect.m_normal));
		glm::vec3 grey(0.5, 0.5, 0.5);
		return glm::mix(grey / 2.0f, grey, f);
	}

	// no intersection - return background color
	return { 0.3f, 0.3f, 0.4f };
}



glm::vec3 CorePathTracer::sampleRay(const Ray &ray, int) {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Implement a PathTracer that calculates the ambient, diffuse
	// and specular, for the given ray in the scene, using the 
	// Phong lighting model. Give special consideration to objects
	// that occluded from direct lighting (shadow rays). You do
	// not need to use the depth argument for this implementation.
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	// ...

	// CGRA_BEGIN_REMOVE
	// intersect ray with the scene
	RayIntersection intersect = m_scene->intersect(ray);

	// if ray hit something
	if (intersect.m_valid) {

		// initialize emmited and reflected light
		glm::vec3 ld(0), ls(0);
		
		for (std::shared_ptr<Light> l : m_scene->lights()) {
			// approximate indirect diffuse (ambient)
			ld += l->ambience() * intersect.m_material->diffuse();

			// find non-occluded lights (direct lighting)
			if (!l->isOccluded(m_scene, intersect.m_position)) {

				// get irradiance and incident direction
				glm::vec3 ei = l->irradiance(intersect.m_position);
				glm::vec3 incident = l->incidentDirection(intersect.m_position);

				// diffuse reflection (Li * cosTheta * diffuse / pi)
				float costheta = glm::max(glm::dot(-incident, intersect.m_normal), 0.f);
				ld += ei * costheta * intersect.m_material->diffuse();

				// specular reflection (Li * dot(half, norm) * spec)
				float spec_dot = glm::max(glm::dot(glm::reflect(ray.direction, intersect.m_normal), -incident), 0.f);
				float rdf = glm::pow(spec_dot, intersect.m_material->shininess());
				ls += ei * rdf * intersect.m_material->specular();
			}
		}


		// return the total radiance leaving the surface
		return ld + ls;
		//return ld / glm::pi<float>()
		//	+ ls * (intersect.m_material->shininess() + 1) / (2.f * glm::pi<float>());
	}
	// CGRA_END_REMOVE
	// no intersection - return background color
	return { 0.3f, 0.3f, 0.4f };
}



glm::vec3 CompletionPathTracer::sampleRay(const Ray &ray, int depth) {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Using the same requirements for the CorePathTracer add in 
	// a recursive element to calculate perfect specular reflection.
	// That is compute the reflection ray off your intersection and
	// sample a ray in that direction, using the result to additionally
	// light your object. To make this more realistic you may weight
	// the incoming light by the (1 - (1/shininess)).
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	// ...

	// CGRA_BEGIN_REMOVE

	// early out
	if (depth < 0) return glm::vec3(0);

	// intersect ray with the scene
	RayIntersection intersect = m_scene->intersect(ray);

	// if ray hit something
	if (intersect.m_valid) {

		// initialize emmited and reflected light
		glm::vec3 ld(0), ls(0), lr(0);

		for (std::shared_ptr<Light> l : m_scene->lights()) {
			// approximate indirect diffuse (ambient)
			ld += l->ambience() * intersect.m_material->diffuse();

			// find non-occluded lights (direct lighting)
			if (!l->isOccluded(m_scene, intersect.m_position)) {

				// get irradiance and incident direction
				glm::vec3 ei = l->irradiance(intersect.m_position);
				glm::vec3 incident = l->incidentDirection(intersect.m_position);

				// diffuse reflection (Li * cosTheta * diffuse / pi)
				float costheta = glm::max(glm::dot(-incident, intersect.m_normal), 0.f);
				ld += ei * costheta * intersect.m_material->diffuse();

				// specular reflection (Li * dot(half, norm) * spec)
				float spec_dot = glm::max(glm::dot(glm::reflect(ray.direction, intersect.m_normal), -incident), 0.f);
				float rdf = glm::pow(spec_dot, intersect.m_material->shininess());
				ls += ei * rdf * intersect.m_material->specular();
			}
		}


		// recurse only if we have depth left
		if (depth > 0) {
			// perfect specular lighting
			float roughness = glm::clamp(1 / intersect.m_material->shininess(), 0.f, 1.f);
			if (roughness < 1) {

				// create a reflected ray
				Ray reflect_ray(intersect.m_position, glm::reflect(ray.direction, intersect.m_normal));

				// weight incoming reflection by the roughness
				lr += sampleRay(reflect_ray, depth - 1) * (1 - roughness) * intersect.m_material->specular();
			}
		}
		

		// return the total radiance leaving the surface
		return ld + ls + lr;
		//return ld / glm::pi<float>()
		//	+ ls * (intersect.m_material->shininess() + 1) / (2.f * glm::pi<float>())
		//	+ lr;
	}
	// CGRA_END_REMOVE
	// no intersection - return background color
	return { 0.3f, 0.3f, 0.4f };
}



glm::vec3 ChallengePathTracer::sampleRay(const Ray &ray, int depth) {
	//-------------------------------------------------------------
	// [Assignment 4] :
	// Implement a PathTracer that calculates the diffuse and 
	// specular, for the given ray in the scene, using the 
	// Phong lighting model. Give special consideration to objects
	// that occluded from direct lighting (shadow rays).
	// Implement support for textured materials (using a texture
	// for the diffuse portion of the material).
	//
	// EXTRA FOR EXPERTS :
	// Additionally implement indirect diffuse and specular instead
	// of using the ambient lighting term.
	// The diffuse is sampled from the surface hemisphere and the
	// specular is sampled from a cone of the phong lobe (which
	// gives a glossy look). For best results you need to normalize
	// the lighting (see http://www.thetenthplanet.de/archives/255)
	//-------------------------------------------------------------

	// YOUR CODE GOES HERE
	// ...

	// CGRA_BEGIN_REMOVE

	// early out
	if (depth < 0) return glm::vec3(0);

	// intersect ray with the scene
	RayIntersection intersect = m_scene->intersect(ray);

	// if ray hit something
	if (intersect.m_valid) {

		// initialize emmited and reflected light
		glm::vec3 ld(0), ls(0), lr(0);

		for (std::shared_ptr<Light> l : m_scene->lights()) {
			// find non-occluded lights (direct lighting)
			if (!l->isOccluded(m_scene, intersect.m_position)) {

				// get irradiance and incident direction
				const glm::vec3 incident = l->incidentDirection(intersect.m_position);
				const float costheta = glm::max(glm::dot(-incident, intersect.m_normal), 0.f);
				const glm::vec3 ei = l->irradiance(intersect.m_position) * costheta;

				// diffuse reflection: Ei * diffuse / pi
				ld += ei * intersect.m_material->diffuse(intersect);

				// specular reflection: Ei * dot(R, V) * spec * (shiny+2)/2pi
				const float spec_dot = glm::max(glm::dot(glm::reflect(ray.direction, intersect.m_normal), -incident), 0.f);
				ls += ei * glm::pow(spec_dot, intersect.m_material->shininess()) * intersect.m_material->specular();
			}
		}


		// recurse only if we have depth left
		if (depth > 0) {
			// diffuse reflection (indirect lighting) (no recurse?)
			{
				Ray diffuse_ray(intersect.m_position, uniformSampleHemisphereNormal(intersect.m_normal));
				const float costheta = glm::max(glm::dot(diffuse_ray.direction, intersect.m_normal), 0.f);
				const float pdf = 1.f / (glm::pi<float>() * 2.f);
				const glm::vec3 ei = sampleRay(diffuse_ray, 0) * costheta / pdf;
				ld += ei * intersect.m_material->diffuse(intersect);
			}

			// glossy specular reflection
			float roughness = glm::clamp(1 / intersect.m_material->shininess(), 0.f, 1.f);
			if (roughness < 1) {

				const glm::vec3 rdir = glm::reflect(ray.direction, intersect.m_normal);
				//const glm::vec3 sample_dir = uniformSampleHemisphereNormal(intersect.m_normal);
				const float cone_halfangle = glm::acos(glm::pow(0.2f, roughness));
				const glm::vec3 sample_dir = uniformSampleConeNormal(rdir, cone_halfangle);
				
				const float costheta = glm::dot(sample_dir, intersect.m_normal);
				if (costheta > 0) {

					// create a reflected ray
					Ray reflect_ray(intersect.m_position, sample_dir);

					const float spec_dot = glm::max(glm::dot(rdir, sample_dir), 0.f);
					const float spec_pow = glm::pow(spec_dot, intersect.m_material->shininess());

					if (spec_pow > 0.0001f) {

						// monte carlo integration over incoming radiance
						// pdf must match generation of sample_dir
						//const float pdf = spec_pow * (intersect.m_material->shininess() + 1) / (2.f * glm::pi<float>());
						//const float pdf = 1.f / (glm::pi<float>() * 2.f);
						const float pdf = 1.f / (glm::pi<float>() * 2.f * (1.f - glm::cos(cone_halfangle)));
						const glm::vec3 ei = sampleRay(reflect_ray, depth - 1) * costheta / pdf;

						// specular reflection
						ls += ei * spec_pow * intersect.m_material->specular();

					}
				}
			}
		}


		// return the total radiance leaving the surface
		//return ld + ls + lr;
		return ld / glm::pi<float>()
			+ ls * (intersect.m_material->shininess() + 2) / (2.f * glm::pi<float>())
			+ lr;
	}
	// CGRA_END_REMOVE
	// no intersection - return background color
	return { 0.3f, 0.3f, 0.4f };
}