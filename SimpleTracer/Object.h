#pragma once

#include "Hittable.h"
#include "Shape.h"
#include "Material.h"

struct Object : public Hittable {
	Object(std::shared_ptr<Shape> shape, std::shared_ptr<Material> material) :
		Object(Transform(), shape, material)
	{}

	Object(Transform fromObject, std::shared_ptr<Shape> shape, std::shared_ptr<Material> material) :
		fromObject(fromObject),
		shape(shape),
		material(material)
	{}

	virtual bool intersect(const Ray& r, Intersection* insect = nullptr) const {
		Transform toObject = inv(fromObject);
		Ray r2 = toObject(r);
		if (r.tMax < r.BIG_ASS_NUMBER / 2 && !r.tMax > .01f) {
			std::cout << " tmax:" << r.tMax << "\n";
		}
		bool hit = shape->intersect(r2, insect);
		if (r.tMax < r.BIG_ASS_NUMBER / 2 && !r.tMax > .01f) {
			std::cout << " tmax:" << r.tMax << "\n";
		}
		if (hit) {
			if (r.tMax < r.BIG_ASS_NUMBER / 2 && !r.tMax > .01f) {
				std::cout << " tmax:" << r.tMax << "\n";
			}
			if (insect != nullptr) {
				*insect = fromObject(*insect);
				insect->m = &(*material);
			}
			r = fromObject(r2);
			if (r.tMax < r.BIG_ASS_NUMBER / 2 && !r.tMax > .01f) {
				std::cout << " tmax:" << r.tMax << "\n";
			}
		}
		if (r.tMax < r.BIG_ASS_NUMBER / 2 && !r.tMax > .01f) {
			std::cout << " tmax:" << r.tMax << "\n";
		}
		return hit;
	}
private:
	Transform fromObject;
	std::shared_ptr<Shape> shape;
	std::shared_ptr<Material> material;
};
