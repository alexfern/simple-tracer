#pragma once

#include "Hittable.h"
#include <vector>

struct Aggregate : public Hittable {
private:
	std::vector<std::shared_ptr<Hittable>> hittables;
public:
	Aggregate(std::vector<std::shared_ptr<Hittable>> hittables) :
		hittables(std::move(hittables))
	{}

	virtual bool intersect(const Ray& r, Intersection* insect = nullptr) const {
		bool hit = false;
		for (std::shared_ptr<Hittable> h : hittables) {
			hit = hit || h->intersect(r, insect);
		}

		return hit;
	}
};