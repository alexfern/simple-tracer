#pragma once

#include "LinearAlg.h"

struct Ray {
	Poi3f org;
	Vec3f dir;

	static constexpr float BIG_ASS_NUMBER = 1000000000000000000.0f;

	Ray(Poi3f org, Vec3f dir) :
		org(org),
		dir(dir),
		tMax(BIG_ASS_NUMBER),
		tMin(std::numeric_limits<float>::epsilon() * 100)
	{}

	mutable float tMin;
	mutable float tMax;

	Poi3f operator()(float t) const {
		return org + t * dir;
	}

	const Ray& operator=(const Ray& rhs) const {
		tMin = rhs.tMin;
		tMax = rhs.tMax;
		return *this;
	}
};