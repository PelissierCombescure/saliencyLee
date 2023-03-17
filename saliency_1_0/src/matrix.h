/****************************************************************************\
 Copyright 2005 The University of Maryland at College Park.
All Rights Reserved.

Permission to use, copy, modify and distribute this software and its documentation for 
educational, research, and non-profit purposes, without fee, and without a written 
agreement is hereby granted, provided that the above copyright notice and the following 
three paragraphs appear in all copies. 

IN NO EVENT SHALL THE UNIVERSITY OF MARYLAND AT COLLEGE PARK BE 
LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR 
CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF 
THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY 
OF MARYLAND HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES. 

Permission to use, copy, modify and distribute this software and its documentation for 
educational, research, and non-profit purposes, without fee, and without a written 
agreement is hereby granted, provided that the above copyright notice and the following 
paragraphs appear in all copies.

THE UNIVERSITY OF MARYLAND SPECIFICALLY DISCLAIM ANY WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE 
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF 
MARYLAND HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, 
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

Permission to license this software for commercial use can be obtained for a nominal fee by contacting the authors at:

Chang Ha Lee, Amitabh Varshney, and David Jacobs 
Department of Computer Science 
A.V.Williams Building, Room 4406 
University of Maryland 
College Park, MD 20742, USA 

Email: (chlee|varshney|djacobs)@cs.umd.edu



\****************************************************************************/
#ifndef _MATRIX_H
#define _MATRIX_H


//----------------------------------------------------
//			Funcstions
//----------------------------------------------------
void mPrintfM4(char *filename, float m[4][4]);
void mReadfM4(char *filename, float m[4][4]);

//================== 3x3
void mPrintM3(float m[3][3], char *str);
void mSetIdentityM3(float m[3][3]);
void mClearM3(float m[3][3]);
void mAddM3(float m1[3][3], float m2[3][3]);
void mAddM3(float res[3][3], float m1[3][3], float m2[3][3]);
void mSubtractM3(float res[3][3], float m1[3][3], float m2[3][3]);
float mDeterminantM3(float m[3][3]);
int mInverseM3(float res[3][3], float m[3][3]);
void mMultScalarM3(float m[3][3], float val);
void mOuterProductM3(float m[3][3], float v1[3], float v2[3]);

// res = m*v: multiply a vector [v] to a matrix [m] and store the result in [res]
void mMultM3V3(float res[3], float m[3][3], float v[3]);

// res = v*m: multiply a matrix [m] to a vector [v] and store the result in [res]
void mMultV3M3(float res[3], float v[3], float m[3][3]);

// multiply matrix [m1] and [m2]: res = m1*m2
void mMultM3M3(float res[3][3], float m1[3][3], float m2[3][3]);

void mCovarianceM3(float m[3][3], float a[3], float b[3]);


//================== 4x4
void mPrintM4(float m[4][4], char *str);

// Set a identity matrix to the matrix [m]
void mSetIdentityM4(float m[4][4]);

// copy the matrix [m] into [res]
void mCopyM4(float res[4][4], float m[4][4]);

// copy the 3x3 matrix [m] into 4x4 matrix [res]
void mCopyM4M3(float res[4][4], float m[3][3]);

// copy the 4x4 matrix [m] into 3x3 matrix [res]. discard the rest
void mCopyM3M4(float res[3][3], float m[4][4]);

// transpose the matrix [m] and store the result in [res]
void mTransposeM4(float res[4][4], float m[4][4]);

// compute the inverse matrix when the matrix [m] is similarity matrix
void mInverseSimM4gl(float res[4][4], float m[4][4]);

// multiply a vector [v] to a matrix [m] and store the result in [res]
void mMultM4V4(float res[4], float m[4][4], float v[4]);

// multiply a matrix [m] to a vector [v] and store the result in [res]
void mMultV4M4(float res[4], float v[4], float m[4][4]);

// multiply matrix [m1] and [m2]: res = m1*m2
void mMultM4M4(float res[4][4], float m1[4][4], float m2[4][4]);

// multiply m2*m1 and store in [m1]: m1 = m2*m1
void mLeftMultM4M4(float m1[4][4], float m2[4][4]);

// multiply m1*m2 and store in [m1]: m1 = m1*m2
void mRightMultM4M4(float m1[4][4], float m2[4][4]);

// Rotate
void getRotXM4(float m[4][4], float theta);
void getRotYM4(float m[4][4], float theta);
void getRotZM4(float m[4][4], float theta);

void mRotV4toV4(float R[4][4], Tvector4 v1, Tvector4 v2);
void mRotAroundAxis(float R[4][4], Tvector4 axis, float dAlpha);


#endif