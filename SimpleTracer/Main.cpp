#include <iostream>
#include <fstream>
#include "LinearAlg.h"
#include "Camera.h"
#include "Image.h"
#include "Sphere.h"
#include "Object.h"
#include "Material.h"
#include "Timer.h"
#include "Intersection.h"
#include "Scene.h"

int main() {
	Timer t;
	t.mark();

	std::cout << "Initilizing Scene: ";
	std::vector<std::shared_ptr<Object>> objects;
	std::shared_ptr<Material> matte = std::make_shared<Material>(Vec3f{ 0.8f, 0.8f, 0.8f });
	std::shared_ptr<Material> matte2 = std::make_shared<Material>(Vec3f{ 0.2f, 0.5f, 0.2f });
	std::shared_ptr<Material> light = std::make_shared<Material>(Vec3f{ 0.0f, 0.0f, 0.0f }, Vec3f{1.0f, 1.0f, 1.0f});
	std::shared_ptr<Material> light2 = std::make_shared<Material>(Vec3f{ 0.3f, 0.3f, 0.3f }, Vec3f{ 0.7f, 0.7f, 1.0f });

	

	objects.emplace_back(std::make_shared<Object>(std::make_shared<Sphere>(Poi3f{0.0f, -101.0f, -5.0f}, 100.0f), matte2));
	objects.emplace_back(std::make_shared<Object>(std::make_shared<Sphere>(Poi3f{ 0.0f, 1.0f, -5.0f }, 2.0f), matte));
	objects.emplace_back(std::make_shared<Object>(std::make_shared<Sphere>(Poi3f{ 0.0, 80.0f, 0.0f }, 50.0f), light2));
	/*objects.emplace_back(new Sphere({ 0, 1, -5 }, 1));
	objects.emplace_back(new Sphere({ 5, 1, -5 }, 1));
	objects.emplace_back(new Sphere({ -5, 1, -5 }, 1));
	objects.emplace_back(new Sphere({ -5, 5, -5 }, 1));*/
	std::shared_ptr<Scene> scene{ new Scene(objects) };
	Renderer r{ scene };
	Camera c{ {320 * 5, 180 * 5}, 90, r, 1000 };
	//Camera c{ {150, 100}, 90, r, 1000 };
	//Camera c{ {1920, 1080}, 90, s };
	std::cout << t.mark().count() << std::endl;

	/*Intersection i{};
	Ray ray{ {0,0,0}, {0, 0, -1} };
	bool hit = scene->intersect(ray, &i);
	float weight = 0;
	for (int n = 0; n < 10; n++) {
		Ray r = i.m->getScatteredRay(i, &weight);
		std::cout << r.dir << " " << weight << "\n";
	}*/

	std::cout << "Rendering Scene: ";
	Image render = c.renderImage({});
	std::cout << t.mark().count() << std::endl;

	std::cout << "Writing Image To File: ";
	std::ofstream imgFile;
	imgFile.open("render.ppm", std::ofstream::binary);
	render.writeEncodedPpm(imgFile);
	imgFile << render;
	imgFile.close();
	std::cout << t.mark().count() << std::endl;
	return 0;
};

