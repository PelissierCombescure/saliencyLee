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
#include <memory.h>
#include <float.h>
#include "timerms.h"
#include "geom.h"
#include "macro.h"
#define _SALIENCY_GLOBAL_DEFINE
#include "saliency.h"
#include "qsort.h"
#include "grid.h"
#include "gauss.h"

//----------------------------------------------------
//			Global variables
//----------------------------------------------------
bool *gVisited = 0;
float *gDistance = 0;
//----------------------------------------------------
//			External variables
//----------------------------------------------------
extern bool useSupress;
extern bool isAllowToReadFromFile;
//----------------------------------------------------
//			External functions
//----------------------------------------------------

//----------------------------------------------------
//			functions
//----------------------------------------------------
int writeMeshSaliencyBin(char *filename, Smesh *mesh);


// find the unit for computing saliency and store at mesh->unit and return it
float getSaliencyUnit(Smesh *mesh)
{
	float unit, diagonal;
	float minx[3], maxx[3];
	int i, j, k;

	for (k=0; k<3; k++) {
		minx[k] = FLT_MAX;
		maxx[k] = -FLT_MAX;
	}
	for (i=0; i<mesh->nverts; i++) {
		for (k=0; k<3; k++) {
			SET_MIN_MAX(minx[k], maxx[k], mesh->verts[i].coords[k]);
		}
	}
	diagonal = sqrt(SQR(maxx[0]-minx[0]) + SQR(maxx[1]-minx[1]) + SQR(maxx[2]-minx[2]));
	unit = mesh->unit = diagonal*glUnitRatio;
	printdg("# unit = %.4f, diagonal = %.3f\n", mesh->unit, diagonal);

	return unit;
}


void initSsaliencyList(SsaliencyList *sal, Smesh *mesh)
{
	int n = mesh->nverts;
	int i;

	sal->nlist = n;
	mymalloc(sal->list, Ssaliency, sal->nlist);
	for (i=0; i<2; i++) {
		sal->var[i] = mesh->unit*glGaussianVar[i];
	}
}




void smoothMeshSaliency(Smesh *mesh, float stddev)
{
	Svertex **verts =0;
	int nverts;
	float gauss, gausssum = 0, sal, var = SQR(stddev);
	
	mymalloc(verts, Svertex *, mesh->nverts);
	for (int i=0; i<mesh->nverts; i++) {
		Svertex *v0 = &(mesh->verts[i]);
		getVertNeighbors(&verts, &nverts, v0, mesh, stddev);
		gausssum = sal = 0;
		for (int j=0; j<nverts; j++) {
			Svertex *v1 = verts[j];
			gauss = exp(-0.5*DIST3VSQR(v0->coords, v1->coords)/var);
			gausssum += gauss;
			sal += gauss*v1->saliency;
		}
		v0->saliency = sal / gausssum;
		printProgress(i, mesh->nverts);
	}
	myfree(verts);
}

void supressSaliency(Smesh *mesh, SsaliencyList *sal)
{
	Svertex *v0, *v1;
	float s0, s1, mins = 0, maxs = 1, meanlmax = 0;
	int nlocalmax = 0;
	bool islocalmax;
	int i, j, k;

	meanlmax = 0;
	nlocalmax = 0;
	for (i=0; i<mesh->nverts; i++) {
		v0 = &(mesh->verts[i]);
		s0 = sal->list[i].rawSaliency; 
		islocalmax = (v0->nedges != 0); //true;
		for (int e=0; e<v0->nedges; e++) {
			v1 = getOtherVertInEdge(v0->edges[e], v0);
			s1 = sal->list[v1->id].rawSaliency; 
			if (s0 < s1) { 
				islocalmax = false;
			}
		}
		if (islocalmax) {
			meanlmax += s0;
			nlocalmax++;
		}
	}
	// exclude global maximum
	nlocalmax--;
	meanlmax -= maxs;
	meanlmax /= (float)nlocalmax;

	float factor = SQR(maxs - meanlmax);
	for (i=0; i<mesh->nverts; i++) {
		s0 = sal->list[i].rawSaliency;
		sal->list[i].rawSaliency = s0*factor;
	}
}


void nonlinearlyNormalizeSaliency(Smesh *mesh, SsaliencyList *sal)
{
	float mins = FLT_MAX, maxs = -FLT_MAX;
	int i;

	if (useSupress) {
		supressSaliency(mesh, sal);
	}
	mins = FLT_MAX;
	maxs = -FLT_MAX;
	for (i=0; i<mesh->nverts; i++) {
		if (!(mesh->curvatures[i].isValid))  continue;
		SET_MIN_MAX(mins, maxs, ABS(sal->list[i].rawSaliency));
	}
	sal->minRawSal = mins;
	sal->maxRawSal = maxs;
}



