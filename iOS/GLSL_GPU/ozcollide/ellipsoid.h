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

#ifndef OZCOLLIDE_ELLIPSOID_H
#define OZCOLLIDE_ELLIPSOID_H

#ifndef OZCOLLIDE_PCH
#include <ozcollide/ozcollide.h>
#endif

#include <ozcollide/vec3f.h>

ENTER_NAMESPACE_OZCOLLIDE

class Ellipsoid {
public:
	ozinline Ellipsoid() : center_(Vec3f(0, 0, 0)), radii_(Vec3f(0, 0, 0)) { };
	ozinline Ellipsoid(Vec3f _c, Vec3f _r) : center_(_c), radii_(_r) { };

	ozinline const Vec3f& getCenter() const { return center_; }
	ozinline void setCenter(const Vec3f &center) { center_ = center; }

	ozinline const Vec3f& getRadii() const { return radii_; }
	ozinline void setRadii(const Vec3f &_radii)
	{
		radii_ = _radii;
		invRadii_.x = 1.0f / _radii.x;
		invRadii_.y = 1.0f / _radii.y;
		invRadii_.z = 1.0f / _radii.z;
	}

	// Get the inverse radii. The result is cached from the last setRadii()'s call.
	ozinline const Vec3f& getInvRadii() const { return invRadii_; }

	// Get the support ellipsoid's point of a point
	// (a support point is a point located on the volume perimeter)
	ozinline Vec3f support(const Vec3f &_pt)
	{
		Vec3f p = (_pt - center_) * invRadii_;
		p.normalize();
		return p * radii_ + center_;
	}

protected:
	Vec3f center_;
	Vec3f radii_;
	Vec3f invRadii_;
};

LEAVE_NAMESPACE

#endif
