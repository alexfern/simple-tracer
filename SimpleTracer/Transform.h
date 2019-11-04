#pragma once

#include "LinearAlg.h"
#include "Intersection.h"
#include "Ray.h"

struct Transform {
private:
	Mat44f m;
	Mat44f mInv;
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

	static Transform Rotation(float angle, const Vec3f& axis);

	static Transform Scale(float sx, float sy, float sz);

	static Transform Scale(float scale);

	static Transform Translation(float dx, float dy, float dz);

	static Transform Translation(const Vec3f displacement);

	Transform apply(const Transform& other) const;

	Transform operator()(const Transform& other) const;

	Vec4f operator()(const Vec4f& affline) const;

	Vec3f operator()(const Vec3f& vec) const;

	Poi3f operator()(const Poi3f& poi) const;

	Norm3f operator()(const Norm3f& norm) const;

	Vec4f getTranslation() const;

	Ray operator()(const Ray& ray) const;

	friend Transform inv(const Transform& trans);

	Intersection operator()(const Intersection& insect) const;

	friend bool operator==(const Transform& lhs, const Transform& rhs);
};