void getSingleSaliencyFromGaussians(Smesh *mesh, SMsaliencyList *msal, int level)
{
	int i;
	msal->minsal = FLT_MAX;
	msal->maxsal = -FLT_MAX;
	for (i=0; i<msal->nlist; i++) {
		mesh->verts[i].saliency = 0;
		mesh->verts[i].saliency += ABS(msal->list[i].rawSaliency[level]);
		SET_MIN_MAX(msal->minsal, msal->maxsal, mesh->verts[i].saliency);
	}
}


// assume that the maximum and minimum of saliency is stored at msal->minsal and msal->maxsal
// call getMultiSaliencyFromGaussians before calling this function
void amplification(Smesh *mesh, SMsaliencyList *msal)
{
	SfloatSortData *sorted;
	float cutvalue, cutpercentage = glCutPercentage;
	float maxhighvalue = glAmplifyTopValue;
	int i, cutid;

	mymalloc(sorted, SfloatSortData, mesh->nverts+1);
	for (i=0; i<mesh->nverts; i++) {
		sorted[i].id = i;
		sorted[i].val = mesh->verts[i].saliency;
	}
	sortFloatData(sorted, mesh->nverts, Sort_Ascending);
	cutid = sorted[(int)ROUND((1-cutpercentage)*(float)mesh->nverts)].id;
	cutvalue = sorted[(int)ROUND((1-cutpercentage)*(float)mesh->nverts)].val;
	
	for (i=0; i<mesh->nverts; i++) {
		float salval = mesh->verts[i].saliency;
		if (salval > cutvalue) {
			salval *= maxhighvalue;;
		}
		mesh->verts[i].saliency = salval; 
	}

	msal->minsal = FLT_MAX;
	msal->maxsal = -FLT_MAX;
	for (i=0; i<mesh->nverts; i++) {
		SET_MIN_MAX(msal->minsal, msal->maxsal, mesh->verts[i].saliency);
	}
	
	free(sorted);
}

// assume that the maximum and minimum of saliency is stored at msal->minsal and msal->maxsal
// call getMultiSaliencyFromGaussians before calling this function
void normalizeMeshSaliency(Smesh *mesh, SMsaliencyList *msal)
{
	for (int i=0; i<mesh->nverts; i++) {
		mesh->verts[i].saliency = (mesh->verts[i].saliency - msal->minsal) / (msal->maxsal - msal->minsal);
	}
	msal->minsal = 0;
	msal->maxsal = 1;
}



void getMultiSaliencyFromGaussians(Smesh *mesh, SMsaliencyList *msal)
{
	float r = glSmoothSize;
	int i, j;
	char filename[512];
	if (useHierarchicalScales) {
		sprintf(filename, "%s_u_raw%.1f", mesh->name, r);
	} else {
		sprintf(filename, "%s_raw%.1f", mesh->name, r);
	}
	sprintf(filename, "%s_r%.1f", filename, glUnitRatio*100.0);
	sprintf(filename, "%s.sal", filename);
	
	if (isAllowToReadFromFile && readMeshSaliency(filename, mesh)) {
		printdg("@ read saliency from \'%s\'\n", filename);
	} else {
		for (i=0; i<msal->nlist; i++) {
			mesh->verts[i].saliency = 0;
			for (j=0; j<msal->nhier; j++) {
				mesh->verts[i].saliency += ABS(msal->list[i].rawSaliency[j]);
			}
		}
		if (r != 0)  smoothMeshSaliency(mesh, r*mesh->unit);
		writeMeshSaliencyBin(filename, mesh);
	}
	msal->minsal = FLT_MAX;
	msal->maxsal = -FLT_MAX;
	for (i=0; i<msal->nlist; i++) {
		SET_MIN_MAX(msal->minsal, msal->maxsal, mesh->verts[i].saliency);
	}
}



