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

#ifndef OZCOLLIDE_SPHERE_H
#define OZCOLLIDE_SPHERE_H

#ifndef OZCOLLIDE_PCH
#include <ozcollide/ozcollide.h>
#endif

#include <ozcollide/vec3f.h>

ENTER_NAMESPACE_OZCOLLIDE

class Sphere {

public:
	Sphere() { };
	Sphere(const Vec3f &_center, float _radius) : center(_center), radius(_radius) { };

	ozinline bool isCollide(const Sphere &_s) const 
	{
		Vec3f diff = _s.center - center;
		float sqDist = diff.lenSq();
		float r = _s.radius + radius;
		if (sqDist <= r * r)
			return true;
		return false;
	}

	ozinline bool isPointInside(const Vec3f _pt) const
	{
		float dist = (_pt - center).len();
		return dist < radius ? true : false;
	}

	Vec3f center;
	float radius;
};

LEAVE_NAMESPACE

#endif
