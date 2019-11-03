#include <iostream>
#include <fstream>
#include "LinearAlg.h"
#include "Camera.h"
#include "Image.h"
#include "Sphere.h"
#include "Object.h"
#include "Material.h"
#include "Timer.h"

int main() {
	Timer t;
	t.mark();

	std::cout << "Initilizing Scene: ";
	std::vector<std::shared_ptr<Object>> objects;
	std::shared_ptr<Material> matte = std::make_shared<Material>(Vec3f{ .8f, .8f, 0.8f });
	std::shared_ptr<Material> matte2 = std::make_shared<Material>(Vec3f{ .5f, .5f, 0.5f });
	std::shared_ptr<Material> light = std::make_shared<Material>(Vec3f{ 0.1f, 0.1f, 0.1f }, Vec3f{.5f, .5f, .5f});

	objects.emplace_back(std::make_shared<Object>(std::make_shared<Sphere>(Poi3f{0, -101, 0}, 100), matte2));
	objects.emplace_back(std::make_shared<Object>(std::make_shared<Sphere>(Poi3f{ 0, 0, -5 }, 2), matte));
	objects.emplace_back(std::make_shared<Object>(std::make_shared<Sphere>(Poi3f{ 2, 2, -5 }, 1), light));
	/*objects.emplace_back(new Sphere({ 0, 1, -5 }, 1));
	objects.emplace_back(new Sphere({ 5, 1, -5 }, 1));
	objects.emplace_back(new Sphere({ -5, 1, -5 }, 1));
	objects.emplace_back(new Sphere({ -5, 5, -5 }, 1));*/
	std::shared_ptr<Scene> scene{ new Scene(objects) };
	Renderer r{ scene };
	//Camera c{ {320, 180}, 90, s };
	Camera c{ {150, 100}, 90, r, 100 };
	//Camera c{ {1920, 1080}, 90, s };
	std::cout << t.mark().count() << std::endl;

	std::cout << "Rendering Scene: ";
	Image render = c.renderImage({});
	std::cout << t.mark().count() << std::endl;

	std::cout << "Writing Image To File: ";
	std::ofstream imgFile;
	imgFile.open("render.ppm", std::ofstream::binary);
	render.writeEncodedPpm(imgFile);
	//imgFile << render;
	imgFile.close();
	std::cout << t.mark().count() << std::endl;
	return 0;
};