void computeSaliency(Smesh *mesh, SsaliencyList *sal)
{
	Svertex **verts = 0, **vertsBig = 0, **vertsSmall = 0;
	int nverts = 0, nvertsBig = 0, nvertsSmall = 0;
	float r[2], rsqr[2], maxr[2], oneovervarsqr[2];
	int i, j;

	mymalloc(gVisited, bool, mesh->nverts);
	mymalloc(gDistance, float, mesh->nverts);
	for (i=0; i<mesh->nverts; i++) {
		gVisited[i] = false;
		gDistance[i] = FLT_MAX;
	}

	mymalloc(verts, Svertex *, mesh->nverts);
	mymalloc(vertsSmall, Svertex *, mesh->nverts);

	for (i=0; i<2; i++) {
		r[i] = 2*sal->var[i];
		rsqr[i] = SQR(r[i]);
		maxr[i] = SQR(r[i] + glGrid.celldiagonal);
		oneovervarsqr[i] = 1/SQR(sal->var[i]);
	}

	for (i=0; i<mesh->nverts; i++) {
		Svertex *vert = &(mesh->verts[i]);
		float gausslist[2];
	
		sal->list[vert->id].rawSaliency = getVertNeighborsGridCombine(vert, mesh, oneovervarsqr, r, rsqr, maxr, gausslist);
		ASSIGN2V(sal->list[vert->id].gaussian, gausslist);
		
		printProgress(i, mesh->nverts);
	}
	myfree(verts);
	myfree(vertsSmall);

	nonlinearlyNormalizeSaliency(mesh, sal);
	myfree(gVisited);
	myfree(gDistance);
}




//------------- Read & Write 


// Assume that memory for <sal->list> already allocated
int readSaliencyBin(char *filename, Smesh *mesh, SsaliencyList *sal)
{
	Utypes buf;
	FILE *fp;
	if ((fp = fopen(filename, "rb")) == 0)  return 0;

	fread(&buf, sizeof(buf.c), 1, fp);
	if (buf.c != 'B') {
		return 0;
	}

	// read number of verts
	fread(&buf, sizeof(buf.i), 1, fp);
	if (mesh->nverts != buf.i)  return 0;
	sal->nlist = mesh->nverts;
	// read variance
	fread(&buf, sizeof(buf.f), 1, fp);
	fread(&buf, sizeof(buf.f), 1, fp);

	for (int i=0; i<sal->nlist; i++) {
		fread(&buf, sizeof(buf.f), 1, fp);
		sal->list[i].gaussian[0] = buf.f;
		fread(&buf, sizeof(buf.f), 1, fp);
		sal->list[i].gaussian[1] = buf.f;
		sal->list[i].rawSaliency = computeRawSaliency(sal->list[i].gaussian);
	}
	nonlinearlyNormalizeSaliency(mesh, sal);

	fclose(fp);
	return 1;
}


// B
// nverts
// variance[0]
// variance[1]
// saliency[0].gaussian[0]
// saliency[0].gaussian[1]
// saliency[1].gaussian[0]
// saliency[1].gaussian[1]
// ...
int writeSaliencyBin(char *filename, Smesh *mesh, SsaliencyList *sal)
{
	Utypes buf;
	FILE *fp;
	if ((fp = fopen(filename, "wb")) == 0)  return 0;

	buf.c = 'B';
	fwrite(&buf, sizeof(buf.c), 1, fp);
	buf.i = mesh->nverts;
	fwrite(&buf, sizeof(buf.i), 1, fp);
	buf.f = sal->var[0];
	fwrite(&buf, sizeof(buf.f), 1, fp);
	buf.f = sal->var[1];
	fwrite(&buf, sizeof(buf.f), 1, fp);
	for (int i=0; i<mesh->nverts; i++) {
		buf.f = sal->list[i].gaussian[0];
		fwrite(&buf, sizeof(buf.f), 1, fp);
		buf.f = sal->list[i].gaussian[1];
		fwrite(&buf, sizeof(buf.f), 1, fp);
	}

	fclose(fp);
	return 1;
}



// Assume that memory for <sal->list> already allocated
int readSaliency(char *filename, Smesh *mesh, SsaliencyList *sal)
{
	return readSaliencyBin(filename, mesh, sal);
}


int writeSaliency(char *filename, Smesh *mesh, SsaliencyList *sal)
{
	return writeSaliencyBin(filename, mesh, sal);
}


//----  Normalized mesh saliency

int readMeshSaliencyBin(char *filename, Smesh *mesh)
{
	Utypes buf;
	FILE *fp;
	if ((fp = fopen(filename, "rb")) == 0)  return 0;

	fread(&buf, sizeof(buf.c), 1, fp);
	if (buf.c != 'B') {
		fclose(fp);
		return 0;
	}

	// read number of verts
	fread(&buf, sizeof(buf.i), 1, fp);
	if (mesh->nverts != buf.i) {
		fclose(fp);
		return 0;
	}

	float mins = FLT_MAX, maxs = -FLT_MAX;
	for (int i=0; i<mesh->nverts; i++) {
		fread(&buf, sizeof(buf.f), 1, fp);
		mesh->verts[i].saliency = buf.f;
		SET_MIN_MAX(mins, maxs, mesh->verts[i].saliency);
	}

	fclose(fp);
	return 1;
}

