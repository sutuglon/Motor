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

#ifndef OZCOLLIDE_INTERSECTION_BOXTRI_H
#define OZCOLLIDE_INTERSECTION_BOXTRI_H

#ifndef OZCOLLIDE_PCH
#include <ozcollide/ozcollide.h>
#endif

ENTER_NAMESPACE_OZCOLLIDE

class Polygon;
class Vec3f;
class Box;
class OBB;

OZCOLLIDE_API bool testIntersectionTriBox(const Vec3f *pts,
										 const Box &);

OZCOLLIDE_API bool testIntersectionTriBox(const Polygon &,
										 const Vec3f *pnts,
										 const Box &);

OZCOLLIDE_API bool testIntersectionTriOBB(const Polygon &,
										 const Vec3f *pnts,
										 const OBB &);

// triangle and box are stationary
OZCOLLIDE_API bool magic_testIntersectionTriBox(const Vec3f *apkTri[3],
											   const Box &rkBox);

// triangle and box have constant linear velocity
OZCOLLIDE_API bool magic_testIntersectionTriBox(const Vec3f *apkTri[3],
											   const Box &rkBox,
											   const Vec3f &rkBoxVel,
											   float fTMax,
											   float &rfTFirst,
											   float &rfTLast);

// this method use the method as described by Watt/Policarpo[01]
OZCOLLIDE_API bool testIntersectionTriBox(const Vec3f *tri_pts[3],
										 const Vec3f &tri_normal,
										 const Box &box,
										 const Vec3f &box_vel,
										 float &dist_travel,
										 Vec3f &reaction);

LEAVE_NAMESPACE

#endif
