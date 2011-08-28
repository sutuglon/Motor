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

#ifndef OZCOLLIDE_POLYGON_H
#define OZCOLLIDE_POLYGON_H

#ifndef OZCOLLIDE_PCH
#include <ozcollide/ozcollide.h>
#endif

#include <ozcollide/vec3f.h>
#include <ozcollide/plane.h>

ENTER_NAMESPACE_OZCOLLIDE

class Polygon {
public:
	OZCOLLIDE_API Polygon();
	OZCOLLIDE_API ~Polygon();

	// Get/Set the number of indices of this polygon.
	// Note that changing the number of indices doesn't erase the old content of the object.
	// For example passing from a triangle (3 indices) to a quadrangle (4 indices) keeps
	// the 3 first indices intact.
	// This methods make uses of malloc() and so, is slower than setIndicesMemory()
	OZCOLLIDE_API void setNbIndices(int nbIndices);

	// Force this polygon to use the supplied memory location for its indices
	// That's allow for example a big block of memory to be chunked into several set of indices
	// without the heavy mechanism of new memory allocation
	ozinline void setIndicesMemory(int _nbIndices, int *_indices)
	{
		nbIndices_ = _nbIndices | 0x100;
		indices_ = _indices;
	}

	ozinline int getNbIndices() const { return nbIndices_ & 0xff; }

	ozinline int getIndex(int i) const { return indices_[i]; }
	ozinline void setIndex(int i, int index) { indices_[i] = index; }

	// Get/Set the normal of the polygon
	ozinline const Vec3f& getNormal() const { return normal_; }
	ozinline void setNormal(const Vec3f &normal) { normal_ = normal; }

	OZCOLLIDE_API bool isDegenerate() const;

	// Copy this polygon (nbIndices, indices and normal) to one another polygon.
	// It causes its content to be erased by this one.
	OZCOLLIDE_API void copyTo(Polygon &) const;

	OZCOLLIDE_API Polygon* clone() const;

	ozinline Plane calculPlane(const Vec3f *_pnts) const
	{
		Plane plane;
		plane.fromPointAndNormal(_pnts[indices_[0]], normal_);
		return plane;
	}

private:
	int		nbIndices_;
	int		*indices_;
	Vec3f	normal_;
};

LEAVE_NAMESPACE

#endif
