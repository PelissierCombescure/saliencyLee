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
#include <stdio.h>
#include <string.h>
#include "macro.h"
#include "geom.h"
#include "matrix.h"

//----------------------------------------------------
//			Global variables
//----------------------------------------------------

//----------------------------------------------------
//			External variables
//----------------------------------------------------

//----------------------------------------------------
//			External functions
//----------------------------------------------------

//----------------------------------------------------
//			Funcstions
//----------------------------------------------------

//*** Matrix

//================== 3x3

void mPrintM3(float m[3][3], char *str)
{
	char buf[512];
	int i;

	if (str == 0) {
		strcpy(buf, "M");
	} else {
		strcpy(buf, str);
	}
	printf("%s[3][3] =\n", buf);
	for (i=0; i<4; i++) {
		printf("          %.3f, %.3f, %.3f\n", m[i][0], m[i][1], m[i][2]);
	}
}

void mSetIdentityM3(float m[3][3])
{
	int i, j;

	for (i=0; i<3; i++) {
		for (j=0; j<3; j++) {
			m[i][j] = (i==j)? 1.0f:0.0f;
		}
	}
}


void mClearM3(float m[3][3])
{
	int i, j;
	for (i=0; i<3; i++) {
		for (j=0; j<3; j++) {
			m[i][j] = 0;
		}
	}
}

void mMultScalarM3(float m[3][3], float val)
{
	int i, j;
	for (i=0; i<3; i++) {
		for (j=0; j<3; j++) {
			m[i][j] *= val;
		}
	}
}

void mAddM3(float m1[3][3], float m2[3][3])
{
	int i, j;
	for (i=0; i<3; i++) {
		for (j=0; j<3; j++) {
			m1[i][j] += m2[i][j];
		}
	}
}


void mAddM3(float res[3][3], float m1[3][3], float m2[3][3])
{
	int i, j;
	for (i=0; i<3; i++) {
		for (j=0; j<3; j++) {
			res[i][j] = m1[i][j] + m2[i][j];
		}
	}
}

void mSubtractM3(float res[3][3], float m1[3][3], float m2[3][3])
{
	int i, j;
	for (i=0; i<3; i++) {
		for (j=0; j<3; j++) {
			res[i][j] = m1[i][j] - m2[i][j];
		}
	}
}

float mDeterminantM3(float m[3][3])
{
	float det;

	det = m[0][0]*(m[1][1]*m[2][2]-m[1][2]*m[2][1])
		- m[0][1]*(m[1][0]*m[2][2]-m[1][2]*m[2][0])
		+ m[0][2]*(m[1][0]*m[2][1]-m[1][1]*m[2][0]);
	return det;
}



/* compute inverse matrix of matrix [m]
@input:
  m: matrix

@output
  res: matrix
 */
int mInverseM3(float res[3][3], float m[3][3])
{
	float value = 0;
	int i;

	value = m[0][0]*m[1][1]*m[2][2] - m[0][0]*m[2][1]*m[1][2] - m[0][1]*m[1][0]*m[2][2] + m[0][1]*m[2][0]*m[1][2] + m[0][2]*m[1][0]*m[2][1] - m[0][2]*m[2][0]*m[1][1];
	if (value == 0)  return 0;	
	for(i = 0; i<3; i++)
		res[0][i] = res[i][0] = 0.0f;

	res[0][0] = (m[1][1]*m[2][2] - m[1][2]*m[2][1])/value;
	res[1][0] = (m[1][2]*m[2][0] - m[1][0]*m[2][2])/value;
	res[2][0] = (m[1][0]*m[2][1] - m[1][1]*m[2][0])/value;

	res[0][1] = (m[0][2]*m[2][1] - m[0][1]*m[2][2])/value;
	res[1][1] = (m[0][0]*m[2][2] - m[0][2]*m[2][0])/value;
	res[2][1] = (m[0][1]*m[2][0] - m[0][0]*m[2][1])/value;

	res[0][2] = (m[0][1]*m[1][2] - m[0][2]*m[1][1])/value;
	res[1][2] = (m[0][2]*m[1][0] - m[0][0]*m[1][2])/value;
	res[2][2] = (m[0][0]*m[1][1] - m[0][1]*m[1][0])/value;

	return 1;
}


void mOuterProductM3(float m[3][3], float v1[3], float v2[3])
{
	int i, j;
	for (i=0; i<3; i++) {
		for (j=0; j<3; j++) {
			m[i][j] = v1[i]*v2[j];
		}
	}
}


/* multiply a vector [v] to a matrix [m] and store the result in [res]
   res = m*v
@input:
  m, v

@output
  res
 */
void mMultM3V3(float res[3], float m[3][3], float v[3])
{
	int i, j;
	for (i=0 ; i<3 ; i++)  {
		res[i] = 0;
		for (j=0 ; j<3 ; j++)
			res[i] += m[i][j] * v[j];
    }
}


