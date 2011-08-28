/*
 *  matrix.h
 *  VSweep
 *
 *  Created by Chema on 8/25/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "vec.h"

template<typename T>
Vec<T,3> Matrix4Rotate(const Vec<T,16>& A, const Vec<T,3>& x) {
    Vec<T,3> ret;
    ret[0] = A[0]*x[0] + A[1]*x[1] + A[2]*x[2];
    ret[1] = A[4]*x[0] + A[5]*x[1] + A[6]*x[2];
    ret[2] = A[8]*x[0] + A[9]*x[1] + A[10]*x[2];
    return ret;
}

template<typename T>
Vec<T,3> Matrix4Affine(const Vec<T,16>& A, const Vec<T,3>& x) {
    Vec<T,3> ret;
    ret[0] = A[0]*x[0] + A[1]*x[1] + A[2]*x[2]+ A[3];
    ret[1] = A[4]*x[0] + A[5]*x[1] + A[6]*x[2]+ A[7];
    ret[2] = A[8]*x[0] + A[9]*x[1] + A[10]*x[2]+ A[11];
    return ret;
}

template<typename T>
Vec<T,3> GetTranslation(const Vec<T,16>& A) {
    Real3 ret;
    ret[0]= A[3];
    ret[1]= A[7];
    ret[2]= A[11];
    return ret;
}

template<typename T>
void SetTranslation(Vec<T,16>& A, const Vec<T,3>& t) {
    A[3]= t[0];
    A[7]= t[1];
    A[11]= t[2];
}

template<typename T>
Vec<T,16> Matrix4Identity() {
    Vec<T,16> ret;
    ret[0] = ret[5] = ret[10] = ret [15]= 1;
    return ret;
}

template<typename T>
Vec<T,16> Rotation(const real angle, const Vec<T,3>& axis)
{
    Vec<T,3> a =normalize( axis );
    real s = sinf(angle);
    real c = cosf(angle);
    real t = 1.0-c;

    Vec<T,16> ret;
    ret[0]= a[0] * a[0] + (1.0 - a[0] * a[0]) * c;
    ret[1]= a[0] * a[1] * t - a[3] * s;
    ret[2]= a[0] * a[3] * t + a[1] * s;
    ret[3]= 0;

    ret[4]= a[0] * a[1] * t + a[3] * s;
    ret[5]= a[1] * a[1] + (1.0 - a[1] * a[1]) * c;
    ret[6]= a[1] * a[3] * t - a[0] * s;
    ret[7]= 0;

    ret[8]= a[0] * a[3] * t - a[1] * s;
    ret[9]= a[1] * a[3] * t + a[0] * s;
    ret[10]=a[3] * a[3] + (1.0 - a[3] * a[3]) * c;
    ret[11]= 0;

    ret[12]= 0;
    ret[13]= 0;
    ret[14]= 0;
    ret[15]= 1;

    return ret;
}
