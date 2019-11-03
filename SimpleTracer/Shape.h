#pragma once

#include "Transform.h"
#include "Ray.h"
#include "Intersection.h"

class Shape {
public:
	virtual bool intersect(const Ray& r, Intersection* insect = nullptr) const = 0;
	virtual ~Shape() {}
private:
};