/* multiply a matrix [m] to a vector [v] and store the result in [res]
   res = v*m
@input:
  m, v

@output
  res
 */
void mMultV3M3(float res[3], float v[3], float m[3][3])
{
	int i, j;
	for (i=0 ; i<3 ; i++)  {
		res[i] = 0;
		for (j=0 ; j<3 ; j++)
			res[i] += m[j][i] * v[j];
    }
}


/* multiply matrix [m1] and [m2]: res = m1*m2
@input:
  m1, m2

@output
  res
 */
void mMultM3M3(float res[3][3], float m1[3][3], float m2[3][3])
{
	int i, j, k;

	for (i=0; i<3; i++)  {
		for (j=0; j<3; j++)  {
			res[i][j] = 0.0 ;
			for (k=0; k<3; k++)  res[i][j] += m1[i][k] * m2[k][j];
		}
	}
}



void mCovarianceM3(float m[3][3], float a[3], float b[3])
{
	#define SETIJ(M, i, j, val)  M[i][j] = val 
	#define ROWCOL(i, j)  a[i]*b[j] 
    
	SETIJ(m, 0, 0, ROWCOL(0,0));  SETIJ(m, 0, 1, ROWCOL(0,1));  SETIJ(m, 0, 2, ROWCOL(0,2));
	SETIJ(m, 1, 0, ROWCOL(1,0));  SETIJ(m, 1, 1, ROWCOL(1,1));  SETIJ(m, 1, 2, ROWCOL(1,2));
	SETIJ(m, 2, 0, ROWCOL(2,0));  SETIJ(m, 2, 1, ROWCOL(2,1));  SETIJ(m, 2, 2, ROWCOL(2,2));

    #undef SETIJ
    #undef ROWCOL
}

//================== 4x4

void mPrintM4(float m[4][4], char *str)
{
	char buf[512];
	int i;

	if (str == NULL) {
		strcpy(buf, "M");
	} else {
		strcpy(buf, str);
	}
	printf("%s[4][4] =\n", buf);
	for (i=0; i<4; i++) {
		printf("          %.3f, %.3f, %.3f, %.3f\n", m[i][0], m[i][1], m[i][2], m[i][3]);
	}
}


void mPrintfM4(char *filename, float m[4][4])
{
	FILE *fp;
	char buf[512];
	int i;

	fp = fopen(filename, "wa");
	fprintf(fp, "4 4\n");
	for (i=0; i<4; i++) {
		fprintf(fp, "%f, %f, %f, %f\n", m[i][0], m[i][1], m[i][2], m[i][3]);
	}
	fclose(fp);
}


void mReadfM4(char *filename, float m[4][4])
{
	FILE *fp;
	char buf[512];
	int i, row, col;

	fp = fopen(filename, "r");
	fscanf(fp, "%d %d\n", &row, &col);
	for (i=0; i<4; i++) {
		fscanf(fp, "%f, %f, %f, %f\n", &m[i][0], &m[i][1], &m[i][2], &m[i][3]);
	}
	fclose(fp);
}



/* Set a identity matrix to the matrix [m]
@input:
  m: matrix

@output
  m: matrix
 */
void mSetIdentityM4(float m[4][4])
{
	int i, j;

	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			m[i][j] = (i==j)? 1.0f:0.0f;
		}
	}
}

/* copy the matrix [m] into [res]
@input:
  m

@output
  res
 */
void mCopyM4(float res[4][4], float m[4][4])
{
	int i, j;
	for (i = 0 ; i < 4 ; i++)
		for (j = 0 ; j < 4 ; j++)
			res[i][j] = m[i][j] ;
}

/* copy the 3x3 matrix [m] into 4x4 matrix [res]
@input:
  m

@output
  res
 */
void mCopyM4M3(float res[4][4], float m[3][3])
{
	int i, j;
	for (i=0; i<3; i++)  {
		for (j=0; j<3; j++)  {
			res[i][j] = m[i][j];
		}
	}
	res[0][3] = res[1][3] = res[2][3] = res[3][0] = res[3][1] = res[3][2] = 0;
	res[3][3] = 1;
}

/* copy the 4x4 matrix [m] into 3x3 matrix [res]. discard the rest
@input:
  m

@output
  res
 */
void mCopyM3M4(float res[3][3], float m[4][4])
{
	int i, j;
	for (i=0; i<3; i++)  {
		for (j=0; j<3; j++)  {
			res[i][j] = m[i][j];
		}
	}
}


/* transpose the matrix [m] and store the result in [res]
@input:
  m

@output
  res
 */
void mTransposeM4(float res[4][4], float m[4][4])
{
	float mlocal[4][4];
	int i, j;
	mCopyM4(mlocal, m);
	for (i = 0 ; i < 4 ; i++)
		for (j = 0 ; j < 4 ; j++)
			res[j][i] = mlocal[i][j] ;
}

