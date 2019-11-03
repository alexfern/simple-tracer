#pragma once

#include "Ray.h"
#include "Intersection.h"


struct Hittable {
	virtual bool intersect(const Ray& r, Intersection* insect = nullptr) const = 0;
	virtual ~Hittable() {}
};