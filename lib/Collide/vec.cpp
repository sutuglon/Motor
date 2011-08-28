/*
 *  vector.cpp
 *  GF
 *
 *  Created by Chema on 10/23/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "vec.h"


Real2 orthogonal(const Real2& ref) {
    return Real2(-ref[1],ref[0]);
}
DReal2 orthogonal(const DReal2& ref) {
    return DReal2(-ref[1],ref[0]);
}

Matrix3 Identity() {
    Matrix3 ret;
    ret[0] = ret[4] = ret[8] = 1;
    return ret;
}

//Matrix4 Rotation(const real angle, const Real3& axis)
//{
//    Real3 a =normalize( axis );
//    real s = sinf(angle);
//    real c = cosf(angle);
//    real t = 1.0-c;
//
//    Matrix4 ret;
//    ret[0]= a[0] * a[0] + (1.0 - a[0] * a[0]) * c;
//    ret[1]= a[0] * a[1] * t - a[3] * s;
//    ret[2]= a[0] * a[3] * t + a[1] * s;
//    ret[3]= 0;
//
//    ret[4]= a[0] * a[1] * t + a[3] * s;
//    ret[5]= a[1] * a[1] + (1.0 - a[1] * a[1]) * c;
//    ret[6]= a[1] * a[3] * t - a[0] * s;
//    ret[7]= 0;
//
//    ret[8]= a[0] * a[3] * t - a[1] * s;
//    ret[9]= a[1] * a[3] * t + a[0] * s;
//    ret[10]=a[3] * a[3] + (1.0 - a[3] * a[3]) * c;
//    ret[11]= 0;
//
//    ret[12]= 0;
//    ret[13]= 0;
//    ret[14]= 0;
//    ret[15]= 1;
//
//    return ret;
//}

void Matrix4Mult(const Matrix4& A, const Matrix4& B, Matrix4& ret)
{
    ret[0] = A[0]*B[0] + A[1]*B[1] + A[2]*B[2]+ A[3]*B[3];
    ret[1] = A[4]*B[0] + A[5]*B[1] + A[6]*B[2]+ A[7]*B[3];
    ret[2] = A[8]*B[0] + A[9]*B[1] + A[10]*B[2]+ A[11]*B[3];
    ret[3] = A[12]*B[0] + A[13]*B[1] + A[14]*B[2]+ A[15]*B[3];

    ret[4] = A[0]*B[4] + A[1]*B[5] + A[2]*B[6]+ A[3]*B[7];
    ret[5] = A[4]*B[4] + A[5]*B[5] + A[6]*B[6]+ A[7]*B[7];
    ret[6] = A[8]*B[4] + A[9]*B[5] + A[10]*B[6]+ A[11]*B[7];
    ret[7] = A[12]*B[4] + A[13]*B[5] + A[14]*B[6]+ A[15]*B[7];

    ret[8] = A[0]*B[8] + A[1]*B[9] + A[2]*B[10]+ A[3]*B[11];
    ret[9] = A[4]*B[8] + A[5]*B[9] + A[6]*B[10]+ A[7]*B[11];
    ret[10] = A[8]*B[8] + A[9]*B[9] + A[10]*B[10]+ A[11]*B[11];
    ret[11] = A[12]*B[8] + A[13]*B[9] + A[14]*B[10]+ A[15]*B[11];

    ret[12] = A[0]*B[12] + A[1]*B[13] + A[2]*B[14]+ A[3]*B[15];
    ret[13] = A[4]*B[12] + A[5]*B[13] + A[6]*B[14]+ A[7]*B[15];
    ret[14] = A[8]*B[12] + A[9]*B[13] + A[10]*B[14]+ A[11]*B[15];
    ret[15] = A[12]*B[12] + A[13]*B[13] + A[14]*B[14]+ A[15]*B[15];
}


Real4 Matrix4MultReal4(const Matrix4& A, const Real4& x) {
    Real4 ret;
    ret[0] = A[0]*x[0] + A[1]*x[1] + A[2]*x[2]+ A[3]*x[3];
    ret[1] = A[4]*x[0] + A[5]*x[1] + A[6]*x[2]+ A[7]*x[3];
    ret[2] = A[8]*x[0] + A[9]*x[1] + A[10]*x[2]+ A[11]*x[3];
    ret[3] = A[12]*x[0] + A[13]*x[1] + A[14]*x[2]+ A[15]*x[3];
    return ret;
}


Real3 Matrix4AffineReal3(const Matrix4& A, const Real3& x) {
    Real3 ret;
    ret[0] = A[0]*x[0] + A[1]*x[1] + A[2]*x[2]+ A[3];
    ret[1] = A[4]*x[0] + A[5]*x[1] + A[6]*x[2]+ A[7];
    ret[2] = A[8]*x[0] + A[9]*x[1] + A[10]*x[2]+ A[11];
    return ret;
}

Real3 Matrix4RotateReal3(const Matrix4& A, const Real3& x) {
    Real3 ret;
    ret[0] = A[0]*x[0] + A[1]*x[1] + A[2]*x[2];
    ret[1] = A[4]*x[0] + A[5]*x[1] + A[6]*x[2];
    ret[2] = A[8]*x[0] + A[9]*x[1] + A[10]*x[2];
    return ret;
}

Real3 GetXVector(const Matrix4& A) {
    Real3 ret;
    ret[0]= A[0];
    ret[1]= A[4];
    ret[2]= A[8];
    return ret;
}

Real3 GetYVector(const Matrix4& A) {
    Real3 ret;
    ret[0]= A[1];
    ret[1]= A[5];
    ret[2]= A[9];
    return ret;
}

Real3 GetZVector(const Matrix4& A) {
    Real3 ret;
    ret[0]= A[2];
    ret[1]= A[3];
    ret[2]= A[4];
    return ret;
}

Real3 GetTranslation(const Matrix4& A) {
    Real3 ret;
    ret[0]= A[3];
    ret[1]= A[7];
    ret[2]= A[11];
    return ret;
}

void SetTranslation(Matrix4& A, const Real3& t) {
    A[3]= t[0];
    A[7]= t[1];
    A[11]= t[2];
}
