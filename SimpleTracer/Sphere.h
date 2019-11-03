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

	bool intersect(const Ray& r, Intersection* insect = nullptr) const {
		Vec3f oc = r.org - center;
		float a = dot(r.dir, r.dir);
		float b = dot(oc, r.dir);
		float c = dot(oc, oc) - radius * radius;
		float discriminant = b * b - a * c;
		if (discriminant > 0) {
			float t = (-b - sqrt(discriminant)) / a;
			if (r.tMax < r.BIG_ASS_NUMBER / 2 && !(t > r.tMin) && !(t < -r.tMin) ) {
				std::cout << "t:" << t << " tmax:" << r.tMax << "\n";
			}
			if (t < r.tMax && t > r.tMin) {
				Poi3f p = (insect->p = r(t)); //Intersection point is t dist along ray
				Vec3f delta = p - center; //To account for sphere not being centered at origin
				insect->n = Norm3f{ normalize(delta) }; //Direction from center to point is normal direction			
				float theta = acos(delta.z / radius); //Theta in range [-PI/2, PI/2]
				float phi = (PI / 2) - atan(delta.x / delta.y); //Phi in range [0, PI]
				if (delta.y < 0) //Extends range to [0, 2PI] by checking if sin(real phi) is negitive
					phi += PI;
				float u = phi / (2 * PI);
				float v = theta / PI;
				insect->uv = { u, v };
				insect->dpdu = { delta.y, delta.x, 0 };
				insect->dpdv = { delta.z * cos(phi), delta.z * sin(phi), -radius * cos(theta) };
				r.tMax = t;
				return true;
			}
			t = (-b + sqrt(discriminant)) / a;
			if (t < r.tMax && t > r.tMin) {
				
				Poi3f p = (insect->p = r(t)); //Intersection point is t dist along ray
				Vec3f delta = p - center; //To account for sphere not being centered at origin
				insect->n = Norm3f{ normalize(delta) }; //Direction from center to point is normal direction			
				float theta = acos(delta.z / radius); //Theta in range [-PI/2, PI/2]
				float phi = (PI / 2) - atan(delta.x / delta.y); //Phi in range [0, PI]
				if (delta.y < 0) //Extends range to [0, 2PI] by checking if sin(real phi) is negitive
					phi += PI;
				float u = phi / (2 * PI);
				float v = theta / PI;
				insect->uv = { u, v };
				insect->dpdu = { delta.y, delta.x, 0 };
				insect->dpdv = { delta.z * cos(phi), delta.z * sin(phi), -radius * cos(theta) };
				r.tMax = t;
				return true;
			}
		}
		return false;
		/*Vec3f oc = r.org - center;
		float a = dot(r.dir, r.dir);
		float b = 2.0f * dot(oc, r.dir);
		float c = dot(oc, oc) - radius * radius;
		float discriminant = b * b - 4 * a * c;*/
		if (discriminant < 0) //No intersections
			return false;

		float t = (-b - sqrt(discriminant)) / (2.0f * a); //Uses the - in +-, because it should be closer (unless we are in front of/inside object)
		if (t > r.tMax || t < r.tMin) //Intesection either to close or to far
			return false;
		r.tMax = t;

		if (insect != nullptr) {
			
			Poi3f p = (insect->p = r(t)); //Intersection point is t dist along ray
			Vec3f delta = p - center; //To account for sphere not being centered at origin
			insect->n = Norm3f{ normalize(delta) }; //Direction from center to point is normal direction			
			float theta = acos(delta.z / radius); //Theta in range [-PI/2, PI/2]
			float phi = (PI / 2) - atan(delta.x / delta.y); //Phi in range [0, PI]
			if (delta.y < 0) //Extends range to [0, 2PI] by checking if sin(real phi) is negitive
				phi += PI;
			float u = phi / (2 * PI);
			float v = theta / PI;
			insect->uv = { u, v };
			insect->dpdu = { delta.y, delta.x, 0 };
			insect->dpdv = { delta.z * cos(phi), delta.z * sin(phi), -radius * cos(theta) };
		}
		
		return true;
	}
};