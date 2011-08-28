/*
 *  vector.h
 *  GF
 *
 *  Created by Chema on 10/23/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef Vector_H
#define Vector_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <assert.h>

typedef float real;
typedef double dreal;
typedef unsigned char ubyte;

const real Pi = 3.14159265;
const real EPSILON3 = 1e-3;
const real EPSILON4 = 1e-4;
const real EPSILON6 = 1e-6;

template<typename T>
T randnorm(void)
{
    return static_cast<T> (rand())/RAND_MAX;
}


// esta plantilla sirve para cualquier vector (de float, de double) de cualquier tamaño (1,2,3)
// p.ej.    Vec<float,2> es un vector de dos componentes float
//          Vec<double,3> es un vector de tres componentes double, etc.
// ver definición de tipos (typedef's) abajo

template<typename T, unsigned S>
struct Vec {
    T x_[S];

    // acceso por componentes
    T& x() {return x_[0];}
    T x() const {return x_[0];}
    T& y()  {return x_[1];}
    T y() const {return x_[1];}
    T& z() {return x_[2];}
    T z() const {return x_[2];}
    T& w() {return x_[3];}
    T w() const {return x_[3];}

    explicit Vec(T x=static_cast<T> (0)){ for(unsigned i=0;i<S;++i) x_[i]=x;}
    Vec(T x, T y) { assert(S>=2); x_[0] = x; x_[1] = y; }
    Vec(T x, T y, T z) {assert(S>=3); x_[0] = x; x_[1] = y; x_[2]= z; }
    Vec(T x, T y, T z, T w) { assert(S>=3); x_[0] = x; x_[1] = y; x_[2]= z; x_[3]= w;}
    explicit Vec(const T* const x){ memcpy(x_, x, S*sizeof(T)); }
    Vec& operator=(const Vec<T,S-1>& ref) { memcpy(x_, ref.x_,(S-1)*sizeof(T)); return *this; }

    // por componentes
    T& operator[](unsigned i) {return x_[i];}
    const T& operator[](unsigned i) const {return x_[i];}


    Vec operator+(const Vec &b) const {
        Vec ret;
        for(unsigned i=0;i<S;++i) ret[i]=x_[i]+b.x_[i];
        return ret;
    }
    Vec& operator+=(const Vec &b) {
        for(unsigned i=0;i<S;++i) x_[i]+=b[i];
        return *this;
    }
	Vec operator -() const { 
        Vec ret;
        for(unsigned i=0;i<S;++i) ret[i]=-x_[i];
        return ret;
    }
    Vec operator-(const Vec &b) const {
        Vec ret;
        for(unsigned i=0;i<S;++i) ret[i]=x_[i]-b.x_[i];
        return ret;
    }
    Vec& operator-=(const Vec &b) {
        for(unsigned i=0;i<S;++i) x_[i]-=b[i];
        return *this;
    }
    Vec operator*(const Vec &b) const {
        Vec ret;
        for(unsigned i=0;i<S;++i) ret[i]=x_[i]*b.x_[i];
        return ret;
    }
    Vec& operator*=(const Vec &b) {
        for(unsigned i=0;i<S;++i) x_[i]*=b[i];
        return *this;
    }

    // con escalar
    Vec& operator*=(const T& r) {
        for(unsigned i=0;i<S;++i) x_[i]*=r;
        return *this;
    }
    Vec operator*(const T& r) const {
        Vec ret(*this);
        ret*=r;
        return ret;
    }
    Vec& operator/=(const T& r) {
        const T inv=static_cast<T>(1.0)/r;
        for(unsigned i=0;i<S;++i) x_[i]*=inv;
        return *this;
    }
    Vec operator/(const T& r) const {
        Vec ret(*this);
        const T inv=static_cast<T>(1.0)/r;
        ret*=inv;
        return ret;
    }
    
    bool operator==(const Vec &ref) const {return memcmp(x_,ref.x_,S*sizeof(T))==0;}
    bool operator!=(const Vec &ref) const {return !(*this==ref);}

    T sqrnorm() const { return dotProd(*this,*this); }
    T norm() const { return sqrt(sqrnorm()); }
    std::ostream& Print(std::ostream& s) const {
        s << "(";
        for(unsigned i=0;i<S-1;++i) {
            s << x_[i] << " ";
        }
        s << x_[S-1];
        s << ")";
        return s;
    }
 };

// normalizar vector
template<typename T, unsigned S>
Vec<T,S> normalize(const Vec<T,S>& ref) { return ref/ref.norm(); }

// producto escalar
template<typename T, unsigned S>
T dotProd(const Vec<T,S> &a, const Vec<T,S> &b)  {
    T ret=static_cast<T> (0);
    for(unsigned i=0;i<S;++i) ret+=a.x_[i]*b.x_[i];
    return ret;
}

// producto vectorial
template<typename T, unsigned S>
Vec<T,S> crossProd(const Vec<T,S>& a, const Vec<T,S>&b)  {
    assert(S>=3);
    return Vec<T,S>(a.x_[1]*b.x_[2]-a.x_[2]*b.x_[1], a.x_[2]*b.x_[0]-a.x_[0]*b.x_[2], a.x_[0]*b.x_[1]-a.x_[1]*b.x_[0]);
}


template<typename T, unsigned S>
Vec<T,S> abs(const Vec<T,S>& ref)  {
    Vec<T,S> ret;
    for(unsigned i=0;i<S;++i) ret[i]=abs(ref.x_[i]);
    return ret;
}

// para que se pueda multiplicar k*vec (vec*k está en el operador* de Vec)
template<typename T, unsigned S>
Vec<T,S> operator*(float landa, const Vec<T,S>& ref) {
    return ref*landa;
}

// para sacar por la consola
template<class T, unsigned S>
std::ostream& operator<<(std::ostream& s, const Vec<T,S>& vec) {
    return vec.Print(s);
}

template<typename T>
Vec<T,2> vecXZ(const Vec<T,3>& v) {
    return Vec<T,2>(v[0],v[2]);
}

template<typename T>
Vec<T,2> vecXY(const Vec<T,3>& v) {
    return Vec<T,2>(v[0],v[1]);
}

// tipos comunes de vectores
typedef Vec<unsigned,2> UInt2;
typedef Vec<unsigned,3> UInt3;
typedef Vec<int,2> Int2;
typedef Vec<int,3> Int3;
typedef Vec<unsigned short,3> UShort3;
typedef Vec<real,2> Real2;
typedef Vec<dreal,2> DReal2;
typedef Vec<dreal,3> DReal3;
typedef Vec<dreal,16> DMatrix4;
typedef Vec<real,2> Interval;
typedef Vec<real,3> Real3;
typedef Vec<real,4> Real4;
typedef Vec<real,16> Matrix4;
typedef Vec<real,9> Matrix3;
typedef Vec<real,4> Matrix2;

typedef Vec<unsigned char,3> UChar3;
typedef UChar3 Color3;

struct Color4: public Vec<unsigned char, 4> {
    Color4 () : Vec<unsigned char,4>(){;}
    Color4 (unsigned char r, unsigned char g, unsigned char b, unsigned char a) : Vec<unsigned char,4>(r,g,b,a){;}
    operator unsigned() const{
        return *const_cast<unsigned*>(reinterpret_cast<const unsigned*> (&x_[0]));
    }
};

//typedef Vec<unsigned char,4> Color4;
typedef Vec<unsigned char,3> Color3;

// caso especial de vectores de 2 dimensiones, devuelve vector perpendicular
Real2 orthogonal(const Real2& ref);
DReal2 orthogonal(const DReal2& ref);

void Matrix4Mult(const Matrix4& A, const Matrix4& B, Matrix4& ret);
Real4 Matrix4MultReal4(const Matrix4& A, const Real4& x);
Real3 Matrix4AffineReal3(const Matrix4& A, const Real3& x);
Real3 Matrix4RotateReal3(const Matrix4& A, const Real3& x);
Matrix4 Rotation(const real angle, const Real3& axis);
Matrix4 RotationX(const real angle);
Matrix4 RotationY(const real angle);
Matrix4 RotationZ(const real angle);
void SetRow(Matrix4& A, int row, const Real3& t);
void SetCol(Matrix4& A, int col, const Real3& t);
void SetRow(Matrix4& A, int row, const Real4& t);
void SetCol(Matrix4& A, int col, const Real4& t);
Real4 GetRow(const Matrix4& A, int row);
Real4 GetCol(const Matrix4& A, int col);
Real3 Get3Row(const Matrix4& A, int row);
Real3 Get3Col(const Matrix4& A, int col);


// inversa de matrix 2x2 homogénea
Matrix3 HInverse(const Matrix3& ref);

Matrix3 Identity3();
Matrix4 Identity4();

Matrix4 LookAt(const Real3& pos, const Real3& target, const Real3& up);
Matrix4 OrthoInverse(const Matrix4& A);

#endif