/* compute the inverse matrix when the matrix [m] is similarity matrix
   res = m^(-1)
   (openGL matrix)
@input:
  m

@output
  res
 */
void mInverseSimM4gl(float res[4][4], float m[4][4])
{
	float m3[3][3], rotate[3][3], translate[4][4];
	float scale[3];
	int i, j;

	mSetIdentityM4(translate);
	for (i=0; i<3; i++) {
		translate[3][i] = -m[3][i];
	}

	mCopyM3M4(m3, m);
	mInverseM3(rotate, m3);

	mCopyM4M3(res, rotate);
	mLeftMultM4M4(res, translate);
}


/* multiply a vector [v] to a matrix [m] and store the result in [res]
   res = m*v
@input:
  m, v

@output
  res
 */
void mMultM4V4(float res[4], float m[4][4], float v[4])
{
	int i, j;
	for (i=0 ; i<4 ; i++)  {
		res[i] = 0;
		for (j=0 ; j<4 ; j++)
			res[i] += m[i][j] * v[j];
    }
}


/* multiply a matrix [m] to a vector [v] and store the result in [res]
   res = v*m
@input:
  m, v

@output
  res
 */
void mMultV4M4(float res[4], float v[4], float m[4][4])
{
	int i, j;
	for (i=0 ; i<4 ; i++)  {
		res[i] = 0;
		for (j=0 ; j<4 ; j++)
			res[i] += m[j][i] * v[j];
    }
}


/* multiply matrix [m1] and [m2]: res = m1*m2
@input:
  m1, m2

@output
  res
 */
void mMultM4M4(float res[4][4], float m1[4][4], float m2[4][4])
{
	int i, j, k;

	for (i=0; i<4; i++)  {
		for (j=0; j<4; j++)  {
			res[i][j] = 0.0 ;
			for (k=0; k<4; k++)  res[i][j] += m1[i][k] * m2[k][j];
		}
	}
}


/* multiply m2*m1 and store in [m1]: m1 = m2*m1
@input:
  m1, m2

@output
  m1
 */
void mLeftMultM4M4(float m1[4][4], float m2[4][4])
{
	float res[4][4];
	int i, j, k;

	for (i=0; i<4; i++)  {
		for (j=0; j<4; j++)  {
			res[i][j] = 0.0 ;
			for (k=0; k<4; k++)  res[i][j] += m2[i][k] * m1[k][j];
		}
	}
	mCopyM4(m1, res);
}

/* multiply m1*m2 and store in [m1]: m1 = m1*m2
@input:
  m1, m2

@output
  m1
 */
void mRightMultM4M4(float m1[4][4], float m2[4][4])
{
	float res[4][4];
	int i, j, k;

	for (i=0; i<4; i++)  {
		for (j=0; j<4; j++)  {
			res[i][j] = 0.0 ;
			for (k=0; k<4; k++)  res[i][j] += m1[i][k] * m2[k][j];
		}
	}
	mCopyM4(m1, res);
}


//------------- Rotate
void getRotXM4(float m[4][4], float theta)
{
	double cost, sint;
	cost = cos(theta);
	sint = sin(theta);

	mSetIdentityM4(m);
	m[1][1] = m[2][2] = cost;
	m[1][2] = -sint;
	m[2][1] = sint;
}

void getRotYM4(float m[4][4], float theta)
{
	double cost, sint;
	cost = cos(theta);
	sint = sin(theta);

	mSetIdentityM4(m);
	m[0][0] = m[2][2] = cost;
	m[0][2] = sint;
	m[2][0] = -sint;
}

void getRotZM4(float m[4][4], float theta)
{
	double cost, sint;
	cost = cos(theta);
	sint = sin(theta);

	mSetIdentityM4(m);
	m[0][0] = m[1][1] = cost;
	m[0][1] = -sint;
	m[1][0] = sint;
}


extern void fromToRotation(float from[3], float to[3], float mtx[3][3]);


void mRotV4toV4(float R[4][4], Tvector4 v1, Tvector4 v2)
{
	float m[3][3];

	fromToRotation(v1, v2, m);
	mCopyM4M3(R, m);
}


void mRotAroundAxis(float R[4][4], Tvector4 axis, float dAlpha)
{
	float mtx[3][3], to[3] = { 0, 0, 1 };
	float Rz[4][4], m[4][4], minv[4][4];

	fromToRotation(axis, to, mtx);
	mCopyM4M3(m, mtx);
	fromToRotation(to, axis, mtx);
	mCopyM4M3(minv, mtx);

	getRotZM4(Rz, -dAlpha);

	mCopyM4(R, minv);
	mRightMultM4M4(R, Rz);
	mRightMultM4M4(R, m);
}


//<=========== Matrix


