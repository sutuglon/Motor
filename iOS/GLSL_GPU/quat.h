/*
 *  quat.h
 *  GLSL Demo
 *
 *  Created by Chema on 2/21/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 
 * Derived FROM: Quaternion class http://willperone.net/Code/quaternion.php
 * By Will Perone
 * Original: 12-09-2003  
 * Revised:  27-09-2003
 *           22-11-2003
 *           10-12-2003
 *           15-01-2004
 *           16-04-2004
 * 
 * Notes:  
 * if |q|=1 then q is a unit Quaternion
 * if q=(0,v_) then q is a pure Quaternion 
 * if |q|=1 then q conjugate = q inverse
 * if |q|=1 then q= [cos(angle), u*sin(angle)] where u is a unit vector 
 * q and -q represent the same rotation 
 * q*q.conjugate = (q.length_squared, 0) 
 * ln(cos(theta),sin(theta)*v_)= ln(e^(theta*v_))= (0, theta*v_)
 ****************************************/

#ifndef Quat_H
#define Quat_H

#include "vec.h" 
//#include "assert.h" 

template<typename T>
struct Quaternion
{
//	union {
//		struct { // raw elements of the Quaternion
//            T x_[4]; 
//        };
//        struct { // the real + the imaginary components
			T    s_; 
			Vec<T,3> v_; 
//		};
//	};

	Quaternion() {}
	Quaternion(T real, T x, T y, T z): s_(real), v_(x,y,z) {}
	Quaternion(T real, const Vec<T,3> &i): s_(real), v_(i) {}

//	Quaternion &operator =(const Quaternion &q)		
//	{ s_= q.s_; v_= q.v_; return *this; }

//    T& operator[](unsigned i) {return x_[i];} 
//    const T& operator[](unsigned i) const {return x_[i];}
	const Quaternion operator +(const Quaternion &q) const	{ return Quaternion(s_+q.s_, v_+q.v_); }
	const Quaternion operator -(const Quaternion &q) const	{ return Quaternion(s_-q.s_, v_-q.v_); }
	const Quaternion operator -() const { return Quaternion(-s_, -v_); }    
	const Quaternion operator *(const Quaternion &q) const {
        return Quaternion( s_*q.s_ - dotProd(v_,q.v_), s_*q.v_+q.s_*v_ + crossProd(v_,q.v_) );
	}
	const Quaternion operator /(const Quaternion &q) const {
        return (*this) * conjugate(q)/q.sqrnorm();
    }

	const Quaternion operator *(T scale) const { return Quaternion(s_*scale,v_*scale); }
	const Quaternion operator /(T scale) const { 
        return (*this)*(static_cast<T>(1)/scale);
    }

	const Quaternion &operator +=(const Quaternion &q) { v_+=q.v_; s_+=q.s_; return *this; }
	const Quaternion &operator -=(const Quaternion &q) { v_-=q.v_; s_-=q.s_; return *this; }
	const Quaternion &operator *=(const Quaternion &q) { return (*this)=(*this)*q; }
	
	const Quaternion &operator *= (T scale)	{ v_*=scale; s_*=scale; return *this; }
	const Quaternion &operator /= (T scale)	{ v_/=scale; s_/=scale; return *this; }
	
	T norm() const { return static_cast<T>(sqrt(s_*s_ + dotProd(v_,v_))); }
	T sqrnorm() const { return static_cast<T>((s_*s_ + dotProd(v_,v_))); }
};

template<typename T>
T dotProd(const Quaternion<T> &q1, const Quaternion<T> &q2) 
{ return q1.v_*q2.v_ + q1.s_*q2.s_; }

template<typename T>
Quaternion<T> lerp(const Quaternion<T> &q1, const Quaternion<T> &q2, T t) 
{ return (q1*(1-t) + q2*t).normalized(); }

//! spherical interpolation
template<typename T>
Quaternion<T> slerp(const Quaternion<T> &q1, const Quaternion<T> &q2, T t, T dot) {
    Quaternion<T> ret;
    if (static_cast<T>(abs(dot)) < static_cast<T>(0.95))
    {
        T angle = static_cast<T>(acos(dot));
        ret = ( q1*static_cast<T>(sin(angle*(1-t))) + q2*static_cast<T>(sin(angle*t)))/
                static_cast<T>(sin(angle));
    } 
    else {
        ret = lerp(q1,q2,t); // use linear interpolation for small angles
    }
    return ret;
}

