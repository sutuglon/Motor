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

#ifndef OZCOLLIDE_BOX_H
#define OZCOLLIDE_BOX_H

#ifndef OZCOLLIDE_PCH
#include <ozcollide/ozcollide.h>
#endif

#include <ozcollide/plane.h>
#include <ozcollide/vec3f.h>

ENTER_NAMESPACE_OZCOLLIDE

class BoxEdge {
public:
	BoxEdge(const Vec3f &a, const Vec3f &b) : p0(a) , p1(b) { };

	Vec3f p0, p1;
};

class Box {
public:

	ozinline Box()
	{
	}

	ozinline Box(const Vec3f &center, const Vec3f &extent) : center(center), extent(extent)
	{
	}

	~Box()
	{
	}

	// Build an AABB from two points
	ozinline void setFromPoints(const Vec3f &_pt0, const Vec3f &_pt1)
	{
		center = (_pt1 + _pt0) * 0.5f;
		extent = (_pt1 - _pt0) * 0.5f;
		extent.positive();
	}

	// Get the min corner
	ozinline Vec3f getMin() const
	{
		return center - extent;
	}

	// Get the max corner
	ozinline Vec3f getMax() const
	{
		return center + extent;
	}

	// Get the min and max
	ozinline void getMinMax(Vec3f &_min, Vec3f &_max) const
	{
		_min = center - extent;
		_max = center + extent;
	}

	// Test if that box overlaps with one another
	ozinline bool isOverlap(const Box &_box) const
	{
		if (isOverlapOnX(_box) &&
			isOverlapOnY(_box) &&
			isOverlapOnZ(_box))
			return true;

		return false;
	}

	// Test if that box overlaps with one another on the X axis only
	ozinline bool isOverlapOnX(const Box &_box) const
	{
		float t = _box.center.x - center.x;
		if (t < 0) t = -t;

		float ext = _box.extent.x + extent.x;
		if (t <= ext)
			return true;
		return false;
	}

	// Test if that box overlaps with one another on the Y axis only
	ozinline bool isOverlapOnY(const Box &_box) const
	{
		float t = _box.center.y - center.y;
		if (t < 0) t = -t;

		float ext = _box.extent.y + extent.y;
		if (t <= ext)
			return true;
		return false;
	}

	// Test if that box overlaps with one another on the Z axis only
	ozinline bool isOverlapOnZ(const Box &_box) const
	{
		float t = _box.center.z - center.z;
		if (t < 0) t = -t;

		float ext = _box.extent.z + extent.z;
		if (t <= ext)
			return true;
		return false;
	}

	// Test if that box is fully inside one another
	ozinline bool isInside(const Box &_box) const
	{
		if (center.x - extent.x > _box.center.x - _box.extent.x &&
			center.x + extent.x < _box.center.x + _box.extent.x &&
			center.y - extent.y > _box.center.y - _box.extent.y &&
			center.y + extent.y < _box.center.y + _box.extent.z &&
			center.z - extent.z > _box.center.z - _box.extent.z &&
			center.z + extent.z < _box.center.z + _box.extent.z)
			return true;
		
		return false;
	}

	// Test if a point is inside that box
	ozinline bool isInside(const Vec3f &_pt) const
	{
		Vec3f min = getMin();
		Vec3f max = getMax();

		if (_pt.x < min.x || _pt.x > max.x ||
			_pt.y < min.y || _pt.y > max.y ||
			_pt.z < min.z || _pt.z > max.z)
			return false;

		return true;
	}

	// Get the smallest extension
	ozinline float getMinExtent() const
	{
		if (extent.x < extent.y && extent.x < extent.z)
			return extent.x;
		if (extent.y < extent.z)
			return extent.y;
		return extent.z;
	}

	// Get the highest extension
	ozinline float getMaxExtent() const
	{
		if (extent.x > extent.y && extent.x > extent.z)
			return extent.x;
		if (extent.y > extent.z)
			return extent.y;
		return extent.z;
	}

	ozinline float ext(int i) const
	{
		if (i == 0) return extent.x;
		else if (i == 1) return extent.y;
		return extent.z;
	}

	// lazy evaluation rules...

	ozinline Vec3f getPoint(int _index) const
	{
		switch(_index) {
		case 0: return Vec3f(center.x - extent.x, center.y - extent.y, center.z - extent.z);
		case 1: return Vec3f(center.x + extent.x, center.y - extent.y, center.z - extent.z);
		case 2: return Vec3f(center.x - extent.x, center.y + extent.y, center.z - extent.z);
		case 3: return Vec3f(center.x + extent.x, center.y + extent.y, center.z - extent.z);
		case 4: return Vec3f(center.x - extent.x, center.y - extent.y, center.z + extent.z);
		case 5: return Vec3f(center.x + extent.x, center.y - extent.y, center.z + extent.z);
		case 6: return Vec3f(center.x - extent.x, center.y + extent.y, center.z + extent.z);
		case 7: return Vec3f(center.x + extent.x, center.y + extent.y, center.z + extent.z);
		default: return ZEROVEC3F;
		}
	}

