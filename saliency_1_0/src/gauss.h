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
#ifndef _GAUSS_H
#define _GAUSS_H

#define TABLE_SIZE  31
#define G_TABLE_RANGE  3

//----  Data Structures
typedef struct {
	float *val;
	int nval;
	float minv, maxv;
	float interval;
} SlookupTable;


//----  Global Variables

#ifndef _GAUSS_GLOBAL_DEFINE
#define _GAUSS_EXTERN extern
#else
#define _GAUSS_EXTERN
#endif


_GAUSS_EXTERN SlookupTable glGaussTable;
_GAUSS_EXTERN SlookupTable glGaussSqrTable;


#ifdef _GAUSS_GLOBAL_DEFINE
// definition

_GAUSS_EXTERN int glGaussTableSize = TABLE_SIZE;

#else
// declaration

_GAUSS_EXTERN int glGaussTableSize;

#endif

#undef _GAUSS_EXTERN
#undef _GAUSS_GLOBAL_DEFINE


//-------- Functions
void buildGaussTable(SlookupTable *table, int num);
void buildGaussTableSqr(SlookupTable *table, int num);

float gaussDirect(float dist, float var, int dim);
float gaussFromTable(float dist, float var);
float gaussFromTable(float normdist);
float gaussFromSqrTable(SlookupTable *table, float normdist);

//-------- inline Functions

inline float gauss(float dist, float var, int dim)
{
	return gaussDirect(dist, var, dim);
	//return gaussFromTable(dist, var);
}

inline float gauss(float dist, float var)
{
	//return gaussDirect(dist, var, dim);
	return gaussFromTable(dist, var);
}


inline float gauss(float normdist)
{
	//return gaussDirect(dist, var, dim);
	return gaussFromTable(normdist);
}


inline float gauss(SlookupTable *table, float normdistsqr)
{
	return gaussFromSqrTable(table, normdistsqr);
}

#endif