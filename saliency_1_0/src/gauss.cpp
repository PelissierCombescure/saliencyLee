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
#include "macro.h"
#define _GAUSS_GLOBAL_DEFINE
#include "gauss.h"


// 1 stddev 68, 2 stddev 95, 3 stddev 99.7
float gaussDirect(float dist, float var, int dim)
{
	float sigmaSqr = SQR(var);
	float coef = 1;
	float val;
	
	val = coef*exp(-dist*dist/(2*sigmaSqr));
	return val;
}


void buildGaussTable(SlookupTable *table, int num)
{
	table->nval = num;
	if (table->nval % 2 == 0)  table->nval++;
	mymalloc(table->val, float, table->nval);

	table->minv = -G_TABLE_RANGE;
	table->maxv = G_TABLE_RANGE;
	table->interval = (table->maxv - table->minv) / (float)(table->nval - 1);

	for (int i=0; i<table->nval; i++) {
		float val = table->minv + (float)i*table->interval;
		table->val[i] = exp(-val*val*0.5);
	}
}


void buildGaussTableSqr(SlookupTable *table, int num)
{
	table->nval = num;
	if (table->nval % 2 == 0)  table->nval++;
	mymalloc(table->val, float, table->nval);

	table->minv = -SQR( G_TABLE_RANGE );
	table->maxv = SQR( G_TABLE_RANGE );
	table->interval = (table->maxv - table->minv) / (float)(table->nval - 1);

	for (int i=0; i<table->nval; i++) {
		float val = table->minv + (float)i*table->interval;
		table->val[i] = exp(-val*0.5);
	}
}


float gaussFromTable(float dist, float var)
{
	float gval, gdirect;
	SlookupTable *table = &glGaussTable;
	static float oneoverint = 1/table->interval;
	float x = dist/var - table->minv;
	float xdivint = (x) * oneoverint;
	int idx = xdivint;

	if (idx >= table->nval-1)  return table->val[idx];

	x = xdivint - idx; 
	gval = table->val[idx] + x * (table->val[idx+1] - table->val[idx]);

	return gval;
}



float gaussFromTable(float normdist)
{
	float gval, gdirect;
	SlookupTable *table = &glGaussTable;
	static float oneoverint = 1/table->interval;
	float x = normdist - table->minv;
	float xdivint = (x) * oneoverint;
	int idx = xdivint;

	if (idx >= table->nval-1)  return table->val[idx];

	x = xdivint - idx; 
	gval = table->val[idx] + x * (table->val[idx+1] - table->val[idx]);

	return gval;
}



float gaussFromSqrTable(SlookupTable *table, float normdist)
{
	float gval, gdirect;
	static float oneoverint = 1/table->interval;
	float x = normdist - table->minv;
	float xdivint = (x) * oneoverint;
	int idx = xdivint;

	if (idx >= table->nval-1)  return table->val[idx];

	x = xdivint - idx; 
	gval = table->val[idx] + x * (table->val[idx+1] - table->val[idx]);

	return gval;
}


