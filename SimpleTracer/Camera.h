#pragma once

#include <cmath>
#include <math.h>
#include "LinearAlg.h"
#include "Transform.h"
#include "Ray.h"
#include "Image.h"
#include "Renderer.h"
#include "Random.h"
#include <future>

struct ViewingFrustum {
	float aspectRatio;
	float verticalFov;

	struct {
		Poi3f topLeft;
		Vec3f down;
		Vec3f across;
		Poi3f origin;
	};

	ViewingFrustum(float aspectRatio, float verticalFov) :
		aspectRatio(aspectRatio),
		verticalFov(verticalFov),
		topLeft({ -aspectRatio * (float)tan(verticalFov / 2 * PI / 180), (float)tan(verticalFov / 2 * PI / 180), -1 }),
		down({ 0, -2 * (float)tan(verticalFov / 2 * PI / 180), 0 }),
		across({ 2 * aspectRatio * (float)tan(verticalFov / 2 * PI / 180), 0, 0 }),
		origin({ 0, 0, 0 })
	{}

	ViewingFrustum(Vec2i resolution, float verticalFov) :
		ViewingFrustum((float)resolution.x / resolution.y, verticalFov)
	{}

	Ray generateRay(Poi2f ndc) const {
		return { origin, (topLeft + ndc.y * down + ndc.x * across) - origin };
	}
};

struct Camera {
private:
	float verticalFov;
	Vec2i resolution;
	int aaNumSamples;
	Renderer renderer;
public:
	Camera(Vec2i resolution, float verticalFov, Renderer renderer, int aaNumSamples = 1) :
		resolution(resolution),
		verticalFov(verticalFov),
		renderer(renderer),
		aaNumSamples(aaNumSamples)
	{}

	/*
	Vec3f rasterToCameraSpace(const Vec2i& pixel) const {
		float aspectRatio = (float)resolution.x / resolution.y;
		float scale = tan(verticalFov / 2 * M_PI / 180);
		float x = (2 * ((pixel.x + 0.5)/ resolution.x) - 1) * scale * aspectRatio;
		float y = (1 - 2 * ((pixel.y + 0.5) / resolution.y)) * scale;
		return Vec3f({ x, y, -1 });
	}
	*/

	Image renderImage(const Transform& camToWorld) const {
		int width = resolution.x;
		int height = resolution.y;

		Image img = Image(width, height);
		ViewingFrustum f{ resolution, verticalFov };

		int numSamples = aaNumSamples;
		const Renderer& renderer = this->renderer;
		auto sampler = [width, height, &camToWorld, &f, &renderer](int x, int y) {
			Poi2f ndc{ (float)(x + randomD()) / width, (float)(y + randomD()) / height };
			if (91 == x && y == 20)
				std::cout << "h";
			Ray r = camToWorld(f.generateRay(ndc));
			Vec3f c = renderer.color(r);
			return c;
			
		};

		std::future<Vec3f>* samples = new std::future<Vec3f>[numSamples]();

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				Vec3f cAvg{0, 0, 0};
				
				/*for (int i = 0; i < numSamples; i++) {
					samples[i] = std::async(std::launch::async, sampler, x, y);
				}*/

				for (int i = 0; i < numSamples; i++) {
					cAvg += sampler(x, y);//samples[i].get();
				}
				cAvg /= (float)numSamples;
				cAvg = { std::min(cAvg.x, 1.0f), std::min(cAvg.y, 1.0f), std::min(cAvg.z, 1.0f) };

				img(x, y) = Vec3<uint8_t>(cAvg * 255);
			}
		}
		delete[] samples;

		return img;
	}

	

	
};

//struct CoordinateTransformer {
//	union {
//		Vec2i resolution;
//		struct { int width, height; };
//	};
//
//	float aspectRatio;
//	float fovScaleFactor;
//
//	CoordinateTransformer(Vec2i resolution, float verticalFov) :
//		resolution(resolution),
//		aspectRatio((float)width / height),
//		fovScaleFactor((float)tan(verticalFov / 2 * M_PI / 180)) //Is 1 when fov is 90 degrees
//	{}
//
//	inline Poi2f rasterToNdc(Poi2i xy) {
//		return { (xy.x + 0.5f) / width, (xy.y + 0.5f) / height };
//	}
//
//	inline Poi2f ndcToScreen(Poi2f ndc) {
//		return { (2 * ndc.x - 1) * aspectRatio, 1 - 2 * ndc.y }; //range is <(-w/h, +w/h), (-1, +1)>
//	}
//
//	inline Poi3f screenToCamera(Poi2f screen) {
//		return { screen.x * fovScaleFactor, screen.y * fovScaleFactor, -1 };
//	}
//};