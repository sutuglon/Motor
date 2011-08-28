/*
 *  inter.cpp
 *  GLSL Demo
 *
 *  Created by Chema on 2/22/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "intersection.h"

//    Points P (x,y) on a line defined by two points P1 (x1,y1,z1) and P2 (x2,y2,z2) is described by
//
//    P = P1 + u (P2 - P1)
//    or in each coordinate
//
//    x = x1 + u (x2 - x1)
//    y = y1 + u (y2 - y1)
//    z = z1 + u (z2 - z1)
//    A sphere centered at P3 (x3,y3,z3) with radius r is described by
//
//    (x - x3)2 + (y - y3)2 + (z - z3)2 = r2
//    Substituting the equation of the line into the sphere gives a quadratic equation of the form
//
//    a u2 + b u + c = 0
//    where:
//
//    a = (x2 - x1)2 + (y2 - y1)2 + (z2 - z1)2
//    b = 2[ (x2 - x1) (x1 - x3) + (y2 - y1) (y1 - y3) + (z2 - z1) (z1 - z3) ]
//
//    c = x32 + y32 + z32 + x12 + y12 + z12 - 2[x3 x1 + y3 y1 + z3 z1] - r2
//
//    The solutions to this quadratic are described by
//
//
//    The exact behaviour is determined by the expression within the square root
//
//    b * b - 4 * a * c
//
//    If this is less than 0 then the line does not intersect the sphere.
//    If it equals 0 then the line is a tangent to the sphere intersecting it at one point, 
//    namely at u = -b/2a.
//    If it is greater then 0 the line intersects the sphere at two points.
//
//    To apply this to two dimensions, that is, the intersection of a line and a circle simply 
//    remove the z component from the above mathematics.
//    Line Segment
//    For a line segment between P1 and P2 there are 5 cases to consider.
//
//    Line segment doesn't intersect and on outside of sphere, 
//      in which case both values of u wll either be less than 0 or greater than 1.
//    Line segment doesn't intersect and is inside sphere, 
//      in which case one value of u will be negative and the other greater than 1.
//    Line segment intersects at one point, 
//      in which case one value of u will be between 0 and 1 and the other not.
//    Line segment intersects at two points, 
//      in which case both values of u will be between 0 and 1.
//    Line segment is tangential to the sphere, 
//      in which case both values of u will be the same and between 0 and 1.
//
//    When dealing with a line segment it may be more efficient to first 
//      determine whether the line actually intersects the sphere or circle. 
//    This is achieved by noting that the closest point on the line through P1P2 to the point P3 
//      is along a perpendicular from P3 to the line. 
//    In other words if P is the closest point on the line then
//
//    (P3 - P) dot (P2 - P1) = 0
//    Substituting the equation of the line into this
//
//    [P3 - P1 - u(P2 - P1)] dot (P2 - P1) = 0
//    Solving the above for u =
//
//    (x3 - x1)(x2 - x1) + (y3 - y1)(y2 - y1) + (z3 - z1)(z2 - z1) 
//    ----------------------------------------------------------- 
//    (x2 - x1)(x2 - x1) + (y2 - y1)(y2 - y1) + (z2 - z1)(z2 - z1)
//    If u is not between 0 and 1 then the closest point is not between P1 and P2 Given u, 
//    the intersection point can be found, it must also be less than the radius r. 
//    If these two tests succeed then the earlier calculation of the actual intersection 
//      point can be applied.
//
//       Calculate the intersection of a ray and a sphere
//       The line segment is defined from p1 to p2
//       The sphere is of radius r and centered at sc
//       There are potentially two points of intersection given by
//       p = p1 + u1 (p2 - p1)
//       p = p1 + u2 (p2 - p1)
//       Return FALSE if the ray doesn't intersect the sphere.

bool RaySphere(Real3 p1,Real3 p2,Real3 sc, real r,real& u1,real& u2)
{
    real a,b,c;
    real bb4ac;
    Real3 dp = p2-p1;
    Real3 l = p1-sc;

    a = dotProd(dp,dp);
    b = 2*dotProd(dp,l);
    c = dotProd(sc,sc)+dotProd(p1,p1)-2*dotProd(sc,p1)-r*r;
    
    bb4ac = b * b - 4 * a * c;
    if (fabs(a) < EPSILON6 || bb4ac < 0) {
      u1 = u2 = 0;
      return false;
    }
    real disc =sqrt(bb4ac);
    u1 = (-b + disc) / (2 * a);
    u2 = (-b - disc) / (2 * a);
    return true;
}