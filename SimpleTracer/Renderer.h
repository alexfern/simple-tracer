#pragma once

#include "LinearAlg.h"
#include "Ray.h"
#include "Intersection.h"
#include "Scene.h"


static constexpr int MAX_DEPTH = 10;


struct Renderer {
private:
	std::shared_ptr<Scene> scene;
	Vec3f ambient = { 0.0f, 0.0f, 0.0f };// { .1f, .1f, .1f };
public:
	Renderer(std::shared_ptr<Scene> scene) :
		scene(scene)
	{}

	Vec3f color(Ray& r, int depth = 0) const {
		Vec3f c = { 0, 0, 0 };
		if (depth >= MAX_DEPTH)
			return c;

		Intersection insect{};
		bool hit = scene->intersect(r, &insect);

		if (hit) {
			//c = (Vec3f(insect.n) + Vec3f{ 1, 1, 1 }) / 2;
			const Material& mat = *(insect.m);
			c += mat.light;
			float weight = 0;
			Ray scattered{ mat.getScatteredRay(insect, &weight) };
			Vec3f incoming = color(scattered, depth + 1);
			{
				float r = mat.color.x * incoming.x;
				float g = mat.color.y * incoming.y;
				float b = mat.color.z * incoming.z;
				float cost = dot(scattered.dir, insect.n);
				if (cost < 0)
					cost = 0;
				c += (Vec3f{ r, g, b } * cost);
			}
			float x = r.tMax;
			//if (x == 0)
				//std::cout << "NANI?";
			float y = x / (1 + x);
			//c = { y, y, y };
		} else { //Some ambient lighting from background
			c += ambient;
		}
		return c;
	}

	void derps() {
		/*float weight;
		Ray scattered = insect.m->getScatteredRay(&weight);
		Poi3f reflectionOrg = insect.p;
		Vec3f reflectionDir = r.dir - 2 * dot(r.dir, insect.n) * Vec3f(insect.n);
		Ray reflectionRay{ reflectionOrg, reflectionDir };

		Vec3f reflectionColor;
		if (false && intersect(reflectionRay, reflectionColor, depth + 1)) {
			float a = dot(r.dir, insect.n);
			color = a * color + (1 - a) * reflectionColor;
		}*/
	}
};