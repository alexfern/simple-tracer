#include "Transform.h"
#include <cmath>

static const Transform I;

Transform Transform::Rotation(float angle, const Vec3f& axis) {
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

Transform Transform::Scale(float sx, float sy, float sz) {
	Mat44f trans = Mat44f::I();
	trans[0][0] = sx;
	trans[1][1] = sy;
	trans[2][2] = sz;
	return Transform(trans);
}

Transform Transform::Scale(float scale) {
	return Transform::Scale(scale, scale, scale);
}

Transform Transform::Translation(float dx, float dy, float dz) {
	Mat44f trans = Mat44f::I();
	trans[0][3] = dx;
	trans[1][3] = dy;
	trans[2][3] = dz;
	return Transform(trans);
}

Transform Transform::Translation(const Vec3f displacement) {
	return Transform::Translation(displacement.x, displacement.y, displacement.z);
}

Transform Transform::apply(const Transform& other) const {
	return Transform(other.m * m, mInv * other.mInv);
}

Transform Transform::operator()(const Transform& other) const {
	return Transform(m * other.m, other.mInv * mInv);
}

Vec4f Transform::operator()(const Vec4f& affline) const {
	if (*this == I)
		return affline;
	return m * affline;
}

Vec3f Transform::operator()(const Vec3f& vec) const {
	if (*this == I)
		return vec;
	Vec4f affline{ vec.x, vec.y, vec.z, 0 };
	return operator()(affline).xyz;
}

Poi3f Transform::operator()(const Poi3f& poi) const {
	if (*this == I)
		return poi;
	Poi4f affline{ poi.x, poi.y, poi.z, 1 };
	return (m * affline).xyz;
}

Norm3f Transform::operator()(const Norm3f& norm) const { //http://www.pbr-book.org/3ed-2018/Geometry_and_Transformations/Applying_Transformations.html#Normals
	if (*this == I)
		return norm;
	Mat44f s = transpose(mInv);
	Norm4f affline{ norm.x, norm.y, norm.z, 0 };
	return normalize((s * affline).xyz);
}

Vec4f Transform::getTranslation() const {
	return Vec4f(m.pull<4, 1>(3, 0));
}

Ray Transform::operator()(const Ray& ray) const {
	if (*this == I)
		return ray;
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

Intersection Transform::operator()(const Intersection& insect) const {
	if (*this == I)
		return insect;
	Intersection i{ insect };
	i.wo = operator()(insect.wo);
	i.p = operator()(insect.p);
	i.n = operator()(insect.n);
	i.dpdu = operator()(insect.dpdu);
	i.dpdv = operator()(insect.dpdv);
	return i;
}

Transform inv(const Transform& trans) {
	if (trans == I)
		return trans;
	return Transform(trans.mInv, trans.m);
}

bool operator==(const Transform& lhs, const Transform& rhs) {
	return lhs.m == rhs.m;
}
