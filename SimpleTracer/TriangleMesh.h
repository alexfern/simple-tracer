#pragma once

#include "Ray.h"
#include "Shape.h"
#include "LinearAlg.h"
#include "Triangle.h"

class TriangleMesh : public Shape {
private:
	int numVerts;
	Poi3f* verts;
	Poi2f* vertUvs;
	bool hasVertNorms;
	Norm3f* vertNorms;
	
	int numTris;
	int* vertIndexes;
	
private:
	void getVertIndexes(int triIndex, int& a, int& b, int& c) const {
		a = vertIndexes[3 * triIndex];
		b = vertIndexes[3 * triIndex + 1];
		c = vertIndexes[3 * triIndex + 2];
	}

public:
	TriangleMesh() {

	}

	virtual bool intersect(const Ray& ray, Intersection* insect = nullptr) const {
		if (numTris < 1)
			throw "Fewer than 1 triangle";
		int closestIndex = 0;

		bool hit = true;
		for (int triIndex = 0; triIndex < numTris; triIndex++) {
			int iA, iB, iC;
			getVertIndexes(triIndex, iA, iB, iC);
			Triangle tri{ iA, iB, iC, verts, vertUvs, vertNorms, hasVertNorms };

			hit = hit || tri.intersect(ray, insect);
		}
		return hit;
	}

	~TriangleMesh() {
		delete[] verts, vertUvs, vertNorms, vertIndexes;
		verts = nullptr;
		vertUvs = nullptr;
		vertNorms = nullptr;
		vertIndexes = nullptr;
	}
};