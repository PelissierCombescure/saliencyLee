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
#ifndef _GRID_H
#define _GRID_H

#define GRID_NUM  64 //32

//----  Data Structures
typedef struct {
	float x[3];
	float meancurv;
	float area;
	int id;
} Sval3;

typedef struct {
	Sval3 *verts;
	int nverts;
} SgridItem;

typedef struct {
	SgridItem *data;
	int ndata;
	int gridnum;
	float size;
	float gridsize;
	float celldiagonal;
	float minx[3];
} Sgrids;

//----  Global Variables

#ifndef _GRID_GLOBAL_DEFINE
#define _GRID_EXTERN extern
#else
#define _GRID_EXTERN
#endif


_GRID_EXTERN Sgrids glGrid;

#ifdef _GRID_GLOBAL_DEFINE
// definition

_GRID_EXTERN int glGridNum = GRID_NUM;
_GRID_EXTERN float glGridSize = GRID_NUM;
_GRID_EXTERN float glGridMin[3] = { 0, 0, 0 };

#else
// declaration

_GRID_EXTERN int glGridNum;
_GRID_EXTERN float glGridSize;
_GRID_EXTERN float glGridMin[3];

#endif

#undef _GRID_EXTERN
#undef _GRID_GLOBAL_DEFINE


//-------- Functions
void initGrid(Sgrids *grids, Smesh *mesh, int gridnum);
void buildGrid(Sgrids *grids, Smesh *mesh);
void findNeighborGrids(int starti[3], int endi[3], float x[3], float r);


//-------- inline Functions

inline int getGridIdx(int x, int y, int z)
{ 
	return (x + glGridNum*(y + glGridNum*z));
}

inline int getGridIdx(int x[3]) 
{ 
	return getGridIdx(x[0], x[1], x[2]);
}

inline void getGridCoord(int& x, int& y, int& z, int idx)
{ 
	x = idx % glGridNum;
	int temp = idx / glGridNum;
	y = temp % glGridNum;
	z = temp / glGridNum;
}

inline void getGridCoord(int x[3], int idx)
{
	getGridCoord(x[0], x[1], x[2], idx);
}


#endif