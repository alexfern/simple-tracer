#pragma once

#include "Intersection.h"
#include "LinearAlg.h"
#include "Ray.h"
#include "Shape.h"

struct Sphere : public Shape {
private:
	Poi3f center;
	float radius;
public:
	Sphere(const Poi3f& center, float radius) :
		center(center),
		radius(radius)
	{}

	bool intersect(const Ray& ray, Intersection* insect = nullptr) const {
		Vec3f oc = ray.org - center;
		float a = dot(ray.dir, ray.dir);
		float b = dot(oc, ray.dir);
		float c = dot(oc, oc) - radius * radius;
		float discriminant = b * b - a * c;
		if (discriminant < 0)
			return false; //No intersects

		float t = (-b - sqrt(discriminant)) / a; //First try - part of +- in quadratic equation: its the closer part
		if (t >= ray.tMax || t <= ray.tMin) {
			t = (-b + sqrt(discriminant)) / a; //Might be inside the circle or something weird, try other root
			if (t >= ray.tMax || t <= ray.tMin)
				return false; //Solutions aren't in valid range
		}

		if (insect != nullptr) { //Populate intersection data if possible
			insect->wo = -ray.dir; //Wo is back towards incoming direction
			Poi3f p = (insect->p = ray(t)); //Intersection point is t dist along ray
			Vec3f delta = p - center; //To account for sphere not being centered at origin
			insect->n = Norm3f{ normalize(delta) }; //Direction from center to point is normal direction			
			float theta = acos(delta.z / radius); //Theta in range [0, PI]
			float v = theta / PI;

			float phi = (PI / 2) - atan(delta.x / delta.y); //Phi in range [0, PI]
			if (delta.y < 0) //Extends range to [0, 2PI] by checking if sin(real phi) is negitive
				phi += PI;

			//cot^-1(x/y) might be undefined if 
			if (v == 0 || v == 1) { //at poles of sphere
				phi = 0;
			}
			else if (delta.y == 0) { // or y is otherwise zero
				phi = delta.x > 0 ? 0 : PI;
			}

			float u = phi / (2 * PI);

			insect->uv = { u, v };
			insect->dpdu = { delta.y * 2 * PI, delta.x * 2 * PI, 0 };
			insect->dpdv = { delta.z * cos(phi) * PI, delta.z * sin(phi) * PI, -radius * sin(theta) * PI };
		}
		ray.tMax = t;
		return true;
	}
};