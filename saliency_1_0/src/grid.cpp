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
#include <memory.h>
#include <float.h>
#include "timerms.h"
#include "geom.h"
#include "macro.h"
#include "matrix.h"
#include "GL/glut.h"
#include "iui2cpp.h"
#include "saliency.h"
#define _GRID_GLOBAL_DEFINE
#include "grid.h"


#define EPSILON	1e-10


//----------------------------------------------------
//			functions
//----------------------------------------------------

void initGrid(Sgrids *grids, Smesh *mesh, int gridnum)
{
	grids->gridnum = gridnum;
	grids->ndata = CUBE(grids->gridnum);
	mymalloc(grids->data, SgridItem, grids->ndata);
	grids->size = 2*mesh->extent;
	grids->gridsize = grids->size / (float)grids->gridnum;
	grids->celldiagonal = sqrt(3)*grids->gridsize;
	for (int k=0; k<3; k++) {
		grids->minx[k] = mesh->center[k] - mesh->extent;
	}
}

void buildGrid(Sgrids *grids, Smesh *mesh)
{
	int i, j, k, xi[3], idx;

	for (i=0; i<grids->ndata; i++) {
		grids->data[i].nverts = 0;
		grids->data[i].verts = 0;
	}
	for (i=0; i<mesh->nverts; i++) {
		if (!(mesh->curvatures[i].isValid))  continue;
		for (k=0; k<3; k++) {
			xi[k] = (mesh->verts[i].coords[k]-grids->minx[k]) / grids->gridsize;
			if (xi[k] >= grids->gridnum)  xi[k] = grids->gridnum-1;
		}
		idx = getGridIdx(xi);
		grids->data[idx].nverts++;
	}
	for (i=0; i<grids->ndata; i++) {
		mymalloc(grids->data[i].verts, Sval3, grids->data[i].nverts);
		grids->data[i].nverts = 0;
	}
	for (i=0; i<mesh->nverts; i++) {
		if (!(mesh->curvatures[i].isValid))  continue;
		for (k=0; k<3; k++) {
			xi[k] = (mesh->verts[i].coords[k]-grids->minx[k]) / grids->gridsize;
			if (xi[k] >= grids->gridnum)  xi[k] = grids->gridnum-1;
		}
		idx = getGridIdx(xi);
		ASSIGN3V(grids->data[idx].verts[grids->data[idx].nverts].x, mesh->verts[i].coords);
		grids->data[idx].verts[grids->data[idx].nverts].area = mesh->verts[i].area;
		grids->data[idx].verts[grids->data[idx].nverts].meancurv = computeMeanCurvature(mesh, i);
		grids->data[idx].verts[grids->data[idx].nverts].id = i;
		grids->data[idx].nverts++;
	}
	int nverts = 0, nnonzero = 0;
	for (i=0; i<grids->ndata; i++) {
		if (grids->data[i].nverts > 0)  { 
			nnonzero++;
		}
		nverts += grids->data[i].nverts;
	}
}

void findNeighborGrids(int starti[3], int endi[3], float x[3], float r)
{
	float x0, x1;

	for (int k=0; k<3; k++) {
		x0 = x[k] - r;
		x1 = x[k] + r;

		starti[k] = (x0 - glGrid.minx[k]) / glGrid.gridsize;
		endi[k] = (x1 - glGrid.minx[k]) / glGrid.gridsize;

		TRIM(starti[k], 0, glGrid.gridnum-1);
		TRIM(endi[k], 0, glGrid.gridnum-1);
	}
}



