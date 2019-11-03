#pragma once

#include "Shape.h"
#include "Intersection.h"
#include "LinearAlg.h"

struct Triangle : public Shape {
	Poi3f *pA, *pB, *pC;
	Poi2f *uvA, *uvB, *uvC;
	Norm3f *nA, *nB, *nC;
	bool hasVertNorms;

	Triangle(int iA, int iB, int iC, Poi3f* verts, Poi2f* uvs, Norm3f* norms, bool hasVertNorms) {
		pA = &(verts[iA]);
		pB = &(verts[iB]);
		pC = &(verts[iC]);
		uvA = &(uvs[iA]);
		uvB = &(uvs[iB]);
		uvC = &(uvs[iC]);
		if (hasVertNorms) {
			nA = &(norms[iA]);
			nB = &(norms[iB]);
			nC = &(norms[iC]);
		}
		this->hasVertNorms = hasVertNorms;
	}

	virtual bool intersect(const Ray& r, Intersection* insect = nullptr) const {
		Mat33f matrix = Mat33f();
		matrix.put(0, 0, asRowMatrix(*pB - *pA));
		matrix.put(1, 0, asRowMatrix(*pC - *pA));
		matrix.put(2, 0, asRowMatrix(r.dir));
		Vec3f uvt = inv(matrix) * (r.org - *pA);
		Poi2f uv{ uvt.x, uvt.y }; //Localize to Triangle
		float t = uvt.z;
		bool hitSurface = uv.x >= 0 && uv.y >= 0 && uv.x + uv.y <= 1;
		if (t > r.tMax || t < r.tMin || !hitSurface) //Intesection either to close or to far, or didn't even hit
			return false;
		r.tMax = t;

		if (insect != nullptr) {
			insect->p = r(t);
			if (hasVertNorms) {
				insect->n = normalize(*nA + uv.x * (*nB - *nA) + uv.y * (*nC - *nA)); //Does this work?... maybe have seperate normals for shading anyways
			} else {
				insect->n = Norm3f(normalize(cross(*pB - *pA, *pC - *pA)));
			}
			insect->uv = *uvA + uv.x * (*uvB - *uvA) + uv.y * (*uvC - *uvA); //Transforms localized UVs to be relative to mesh
		}

		return true;
	}

	virtual ~Triangle() {
		pA = pB = pC = nullptr;
		uvA = uvB = uvC = nullptr;
		nA = nB = nC = nullptr;
	}
};