	ozinline static Vec3f getVertexNormal(int _index)
	{
		switch(_index) {
		case 0: return Vec3f(-OZ_INVQSQR3, -OZ_INVQSQR3, -OZ_INVQSQR3);
		case 1: return Vec3f(+OZ_INVQSQR3, -OZ_INVQSQR3, -OZ_INVQSQR3);
		case 2: return Vec3f(-OZ_INVQSQR3, +OZ_INVQSQR3, -OZ_INVQSQR3);
		case 3: return Vec3f(+OZ_INVQSQR3, +OZ_INVQSQR3, -OZ_INVQSQR3);
		case 4: return Vec3f(-OZ_INVQSQR3, -OZ_INVQSQR3, +OZ_INVQSQR3);
		case 5: return Vec3f(+OZ_INVQSQR3, -OZ_INVQSQR3, +OZ_INVQSQR3);
		case 6: return Vec3f(-OZ_INVQSQR3, +OZ_INVQSQR3, +OZ_INVQSQR3);
		case 7: return Vec3f(+OZ_INVQSQR3, +OZ_INVQSQR3, +OZ_INVQSQR3);
		default: return ZEROVEC3F;
		}
	}

	ozinline static Vec3f getFaceNormal(PLANE _which)
	{
		switch(_which) {
			case 0: return Vec3f(-1,  0,  0);
			case 1: return Vec3f( 1,  0,  0);
			case 2: return Vec3f( 0, -1,  0);
			case 3: return Vec3f( 0,  1,  0);
			case 4: return Vec3f( 0,  0, -1);
			case 5: return Vec3f( 0,  0,  1);
			default: return ZEROVEC3F;
		}
	}

	ozinline Plane getPlane(PLANE _which) const
	{
		Plane plane;
		switch(_which) {
			case 0: plane.fromPointAndNormal(Vec3f(center.x - extent.x, 0, 0), getFaceNormal(_which));
			case 1: plane.fromPointAndNormal(Vec3f(center.x + extent.x, 0, 0), getFaceNormal(_which));
			case 2: plane.fromPointAndNormal(Vec3f(0, center.y - extent.y, 0), getFaceNormal(_which));
			case 3: plane.fromPointAndNormal(Vec3f(0, center.y + extent.y, 0), getFaceNormal(_which));
			case 4: plane.fromPointAndNormal(Vec3f(0, 0, center.z - extent.z), getFaceNormal(_which));
			case 5: plane.fromPointAndNormal(Vec3f(0, 0, center.z + extent.z), getFaceNormal(_which));
			default: plane = Plane(0, 0, 0, 0);
		}
		return plane;
	}

	ozinline BoxEdge getEdge(int _index) const
	{
		float x0 = center.x - extent.x;
		float y0 = center.y - extent.y;
		float z0 = center.z - extent.z;
		float x1 = center.x + extent.x;
		float y1 = center.y + extent.y;
		float z1 = center.z + extent.z;

		switch(_index) {
			case 0: return BoxEdge(Vec3f(x0, y0, z0), Vec3f(x1, y0, z0));
			case 1: return BoxEdge(Vec3f(x1, y0, z0), Vec3f(x1, y1, z0));
			case 2: return BoxEdge(Vec3f(x1, y1, z0), Vec3f(x0, y1, z0));
			case 3: return BoxEdge(Vec3f(x0, y1, z0), Vec3f(x0, y0, z0));

			case 4: return BoxEdge(Vec3f(x0, y0, z1), Vec3f(x1, y0, z1));
			case 5: return BoxEdge(Vec3f(x1, y0, z1), Vec3f(x1, y1, z1));
			case 6: return BoxEdge(Vec3f(x1, y1, z1), Vec3f(x0, y1, z1));
			case 7: return BoxEdge(Vec3f(x0, y1, z1), Vec3f(x0, y0, z1));

			case 8: return BoxEdge(Vec3f(x0, y0, z0), Vec3f(x0, y0, z1));
			case 9: return BoxEdge(Vec3f(x1, y0, z0), Vec3f(x1, y0, z1));
			case 10: return BoxEdge(Vec3f(x1, y1, z0), Vec3f(x1, y1, z1));
			case 11: return BoxEdge(Vec3f(x0, y1, z0), Vec3f(x0, y1, z1));

			default: return BoxEdge(ZEROVEC3F, ZEROVEC3F);
		}
	}

	Vec3f center;
	Vec3f extent;
};

LEAVE_NAMESPACE

#endif
