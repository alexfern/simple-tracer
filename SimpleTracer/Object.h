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
		bool hit = shape->intersect(r2, insect);
		if (hit) {
			if (insect != nullptr) {
				*insect = fromObject(*insect);
				insect->m = &(*material);
			}
			r = fromObject(r2);
		}
		return hit;
	}
private:
	Transform fromObject;
	std::shared_ptr<Shape> shape;
	std::shared_ptr<Material> material;
};