int writeMeshSaliencyBin(char *filename, Smesh *mesh)
{
	Utypes buf;
	FILE *fp;
	if ((fp = fopen(filename, "wb")) == 0)  return 0;

	buf.c = 'B';
	fwrite(&buf, sizeof(buf.c), 1, fp);

	buf.i = mesh->nverts;
	fwrite(&buf, sizeof(buf.i), 1, fp);

	for (int i=0; i<mesh->nverts; i++) {
		buf.f = mesh->verts[i].saliency;
		fwrite(&buf, sizeof(buf.f), 1, fp);
	}

	fclose(fp);
	return 1;
}

int readMeshSaliency(char *filename, Smesh *mesh)
{
	return readMeshSaliencyBin(filename, mesh);
}

int writeMeshSaliency(char *filename, Smesh *mesh, SMsaliencyList *msal)
{
	getMultiSaliencyFromGaussians(mesh, msal);
	if (useAmplification)  amplification(mesh, msal);

	return writeMeshSaliencyBin(filename, mesh);
}



//==================== Multiresolution Saliency Map =============================
void initSMsaliencyList(SMsaliencyList *sal, Smesh *mesh)
{
	Sfloat2 *mgauss;
	int n = mesh->nverts;
	int i, k;

	sal->nhier = glNumMultiSaliency;
	mymalloc(glMgaussianVar, Sfloat2, sal->nhier);
	mgauss = glMgaussianVar;

	sal->nlist = n;
	mymalloc(sal->list, SMsaliency, sal->nlist);
	for (i=0; i<sal->nlist; i++) {
		mymalloc(sal->list[i].gaussian, Sfloat2, sal->nhier);
		mymalloc(sal->list[i].rawSaliency, float, sal->nhier);
	}

	mymalloc(sal->var, Sfloat2, sal->nhier);
	for (i=0; i<sal->nhier; i++) {
		for (k=0; k<2; k++) {
			float factor;
			if (k==0)  factor = 1;
			else  factor = glCenterSurroundFactor;
			if (useHierarchicalScales) {
				mgauss[i].val[k] = (glStartMvar*pow(2, i))* factor; 
			} else {
				mgauss[i].val[k] = (glStartMvar+i*glMvarInc)* factor; 
			}
			if (mgauss[i].val[k] > mesh->extent / mesh->unit) {
				sal->nhier = i;
				break;
			}
			sal->var[i].val[k] = 1.0*mesh->unit * mgauss[i].val[k];
			printdg("var[%d][%d] = %f\n", i, k, sal->var[i].val[k] / mesh->unit);
		}
	}
}



void getVertNeighborsGrid(Svertex ***nei, int *nnei, Svertex *vert, Smesh *mesh, float var)
{
	Svertex *v1;
	float val = 0, r = 2*var, rsqr = r*r, dist;
	float gausssum = 0;
	int i;
	int nadjverts = 0;
	int starti[3], endi[3], i0, i1, i2;

	(*nei)[nadjverts] = vert;
	nadjverts = 1;

	findNeighborGrids(starti, endi, vert->coords, r);
	for (i2=starti[2]; i2 <= endi[2]; i2++)  
	for (i1=starti[1]; i1 <= endi[1]; i1++) 
	for (i0=starti[0]; i0 <= endi[0]; i0++)  {
		int idx = getGridIdx(i0, i1, i2);
		SgridItem *grid = &(glGrid.data[idx]);
		for (i=0; i<grid->nverts; i++) {
			dist = DIST3VSQR(vert->coords, grid->verts[i].x);
			if (dist > rsqr)  continue;
			v1 = &(mesh->verts[grid->verts[i].id]);
			(*nei)[nadjverts] = v1;
			nadjverts++;
		}
	}

	*nnei = nadjverts;
}


#define USE_GAUSS_TABLE

#ifdef USE_GAUSS_TABLE

