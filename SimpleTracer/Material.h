#pragma once

#include "LinearAlg.h"
#include "Ray.h"
#include "Intersection.h"
#include "Random.h"
#include "Transform.h"
#include <cmath>

Vec3f UniformSampleHemisphere(const Poi2f& u) {
	float z = u[0];
	float r = std::sqrt(std::max((float)0, (float)1. - z * z));
	float phi = 2 * PI * u[1];
	return Vec3f(r * std::cos(phi), r * std::sin(phi), z);
}

float UniformHemispherePdf() {
	return 1 / (2 * PI);
}

Vec3f randomInUnitSphere() {
	Vec3f p;
	do {
		p = 2 * Vec3f(randomF(), randomF(), randomF()) - Vec3f{1, 1, 1};
	} while (p.lengthSq() > 1);
	return p;
}

struct Material {
	Vec3f color;
	Vec3f light;

	Material(Vec3f color) :
		Material(color, {0, 0, 0})
	{}

	Material(Vec3f color, Vec3f light) :
		color(color),
		light(light)
	{}

	Ray getScatteredRay(const Intersection& insect, float& weight) const {
		Poi3f p = insect.p;
		Norm3f n = insect.n;
		Vec3f s = normalize(insect.dpdu);
		Vec3f t = normalize(cross(s, n));
		Mat33f m{	
			s.x, s.y, s.z,
			t.x, t.y, t.z,
			n.x, n.y, n.z
		};
		float a = randomF();
		float b = randomF();
		float x = cos(2 * PI * b) * sqrt(1 - a * a);
		float y = sin(2 * PI * b) * sqrt(1 - a * a);
		float z = a;
		//weight = 1 / (2 * PI);
		weight = UniformHemispherePdf();
		return Ray{ p, Vec3f(n) + randomInUnitSphere() };
		return Ray{ p, m * UniformSampleHemisphere({a, b}) };
		return Ray{ p, m * Vec3f{x, y, z} };
	}

	

	~Material() {};

};