/*
 *  vector.cpp
 *  GF
 *
 *  Created by Chema on 10/23/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "vec.h"
#include "matrixInverse.h"


Real2 orthogonal(const Real2& ref) {
    return Real2(-ref[1],ref[0]);
}
DReal2 orthogonal(const DReal2& ref) {
    return DReal2(-ref[1],ref[0]);
}

Matrix3 Matrix3Identity() {
    Matrix3 ret;
    ret[0] = ret[4] = ret[8] = 1;
    return ret;
}

Matrix4 Matrix4Identity() {
    Matrix4 ret;
    ret[0] = ret[5] = ret[10] = ret[15] = 1 ;
    return ret;
}

Matrix3 Matrix3Transpose(const Matrix3& A)
{
	Matrix3 ret;
    
    ret[0] = A[0];
    ret[1] = A[3];
    ret[2] = A[6];
    
    ret[3] = A[1];
    ret[4] = A[4];
    ret[5] = A[7];
    
    ret[6] = A[2];
    ret[7] = A[5];
    ret[8] = A[8];

    return ret;
}

Matrix4 Matrix4Transpose(const Matrix4& A)
{
	Matrix4 ret;

    SetCol(ret,0,GetRow(A, 0));
    SetCol(ret,1,GetRow(A, 1));
    SetCol(ret,2,GetRow(A, 2));
    SetCol(ret,3,GetRow(A, 3));
	
    return ret;
}

Matrix4 Matrix4Scale(const real x, const real y, const real z) {
    Matrix4 ret(Matrix4Identity());
    ret[0] = x;
    ret[5] = y;
    ret[10] = z;
    return ret;
}

// TODO: make column major

//Real4 Matrix4MultReal4(const Matrix4& A, const Real4& x) {
//    Real4 ret;
//    ret[0] = A[0]*x[0] + A[1]*x[1] + A[2]*x[2]+ A[3]*x[3];
//    ret[1] = A[4]*x[0] + A[5]*x[1] + A[6]*x[2]+ A[7]*x[3];
//    ret[2] = A[8]*x[0] + A[9]*x[1] + A[10]*x[2]+ A[11]*x[3];
//    ret[3] = A[12]*x[0] + A[13]*x[1] + A[14]*x[2]+ A[15]*x[3];
//    return ret;
//}
//
//
//Real3 Matrix4AffineReal3(const Matrix4& A, const Real3& x) {
//    Real3 ret;
//    ret[0] = A[0]*x[0] + A[1]*x[1] + A[2]*x[2]+ A[3];
//    ret[1] = A[4]*x[0] + A[5]*x[1] + A[6]*x[2]+ A[7];
//    ret[2] = A[8]*x[0] + A[9]*x[1] + A[10]*x[2]+ A[11];
//    return ret;
//}


Real3 Matrix4AffineReal3(const Matrix4& A, const Real3& x) {
    Real3 ret;
    ret[0] = A[0]*x[0] + A[4]*x[1] + A[8]*x[2]+ A[12];
    ret[1] = A[1]*x[0] + A[5]*x[1] + A[9]*x[2]+ A[13];
    ret[2] = A[2]*x[0] + A[6]*x[1] + A[10]*x[2]+ A[14];
    return ret;
}

// column major
Matrix4 Rotation(const real angle, const Real3& axis)
{
    Real3 u =normalize( axis );
    real s = sinf(angle);
    real c = cosf(angle);
    real t = 1.0-c;
    real ux = u[0];
    real uy = u[1];
    real uz = u[2];

    Matrix4 ret(Matrix4Identity());
    ret[0]= c+ux*ux*t;
    ret[1]= uy*ux*t+uz*s;
    ret[2]=uz*ux*t-uy*s; 
    
    ret[4]= ux*uy*t-uz*s;
    ret[5]= c+uy*uy*t;
    ret[6]=uz*uy*t+ux*s; 
    
    ret[8]= ux*uz*t+uy*s;
    ret[9]= uy*uz*t-ux*s;
    ret[10]=c+uz*uz*t;
    
    return ret;
}

Matrix4 RotationX(const real angle) {
    real c = cosf(angle);
    real s = sinf(angle);
    
    Matrix4 ret(Matrix4Identity());
    ret[5]=c;ret[9]=-s;
    ret[6]=s;ret[10]=c;
    
    return ret;
}

Matrix4 RotationY(const real angle) {
    real s = sinf(angle);
    real c = cosf(angle);
    
    Matrix4 ret(Matrix4Identity());
    ret[0]=ret[10]=c;
    ret[2]=-s;ret[8]=s;
    
    return ret;
}

Matrix4 RotationZ(const real angle) {
    real s = sinf(angle);
    real c = cosf(angle);
    
    Matrix4 ret(Matrix4Identity());
    ret[0]=ret[5]=c;
    ret[1]=s;ret[4]=-s;
    
    return ret;
}

void Matrix4Mult(Matrix4& ret, const Matrix4& A, const Matrix4& B)
{
    // [ 0 4  8 12 ]   [ 0 4  8 12 ]
	// [ 1 5  9 13 ] x [ 1 5  9 13 ]
	// [ 2 6 10 14 ]   [ 2 6 10 14 ]
	// [ 3 7 11 15 ]   [ 3 7 11 15 ]
    Matrix4 cret;
    // cache friendly reordering
    cret[0] = A[0]*B[0] + A[4]*B[1] + A[8]*B[2]+ A[12]*B[3];
    cret[1] = A[1]*B[0] + A[5]*B[1] + A[9]*B[2]+ A[13]*B[3];
    cret[2] = A[2]*B[0] + A[6]*B[1] + A[10]*B[2]+ A[14]*B[3];
    cret[3] = A[3]*B[0] + A[7]*B[1] + A[11]*B[2]+ A[15]*B[3];

    cret[4] = A[0]*B[4] + A[4]*B[5] + A[8]*B[6]+ A[12]*B[7];
    cret[5] = A[1]*B[4] + A[5]*B[5] + A[9]*B[6]+ A[13]*B[7];
    cret[6] = A[2]*B[4] + A[6]*B[5] + A[10]*B[6]+ A[14]*B[7];
    cret[7] = A[3]*B[4] + A[7]*B[5] + A[11]*B[6]+ A[15]*B[7];

    cret[8] = A[0]*B[8] + A[4]*B[9] + A[8]*B[10]+ A[12]*B[11];
    cret[9] = A[1]*B[8] + A[5]*B[9] + A[9]*B[10]+ A[13]*B[11];
    cret[10] = A[2]*B[8] + A[6]*B[9] + A[10]*B[10]+ A[14]*B[11];
    cret[11] = A[3]*B[8] + A[7]*B[9] + A[11]*B[10]+ A[15]*B[11];

    cret[12] = A[0]*B[12] + A[4]*B[13] + A[8]*B[14]+ A[12]*B[15];
    cret[13] = A[1]*B[12] + A[5]*B[13] + A[9]*B[14]+ A[13]*B[15];    
    cret[14] = A[2]*B[12] + A[6]*B[13] + A[10]*B[14]+ A[14]*B[15];
    cret[15] = A[3]*B[12] + A[7]*B[13] + A[11]*B[14]+ A[15]*B[15];
    
    std::swap(ret,cret);
}

Real3 Matrix4RotateReal3(const Matrix4& A, const Real3& x) {
    Real3 ret;
    ret[0] = A[0]*x[0] + A[4]*x[1] + A[8]*x[2];
    ret[1] = A[1]*x[0] + A[5]*x[1] + A[9]*x[2];
    ret[2] = A[2]*x[0] + A[6]*x[1] + A[10]*x[2];
    return ret;
}


Real3 GetTranslation(const Matrix4& A) {
    Real3 ret;
    ret[0]= A[3];
    ret[1]= A[7];
    ret[2]= A[11];
    return ret;
}


void SetRow(Matrix4& A, int row, const Real3& t) {
    A[0+row]= t[0]; A[4+row]= t[1]; A[8+row]= t[2];
}

void SetCol(Matrix4& A, int col, const Real3& t) {
    col = col*4;
    A[0+col]= t[0]; A[1+col]= t[1]; A[2+col]= t[2];
}

void SetRow(Matrix4& A, int row, const Real4& t) {
    A[0+row]= t[0]; A[4+row]= t[1]; A[8+row]= t[2]; A[12+row]= t[3];
}

void SetCol(Matrix4& A, int col, const Real4& t) {
    col = col*4;
    A[0+col]= t[0]; A[1+col]= t[1]; A[2+col]= t[2]; A[3+col]= t[3];
}

Real4 GetRow(const Matrix4& A, int row) {
    return Real4(A[0+row],A[4+row],A[8+row],A[12+row]) ;
}

Real4 GetCol(const Matrix4& A, int col) {
    col = col*4;
    return Real4(A[0+col],A[1+col],A[2+col],A[3+col]) ;
}

Real3 Get3Row(const Matrix4& A, int row) {
    return Real3(A[0+row],A[4+row],A[8+row]) ;
}

Real3 Get3Col(const Matrix4& A, int col) {
    col = col*4;
    return Real3(A[0+col],A[1+col],A[2+col]) ;
}

Matrix3 Matrix4GetRotation(const Matrix4& A) {
    Matrix3 ret;
    ret[0] = A[0];
    ret[1] = A[1];
    ret[2] = A[2];

    ret[3] = A[4];
    ret[4] = A[5];
    ret[5] = A[6];
    
    ret[6] = A[8];
    ret[7] = A[9];
    ret[8] = A[10];
    
    return ret;
}

Matrix4 Matrix4OrthoInverse(const Matrix4& A) {
// la inversa de matrices de transformacion 
// La idea es: 
// si la matriz de transformacion primero rota y luego traslada 
// la inversa primero hace traslacion inversa, luego rotacion transpuesta 
// equivalente a TxR = RrT*trInv
// donde RrT = [Rr' zeros(3,1);zeros(1,3) 1] % y trInv = [eye(3) -tr;zeros(1,3) 1]

    Matrix4 ret=Matrix4Identity();
    
    // inverse rotation
    SetCol(ret, 0, Get3Row(A,0));
    SetCol(ret, 1, Get3Row(A,1));
    SetCol(ret, 2, Get3Row(A,2));

    // inverse translation
    Real3 tr = Matrix4RotateReal3(ret,(-1)*Get3Col(A,3) );
    SetCol(ret,3,tr);
    
    return ret;
}

int Matrix4Inverse(Matrix4& ret, const Matrix4& A) {
    return MatrixInverse(ret, A);
}

int Matrix3Inverse(Matrix3& ret, const Matrix3& A) {
    return MatrixInverse(ret, A);
}


Matrix4 Matrix4Perspective(float fov, float aspect, float nearZ, float farZ) {
    Matrix4 ret;
    
    real f = static_cast<real>(1.0) / tanf( (fov * (M_PI/180)) / 2.0f);
    real z = static_cast<real>(0.0);
    real m1 = static_cast<real>(-1.0);
    SetCol(ret, 0, Real4(f/aspect,  z,  z,                      z));
    SetCol(ret, 1, Real4(z,         f,  z,                      z));
    SetCol(ret, 2, Real4(z,         z,(farZ+nearZ)/(nearZ-farZ),m1));    
    SetCol(ret, 3, Real4(z,         z,2*farZ*nearZ/(nearZ-farZ),z));
    return ret;
}

Matrix4 Matrix4LookAt(const Real3& pos, const Real3& target, const Real3& up) {
    Matrix4 ret(Matrix4Identity()); 
    Real3 z=normalize(target-pos);
    Real3 x=normalize(crossProd(z,up));
    Real3 y=crossProd(x,z);
    
    // camera rotation
    SetRow(ret,0,x);
    SetRow(ret,1,y);
    SetRow(ret,2,(-1.f)*z);

    // camera translation
    Real3 tr = Matrix4RotateReal3(ret,(-1)*pos);
    SetCol(ret,3,tr);

    return ret;
}


      