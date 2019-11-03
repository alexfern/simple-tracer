#pragma once

#include "LinearAlg.h"
#include "Object.h"
#include "Ray.h"
#include "Intersection.h"

struct Scene : public Hittable {
private:
	std::vector<std::shared_ptr<Object>> objects;
public:
	Scene(std::vector<std::shared_ptr<Object>> objs) :
		objects(objs)
	{}

	bool intersect(const Ray& r, Intersection* insect) const {
		bool hit = false;
		for (size_t n = 0; n < objects.size(); n++) {
			if (r.tMax < r.BIG_ASS_NUMBER / 2 && !r.tMax > .01f) {
				std::cout << " tmax:" << r.tMax << "\n";
			}
			bool h = objects[n]->intersect(r, insect);
			hit = hit ? true : h;
			if (r.tMax < r.BIG_ASS_NUMBER / 2 && !r.tMax > .01f) {
				std::cout << " tmax:" << r.tMax << "\n";
			}
		}

		return hit;
	}
};