//
//  MatrixInverse.h
//  iPhoneGLEssentials
//
//  Created by Chema on 3/24/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "vec.h"

#define MATSWAP(a,b) {temp=(a);(a)=(b);(b)=temp;}
template<typename T, unsigned N>
int MatrixInverse(Vec<T,N>& ret, const Vec<T,N>& A) {
    
    int flag = 1;
    
    int n = sqrt(N);
    float matr[n][n], ident[n][n];
    int icol=0, irow=0;
    int indxc[n], indxr[n], ipiv[n];
    float big, dum, pivinv, temp;
    
    
    for (unsigned i=0; i<n; i++) {
        for (unsigned j=0; j<n; j++) {
            matr[i][j] = A[n*i+j];
            ident[i][j] = 0.0f;
        }
        ident[i][i]=1.0f;
    } 
    
    
    // Gauss-jordan elimination with full pivoting.  
    // from numerical recipies in C second edition, pg 39
    
    for(unsigned j=0;j<n;j++) ipiv[j] = 0;
    for(unsigned i=0;i<n;i++) {
        big=0.0;
        for (unsigned j=0;j<n;j++) {
            if(ipiv[j] != 1) {
                for (unsigned k=0;k<n;k++) {
                    if(ipiv[k] == 0) {
                        if(fabs(matr[j][k]) >= big) {
                            big = (float) fabs(matr[j][k]);
                            irow=j;
                            icol=k;
                        }
                    } 
                    else if (ipiv[k] > 1) return flag;
                } 
            }
        }
        ++(ipiv[icol]);
        if (irow != icol) {
            for (unsigned l=0;l<n;l++) MATSWAP(matr[irow][l],matr[icol][l]);
            for (unsigned l=0;l<n;l++) MATSWAP(ident[irow][l],ident[icol][l]);
        }
        indxr[i]=irow;
        indxc[i]=icol;
        if(matr[icol][icol] == 0.0f) return flag; 
        pivinv = 1.0f / matr[icol][icol];
        matr[icol][icol]=1.0f;
        for (unsigned l=0;l<n;l++) matr[icol][l] *= pivinv;
        for (unsigned l=0;l<n;l++) ident[icol][l] *= pivinv;
        for (unsigned ll=0;ll<n;ll++) {
            if (ll != icol) {
                dum=matr[ll][icol];
                matr[ll][icol]=0.0f;
                for (unsigned l=0;l<n;l++) matr[ll][l] -= matr[icol][l]*dum;
                for (unsigned l=0;l<n;l++) ident[ll][l] -= ident[icol][l]*dum;
            }
        }
    }
    for (int l=n-1;l>=0;l--) {
        if (indxr[l] != indxc[l]) {
            for (unsigned k=0;k<n;k++) {
                MATSWAP(matr[k][indxr[l]],matr[k][indxc[l]]);
            }
        }
    }
    for (unsigned i=0; i<n; i++) 
        for (unsigned j=0; j<n; j++)
            ret[n*i+j] = matr[i][j];
    return flag = 0;
}
