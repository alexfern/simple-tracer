#pragma once

#include "LinearAlg.h"
#include <vector>

struct Image {
	using Pixel = Vec3<uint8_t>;
private:
	std::vector<Pixel> pixelBuffer;
	size_t width;
	size_t height;
public:
	Image(size_t width, size_t height) :
		width(width),
		height(height),
		pixelBuffer(width* height, Pixel{ 0, 0, 0 })
	{}

	size_t getWidth() const {
		return width;
	}

	size_t getHeight() const {
		return height;
	}

	Pixel getPixel(int x, int y) const {
		return pixelBuffer[x * height + y];
	}

	Pixel& getPixel(int x, int y) {
		return pixelBuffer[x * height + y];
	}

	Pixel putPixel(int x, int y, Pixel p) {
		return (getPixel(x, y) = p);
	}

	Pixel operator()(int x, int y) const {
		return getPixel(x, y);
	}

	Pixel& operator()(int x, int y) {
		return getPixel(x, y);
	}

	void writePlainPpm(std::ostream& stream) const {
		stream << "P3\n" << width << " " << height << "\n255\n";
		for (size_t y = 0; y < height; ++y) {
			for (size_t x = 0; x < width; ++x) {
				Pixel p = getPixel(x, y);
				stream << (int) p.r << " " << (int) p.g << " " << (int) p.b << "\n";
			}
		}
	}

	void writeEncodedPpm(std::ostream& stream) const {
		stream << "P6\n" << width << " " << height << "\n255\n";
		for (size_t y = 0; y < height; ++y) {
			for (size_t x = 0; x < width; ++x) {
				Pixel p = getPixel(x, y);
				stream.write((char*) p.data, sizeof(p.data));
			}
		}
	}

	friend std::ostream& operator<<(std::ostream& lhs, const Image& rhs) {
		rhs.writePlainPpm(lhs);
		return lhs;
	}
};