template<typename T>
Quaternion<T> slerp(const Quaternion<T> &q1, const Quaternion<T> &q2, T t) {
    Quaternion<T> q3;
    T dot = dotProd(q1, q2);// dot = cos(theta), 
    q3 = dot<0?-q2:q2;      // check more than 90 degrees apart
    slerp(q1,q3,t,dot);
}


//! spherical cubic interpolation
template<typename T>
Quaternion<T> squad(const Quaternion<T> &q1,const Quaternion<T> &q2,const Quaternion<T> &a,const Quaternion<T> &b,T t)
{
    Quaternion<T> c= slerp(q1,q2,t,dotProd(q1,q2)),
                  d= slerp(a,b,t,dotProd(a,b));
    return slerp(c,d,2*t*(1-t),dotProd(c,d));
}

//! Shoemake-Bezier interpolation using De Castlejau algorithm
template<typename T>
Quaternion<T> bezier(const Quaternion<T> &q1,const Quaternion<T> &q2,const Quaternion<T> &a,const Quaternion<T> &b,T t)
{
    // level 1
    Quaternion<T> q11= slerp(q1,a,t),
            q12= slerp(a,b,t),
            q13= slerp(b,q2,t);		
    // level 2 and 3
    Quaternion<T> q112 = slerp(q11,q12,t,dotProd(q11,q12));
    Quaternion<T> q123 = slerp(q12,q13,t,dotProd(q12,q13));
    return slerp(q112, q123, t, dotProd(q112,q123));
}

//! Given 3 quaternions, qn-1,qn and qn+1, calculate a control point to be used in spline interpolation
template<typename T>
Quaternion<T> spline(const Quaternion<T> &qnm1,const Quaternion<T> &qn,const Quaternion<T> &qnp1)
{
    Quaternion<T> qni(qn.s_, -qn.v_);	
    return qn * exp(( log(qni*qnm1)+log(qni*qnp1) )/-4);
}

template<typename T>
Vec<T,3> rotate(const Vec<T,3> &v, const Quaternion<T>& q) {   
    Quaternion<T> qv(0, v);
    Quaternion<T> qc=conjugate(q);
    return (q * qv * qc).v_;
}

template<typename T>
Vec<T,16> toMatrix4(const Quaternion<T>& q) {
    //Assert(length() > 0.9999 && length() < 1.0001, "Quaternion is not normalized");		
    Vec<T,16> ret;
    T w=q.s_;
    T x=q.v_[0];
    T y=q.v_[1];
    T z=q.v_[2];

    SetCol(ret,0,Vec<T,4>(1-2*(y*y+z*z),2*(x*y-w*z),  2*(x*z+w*y),0));   
    SetCol(ret,1,Vec<T,4>(2*(x*y+w*z),  1-2*(x*x+z*z),2*(y*z-w*x),0));   
    SetCol(ret,2,Vec<T,4>(2*(x*z-w*y),  2*(y*z+w*x),  1-2*(x*x+y*y),0));
    SetCol(ret,3,Vec<T,4>(0,0,0,1));
    return ret;
}

//! logarithm of a Quaternion = v_*a where q = [cos(a),v_*sin(a)]
template<typename T>
Quaternion<T> log(const Quaternion<T>& q) {
    T a = static_cast<T>(acos(q.s_));
    T sina = static_cast<T>(sin(a));
    Quaternion<T> ret;

    ret.s_ = 0;
    if (sina > 0)
    {
        ret.v_[0] = a*q.v_[0]/sina;
        ret.v_[1] = a*q.v_[1]/sina;
        ret.v_[2] = a*q.v_[2]/sina;
    } else {
        ret.v_[0]= ret.v_[1]= ret.v_[2]= 0;
    }
    return ret;
}

