/*
OZCollide - Collision Detection Library
Copyright (C) 2006  Igor Kravtchenko

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Contact the author: igor@tsarevitch.org
*/

#ifndef OZCOLLIDE_PLANE_H
#define OZCOLLIDE_PLANE_H

#ifndef OZCOLLIDE_PCH
#include <ozcollide/ozcollide.h>
#endif

#include <ozcollide/vec3f.h>

ENTER_NAMESPACE_OZCOLLIDE

enum PLANE {
	LEFT_PLANE, RIGHT_PLANE,
	BOTTOM_PLANE, TOP_PLANE,
	NEAR_PLANE, FAR_PLANE
};

class Plane {
public:

	OZCOLLIDE_API Plane();
	OZCOLLIDE_API Plane(float a, float b, float c, float d);

	OZCOLLIDE_API void fromPoints(const Vec3f &p0, const Vec3f &p1, const Vec3f &p2);
	OZCOLLIDE_API void fromPointsNN(const Vec3f &p0, const Vec3f &p1, const Vec3f &p2);
	OZCOLLIDE_API void fromPointAndNormal(const Vec3f &p, const Vec3f &n);

	ozinline float dot(const Vec3f &p) const
	{
		return a * p.x + b * p.y + c * p.z;
	}

	ozinline float dist(const Vec3f &p) const
	{
		return a * p.x + b * p.y + c * p.z + d;
	}

	ozinline Vec3f reflect(const Vec3f &vec)
	{
		float d = dist(vec);
		return  vec + 2 * Vec3f(-a, -b, -c) * d;
	}

	ozinline Vec3f project(const Vec3f &p)
	{
		float h = dist(p);
		return Vec3f(	p.x - a * h,
						p.y - b * h,
						p.z - c * h);
	}

	ozinline bool isOnPlane(const Vec3f &p, float threshold = 0.001f)
	{
		float d = dist(p);
		if (d < threshold && d > -threshold)
			return true;
		return false;
	}

	// Calcul the intersection between this plane and a line
	// If the plane and the line are parallel, OZFALSE is returned
	ozinline bool intersectWithLine(const Vec3f &p0, const Vec3f &p1, float &t)
	{
		Vec3f dir = p1 - p0;
		float div = dot(dir);
		if (div == 0)
			return false;

		t = -dist(p0) / div;
		return true;
	}

	float a, b, c, d;
};

LEAVE_NAMESPACE

#endif
