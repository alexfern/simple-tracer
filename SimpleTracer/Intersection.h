#pragma once

#include "LinearAlg.h"

struct Material;

struct Intersection {
	//Outgoing ray direction
	Vec3f wo;

	//Point of intersection and normal to surface
	Poi3f p;
	Norm3f n;
	
	//UV cooridinates of point and partials of point to them
	Poi2f uv;
	Vec3f dpdu, dpdv;

	Material* m;
};