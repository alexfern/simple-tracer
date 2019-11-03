#pragma once

#include "LinearAlg.h"
#include "Intersection.h"
#include "Ray.h"
#include <cmath>

struct Transform {
public:
	Transform() :
		Transform(Mat44f::I(), Mat44f::I())
	{}

	Transform(const Mat44f& m) :
		m(m),
		mInv(inv(m))
	{}

	Transform(const Mat44f& m, const Mat44f& mInv) :
		m(m),
		mInv(mInv)
	{}

	static Transform Rotation(float angle, const Vec3f& axis) {
		Vec3f u = normalize(axis);
		Mat44f trans = Mat44f::I();
		trans[0][0] = cos(angle) + u.x * u.x * (1 - cos(angle));
		trans[0][1] = u.x * u.y * (1 - cos(angle)) - u.z * sin(angle);
		trans[0][2] = u.x * u.z * (1 - cos(angle)) + u.y * sin(angle);

		trans[1][0] = u.y * u.x * (1 - cos(angle)) + u.z * sin(angle);
		trans[1][1] = cos(angle) + u.y * u.y * (1 - cos(angle));
		trans[1][2] = u.y * u.z * (1 - cos(angle)) - u.x * sin(angle);

		trans[2][0] = u.z * u.x * (1 - cos(angle)) - u.y * sin(angle);
		trans[2][1] = u.z * u.y * (1 - cos(angle)) + u.x * sin(angle);
		trans[2][2] = cos(angle) + u.z * u.z * (1 - cos(angle));
		return Transform(trans);
	}

	static Transform Scale(float sx, float sy, float sz) {
		Mat44f trans = Mat44f::I();
		trans[0][0] = sx;
		trans[1][1] = sy;
		trans[2][2] = sz;
		return Transform(trans);
	}

	static Transform Scale(float scale) {
		return Transform::Scale(scale, scale, scale);
	}

	static Transform Translation(float dx, float dy, float dz) {
		Mat44f trans = Mat44f::I();
		trans[0][3] = dx;
		trans[1][3] = dy;
		trans[2][3] = dz;
		return Transform(trans);
	}

	static Transform Translation(const Vec3f displacement) {
		return Transform::Translation(displacement.x, displacement.y, displacement.z);
	}


	Transform apply(const Transform& other) const {
		return Transform(other.m * m, mInv * other.mInv);
	}

	Transform operator()(const Transform& other) const {
		return Transform(m * other.m, other.mInv * mInv);
	}

	Vec4f operator()(const Vec4f& affline) const {
		return m * affline;
	}

	Vec3f operator()(const Vec3f& vec) const {
		Vec4f affline{ vec.x, vec.y, vec.z, 0 };
		return operator()(affline).xyz;
	}

	Poi3f operator()(const Poi3f& poi) const {
		Poi4f affline{ poi.x, poi.y, poi.z, 1 };
		return (m * affline).xyz;
	}

	Norm3f operator()(const Norm3f& norm) const {
		//http://www.pbr-book.org/3ed-2018/Geometry_and_Transformations/Applying_Transformations.html#Normals
		Mat44f s = transpose(mInv);
		Norm4f affline{ norm.x, norm.y, norm.z, 0 };
		return normalize((s * affline).xyz);
	}

	Vec4f getTranslation() const {
		return Vec4f(m.pull<4, 1>(3, 0));
	}

	Ray operator()(Ray& ray) const {
		Ray r{ operator()(ray.org), operator()(ray.dir) };
		float dir1 = r.dir.length();
		float dir2 = ray.dir.length();
		float k = 1;
		if (dir1 == 0 || dir2 == 0) {
			k = 1;
		}
		else {
			float k = r.dir.length() / ray.dir.length();
		}
		r.tMin = ray.tMin * k;
		r.tMax = ray.tMax * k;
		if (r.tMax < r.BIG_ASS_NUMBER / 2 && !r.tMax > .01f) {
			std::cout << " tmax:" << r.tMax << "\n";
		}
		return r;
	}

	Ray operator()(const Ray& ray) const {
		Ray r{ operator()(ray.org), operator()(ray.dir) };
		float dir1 = r.dir.length();
		float dir2 = ray.dir.length();
		float k = 0;
		if (dir1 == 0 || dir2 == 0)
			k = 1;
		else {
			k = r.dir.length() / ray.dir.length();
		}
		r.tMin = ray.tMin * k;
		r.tMax = ray.tMax * k;
		return r;
	}

	friend Transform inv(const Transform& trans) {
		return Transform(trans.mInv, trans.m);
	}

	Intersection operator()(const Intersection& insect) const {
		Intersection i{};
		i.wo = operator()(insect.wo);
		i.p = operator()(insect.p);
		i.n = operator()(insect.n);
		i.uv = insect.uv;
		i.dpdu = operator()(insect.dpdu);
		i.dpdv = operator()(insect.dpdv);
		return i;
	}

private:
	Mat44f m;
	Mat44f mInv;
};