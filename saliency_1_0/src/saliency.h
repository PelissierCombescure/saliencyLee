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
#ifndef _SALIENCY_H
#define _SALIENCY_H

#define DEFAULT_CUT_PERCENTAGE	0.3

//----  Data Structures

typedef struct {
	float val[2];
} Sfloat2;

typedef struct {
	float gaussian[2];
	float rawSaliency;
} Ssaliency;

typedef struct {
	float var[2];
	int nlist;

	Ssaliency *list;
	float minRawSal;
	float maxRawSal;
} SsaliencyList;



typedef struct {
	Sfloat2 *gaussian;
	float *rawSaliency;
} SMsaliency;


typedef struct {
	Sfloat2 *var;
	int nhier;
	int nlist;

	SMsaliency *list;
	float *minRawSal;
	float *maxRawSal;
	float minsal;
	float maxsal;
} SMsaliencyList;


//----  Global Variables

#ifndef _SALIENCY_GLOBAL_DEFINE
#define _SALIENCY_EXTERN extern
#else
#define _SALIENCY_EXTERN
#endif


_SALIENCY_EXTERN SsaliencyList glSaliency;
_SALIENCY_EXTERN SMsaliencyList glMsaliency;
_SALIENCY_EXTERN Sfloat2 *glMgaussianVar;

#ifdef _SALIENCY_GLOBAL_DEFINE

_SALIENCY_EXTERN float glGaussianVar[2] = { 1, 2 };
_SALIENCY_EXTERN float glStartMvar = 2;
_SALIENCY_EXTERN float glMvarInc = 1;
_SALIENCY_EXTERN int glNumMultiSaliency = 5;
_SALIENCY_EXTERN bool useAreaForSaliency = true;
_SALIENCY_EXTERN bool useAmplification = false;
_SALIENCY_EXTERN bool useHierarchicalScales = false;
_SALIENCY_EXTERN float glUnitRatio = 0.003;  // unit ratio relative the diagonal of bounding box
_SALIENCY_EXTERN float glCenterSurroundFactor = 2;

_SALIENCY_EXTERN float glSmoothSize = 1;
_SALIENCY_EXTERN float glCutPercentage = DEFAULT_CUT_PERCENTAGE;
_SALIENCY_EXTERN float glAmplifyTopValue = 100;

#else

_SALIENCY_EXTERN float glGaussianVar[2];
_SALIENCY_EXTERN float glStartMvar;
_SALIENCY_EXTERN float glMvarInc;
_SALIENCY_EXTERN int glNumMultiSaliency;
_SALIENCY_EXTERN bool useAreaForSaliency;
_SALIENCY_EXTERN bool useAmplification;
_SALIENCY_EXTERN bool useHierarchicalScales;
_SALIENCY_EXTERN float glUnitRatio;
_SALIENCY_EXTERN float glCenterSurroundFactor;

_SALIENCY_EXTERN float glSmoothSize;
_SALIENCY_EXTERN float glCutPercentage;
_SALIENCY_EXTERN float glAmplifyTopValue;

#endif

#undef _SALIENCY_EXTERN
#undef _SALIENCY_GLOBAL_DEFINE


//-------- Functions
void initSsaliencyList(SsaliencyList *sal, Smesh *mesh);
void computeSaliency(Smesh *mesh, SsaliencyList *sal);
int readSaliency(char *filename, Smesh *mesh, SsaliencyList *sal);
int writeSaliency(char *filename, Smesh *mesh, SsaliencyList *sal);

int readMeshSaliency(char *filename, Smesh *mesh);
int writeMeshSaliency(char *filename, Smesh *mesh, SMsaliencyList *msal);

void smoothMeshSaliency(Smesh *mesh, float stddev);

void initSMsaliencyList(SMsaliencyList *sal, Smesh *mesh);
void getMultiSaliencyFromGaussians(Smesh *mesh, SMsaliencyList *msal);
void getSingleSaliencyFromGaussians(Smesh *mesh, SMsaliencyList *msal, int level);
// assume that the maximum and minimum of saliency is stored at msal->minsal and msal->maxsal
// call getMultiSaliencyFromGaussians before calling this function
void normalizeMeshSaliency(Smesh *mesh, SMsaliencyList *msal);
void amplification(Smesh *mesh, SMsaliencyList *msal);

float getSaliencyUnit(Smesh *mesh);

void getVertNeighborsGrid(Svertex ***nei, int *nnei, Svertex *vert, Smesh *mesh, float var);
float getVertNeighborsGridCombine(Svertex *vert, Smesh *mesh, float oneovervarsqr[2], float r[2], float rsqr[2], float maxr[2], float gaussianlist[2]);


//-------- inline Functions

inline float computeMeanCurvature(Smesh *mesh, int vid)
{
	return AVG2V(mesh->curvatures[vid].coord);
}

inline float computeRawSaliency(float gaussval[2])
{
	return ABS( (gaussval[0]) - (gaussval[1]) );
}


inline void getVertNeighbors(Svertex ***nei, int *nnei, Svertex *vert, Smesh *mesh, float var)
{
	getVertNeighborsGrid(nei, nnei, vert, mesh, var);
}



#endif