//! e^Quaternion = exp(v_*a) = [cos(a),vsin(a)]
template<typename T>
Quaternion<T> exp(const Quaternion<T>& q) {
    T a = static_cast<T>(q.v_.length());
    T inva = static_cast<T>(1.)/a;
    T sina = static_cast<T>(sin(a));
    T cosa = static_cast<T>(cos(a));
    
    Quaternion<T> ret;
    ret.s_ = cosa;
    if (a > 0)
    {
        ret.v_[0] = sina*q.v_[0]*inva;
        ret.v_[1] = sina*q.v_[1]*inva;
        ret.v_[2] = sina*q.v_[2]*inva;
    } else {
        ret.v_[0] = ret.v_[1] = ret.v_[2] = 0;
    }
    return ret;
}

template<typename T>
Quaternion<T> normalize(const Quaternion<T>& q) { return q/q.norm(); }
template<typename T>
Quaternion<T> conjugate(const Quaternion<T>& q) { return Quaternion<T>(q.s_,-q.v_); }
template<typename T>
Quaternion<T> invert(const Quaternion<T>& q) { return conjugate(q)/q.sqrnorm(); }

// transform
template<typename T>
Vec<T,3> getEulerAngles(const Quaternion<T>& q) {
    T sqw = q.s_*q.s_;    
    T sqx = q.v_[0]*q.v_[0];    
    T sqy = q.v_[1]*q.v_[1];    
    T sqz = q.v_[2]*q.v_[2];    

    Vec<T,3> euler;
    euler[0] = static_cast<T>(atan2(2.f * (q.v_[0]*q.v_[1] + q.v_[2]*q.s_), sqx - sqy - sqz + sqw));
    euler[1] = static_cast<T>(asin(-2.f * (q.v_[0]*q.v_[2] - q.v_[1]*q.s_)));
    euler[2] = static_cast<T>(atan2(2.f * (q.v_[2]*q.v_[2] + q.v_[0]*q.s_), -sqx - sqy + sqz + sqw));
    return euler;
}

template<typename T>
void getAxisAngle(const Quaternion<T>&q, Vec<T,3> &axis, T &theta) {
    theta = static_cast<T>(acos(q.s_));
    T invsin = static_cast<T>(1.)/static_cast<T>(sin(theta));
    axis[0] = q.v_[0]*invsin;
    axis[1] = q.v_[1]*invsin;
    axis[2] = q.v_[2]*invsin;
    theta*=2;
}

template<typename T>
Quaternion<T> quatFromAxisAngle(const Vec<T,3> &axis, T angle) { 
    T half = angle*0.5;
    return Quaternion<T>(
        static_cast<T>(cos(half)), 
        static_cast<T>(sin(half))*axis); 
}

template<typename T>
Quaternion<T> quatFromVectors(const Vec<T,3>& from, const Vec<T,3>& to)
{
    Quaternion<T> ret;
	T c = dotProd( normalize(from), normalize(to) );
	if ( c < static_cast<T>(1) - static_cast<T>(EPSILON4)) {
		T theta = static_cast<T>(acos(c));
		Vec<T,3> axis = normalize(crossProd(to,from));
		ret= quatFromAxisAngle(axis, theta);
	}else {
  		ret= Quaternion<T>(static_cast<T>(1),Vec<T,3>(0,0,0));
	}
    return ret;
}

template<typename T>
Quaternion<T> quatFromEuler(const Vec<T,3> &angles) {	
    Quaternion<T> ret;
    Vec<T,3> half = 0.5*angles;
    T cosz = static_cast<T>(cos(half[2]));
    T cosy = static_cast<T>(cos(half[1]));
    T cosx = static_cast<T>(cos(half[0]));

    T sinz = static_cast<T>(sin(half[2]));
    T siny = static_cast<T>(sin(half[1]));
    T sinx = static_cast<T>(sin(half[0]));
    
    // and now compute Quaternion
    ret.s_    = cosz*cosy*cosx + sinz*siny*sinx;
    ret.v_[0] = cosz*cosy*sinx - sinz*siny*cosx;
    ret.v_[1] = cosz*siny*cosx + sinz*cosy*sinx;
    ret.v_[2] = sinz*cosz*cosx - cosz*siny*sinx;
    return ret;
} 

#endif