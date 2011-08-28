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

Matrix3 Identity3() {
    Matrix3 ret;
    ret[0] = ret[4] = ret[8] = 1;
    return ret;
}

Matrix4 Identity4() {
    Matrix4 ret;
    ret[0] = ret[5] = ret[10] = ret[15] = 1 ;
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


// column major

Matrix4 Rotation(const real angle, const Real3& axis)
{
    Real3 a =normalize( axis );
    real s = sinf(angle);
    real c = cosf(angle);
    real t = 1.0-c;

    Matrix4 ret(Identity4());
    ret[0]= a[0] * a[0] + (1.0 - a[0] * a[0]) * c;
    ret[4]= a[0] * a[1] * t - a[3] * s;
    ret[8]= a[0] * a[3] * t + a[1] * s;

    ret[1]= a[0] * a[1] * t + a[3] * s;
    ret[5]= a[1] * a[1] + (1.0 - a[1] * a[1]) * c;
    ret[9]= a[1] * a[3] * t - a[0] * s;

    ret[2]= a[0] * a[3] * t - a[1] * s;
    ret[6]= a[1] * a[3] * t + a[0] * s;
    ret[10]=a[3] * a[3] + (1.0 - a[3] * a[3]) * c;

    return ret;
}

Matrix4 RotationX(const real angle) {
    real c = cosf(angle);
    real s = sinf(angle);
    
    Matrix4 ret(Identity4());
    ret[5]=c;ret[9]=-s;
    ret[6]=s;ret[10]=c;
    
    return ret;
}

Matrix4 RotationY(const real angle) {
    real s = sinf(angle);
    real c = cosf(angle);
    
    Matrix4 ret(Identity4());
    ret[0]=ret[10]=c;
    ret[2]=-s;ret[8]=s;
    
    return ret;
}

Matrix4 RotationZ(const real angle) {
    real s = sinf(angle);
    real c = cosf(angle);
    
    Matrix4 ret(Identity4());
    ret[0]=ret[5]=c;
    ret[1]=s;ret[4]=-s;
    
    return ret;
}

void Matrix4Mult(const Matrix4& A, const Matrix4& B, Matrix4& ret)
{
    // fuck the cache
    ret[0] = A[0]*B[0] + A[4]*B[1] + A[8]*B[2]+ A[12]*B[3];
    ret[4] = A[0]*B[4] + A[4]*B[5] + A[8]*B[6]+ A[12]*B[7];
    ret[8] = A[0]*B[8] + A[4]*B[9] + A[8]*B[10]+ A[12]*B[11];
    ret[12] = A[0]*B[12] + A[4]*B[13] + A[8]*B[14]+ A[12]*B[15];

    ret[1] = A[1]*B[0] + A[5]*B[1] + A[9]*B[2]+ A[13]*B[3];
    ret[5] = A[1]*B[4] + A[5]*B[5] + A[9]*B[6]+ A[13]*B[7];
    ret[9] = A[1]*B[8] + A[5]*B[9] + A[9]*B[10]+ A[13]*B[11];
    ret[13] = A[1]*B[12] + A[5]*B[13] + A[9]*B[14]+ A[13]*B[15];    
    
    ret[2] = A[2]*B[0] + A[6]*B[1] + A[10]*B[2]+ A[14]*B[3];
    ret[6] = A[2]*B[4] + A[6]*B[5] + A[10]*B[6]+ A[14]*B[7];
    ret[10] = A[2]*B[8] + A[6]*B[9] + A[10]*B[10]+ A[14]*B[11];
    ret[14] = A[2]*B[12] + A[6]*B[13] + A[10]*B[14]+ A[14]*B[15];
    
    ret[3] = A[3]*B[0] + A[7]*B[1] + A[11]*B[2]+ A[15]*B[3];
    ret[7] = A[3]*B[4] + A[7]*B[5] + A[11]*B[6]+ A[15]*B[7];
    ret[11] = A[3]*B[8] + A[7]*B[9] + A[11]*B[10]+ A[15]*B[11];
    ret[15] = A[3]*B[12] + A[7]*B[13] + A[11]*B[14]+ A[15]*B[15];
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


Matrix4 OrthoInverse(const Matrix4& A) {
// la inversa de matrices de transformacion 
// La idea es: 
// si la matriz de transformacion primero rota y luego traslada 
// la inversa primero hace traslacion inversa, luego rotacion transpuesta 
// equivalente a TxR = RrT*trInv
// donde RrT = [Rr' zeros(3,1);zeros(1,3) 1] % y trInv = [eye(3) -tr;zeros(1,3) 1]

    Matrix4 ret=Identity4();
    
    // inverse rotation
    SetCol(ret, 0, Get3Row(A,0));
    SetCol(ret, 1, Get3Row(A,1));
    SetCol(ret, 2, Get3Row(A,2));

    // inverse translation
    Real3 tr = Matrix4RotateReal3(ret,(-1)*Get3Col(A,3) );
    SetCol(ret,3,tr);
    
    return ret;
}

Matrix4 LookAt(const Real3& pos, const Real3& target, const Real3& up) {
    Matrix4 ret(Identity4()); 
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