float getVertNeighborsGridCombine(Svertex *vert, Smesh *mesh, float oneovervarsqr[2], 
								  float r[2], float rsqr[2], float maxr[2], float gaussianlist[2])
{
	float normdist, distsqr;
	int nadjverts = 0;
	float gausssum[2], gaussval, signal;
	float gaussian[2], saliency = 1;
	int starti[3], endi[3], i0, i1, i2;
	int i, j, k;

	gausssum[0] = gausssum[1] = 0;
	gaussian[0] = gaussian[1] = 0;

	findNeighborGrids(starti, endi, vert->coords, r[1]);
	for (i2=starti[2]; i2 <= endi[2]; i2++)  
	for (i1=starti[1]; i1 <= endi[1]; i1++) 
	for (i0=starti[0]; i0 <= endi[0]; i0++)  {
		int idx = getGridIdx(i0, i1, i2);
		SgridItem *grid = &(glGrid.data[idx]);
		for (i=0; i<grid->nverts; i++) {
			distsqr = DIST3VSQR(vert->coords, grid->verts[i].x);

			if (distsqr > maxr[1]) break;
			if (distsqr > rsqr[1])  continue;

			normdist = distsqr * oneovervarsqr[1];
			signal = grid->verts[i].meancurv;

			if (useAreaForSaliency) {
				gaussval = grid->verts[i].area * gauss(&glGaussSqrTable, normdist);
			} else {
				gaussval = gauss(&glGaussSqrTable, normdist); 
			}
			gausssum[1] += gaussval;
			gaussian[1] += gaussval * signal;

			if (distsqr > maxr[0]) break;
			if (distsqr > rsqr[0])  continue;

			normdist = distsqr * oneovervarsqr[0];
			if (useAreaForSaliency) {
				gaussval = grid->verts[i].area * gauss(&glGaussSqrTable, normdist);
			} else {
				gaussval = gauss(&glGaussSqrTable, normdist);
			}
			gausssum[0] += gaussval;
			gaussian[0] += gaussval * signal;
		}
	}

	for (k=0; k<2; k++) {
		if (gausssum[k] > 0)  gaussian[k] /= gausssum[k];
	}

	saliency = computeRawSaliency(gaussian);
	ASSIGN2V(gaussianlist, gaussian);
	return saliency;
} // 

#else

float getVertNeighborsGridCombine(Svertex *vert, Smesh *mesh, float oneovervarsqr[2], 
								  float r[2], float rsqr[2], float maxr[2], float gaussianlist[2])
{
	float normdist, distsqr;
	int nadjverts = 0;
	float gausssum[2], gaussval, signal;
	float gaussian[2], saliency = 1;
	int starti[3], endi[3], i0, i1, i2;
	int i, j, k, idx;

	gausssum[0] = gausssum[1] = 0;
	gaussian[0] = gaussian[1] = 0;

	findNeighborGrids(starti, endi, vert->coords, r[1]);
	for (i2=starti[2]; i2 <= endi[2]; i2++)  {
	for (i1=starti[1]; i1 <= endi[1]; i1++)  {
	for (i0=starti[0]; i0 <= endi[0]; i0++)  {
		idx = getGridIdx(i0, i1, i2);
		SgridItem *grid = &(glGrid.data[idx]);
		for (i=0; i<grid->nverts; i++) {
			distsqr = DIST3VSQR(vert->coords, grid->verts[i].x);

			if (distsqr > maxr[1]) break;
			if (distsqr > rsqr[1])  continue;

			normdist = distsqr * oneovervarsqr[1]; 
			float x = -normdist*0.5;
			signal = grid->verts[i].meancurv; 

			gaussval = ( 1 + x*(1 + 0.5*x*(1 + x*0.3333333333*(1 + x*0.25)))  );
			if (useAreaForSaliency) {
				gaussval *= grid->verts[i].area;
			}
			gausssum[1] += gaussval;
			gaussian[1] += gaussval * signal;

			if (distsqr > maxr[0]) break;
			if (distsqr > rsqr[0])  continue;
			normdist = distsqr * oneovervarsqr[0]; 
			x = -normdist*0.5;
			gaussval = ( 1 + x*(1 + 0.5*x*(1 + x*0.3333333333*(1 + x*0.25)))  );
			if (useAreaForSaliency) {
				gaussval *= grid->verts[i].area;
			}
			gausssum[0] += gaussval;
			gaussian[0] += gaussval * signal;
		}
	}}}

	for (k=0; k<2; k++) {
		if (gausssum[k] > 0)  gaussian[k] /= gausssum[k];
	}

	saliency = computeRawSaliency(gaussian);
	ASSIGN2V(gaussianlist, gaussian);
	return saliency;
} // 

#endif
