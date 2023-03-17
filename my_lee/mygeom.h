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
#ifndef GEOM_H
#define GEOM_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

//#define M_PI	3.14159265358979323844


typedef unsigned char	Tbyte;
typedef float			Tpoint3[3];
typedef float			Tpoint4[4];
typedef float			Tvector3[3];
typedef float			Tvector4[4];
typedef Tbyte			Tcolor4[4];

typedef float Tmatrix4[4][4];

typedef union {
	char c;
	int i;
	long l;
	float f;
	double d;
} Utypes;


struct Striangle;
struct Sedge;

typedef struct {
	Tcolor4 ambient;
	Tcolor4 diffuse;
	Tcolor4 specular;
	Tcolor4 display;
} Scolor4;

typedef struct {
	Tpoint3		coords;
	Tvector3	normal;
	Scolor4		color;
	int id;
	Sedge **edges;
	int nedges;
	Striangle **tris;
	int ntris;
	float saliency;
	float area;
} Svertex;

typedef struct {
	int		nverts;
	Svertex *verts;
} Svertices;

typedef struct SvertPointers {
	int		nverts;
	Svertex **verts;
} SvertPointers;


typedef struct Striangle {
    Svertex		*verts[3];	/* Vertices of triangle */
	Tvector3	normal;
	Tcolor4		color;
	float		area;
	int			id;
} Striangle;

typedef struct {
    int			ntris;		/* # of triangles in object */
    Striangle	*tris;		/* Triangles */
} Striangles;


typedef struct Sedge {
	Svertex *verts[2];
	Striangle *tris[2];
} Sedge;


typedef struct {
	float coord[2];
	float E1[3];
	float E2[3];
	bool isValid;
} Scurvature;


typedef struct {
	char *name;
	Svertex *verts;
	int nverts;
	Striangle *tris;
	int ntris;
	Sedge *edges;
	int nedges;
	float unit;
	float center[3];
	float extent;
	Scurvature *curvatures;
	float minCurvature[2], maxCurvature[2];
	float minMeanCurv, maxMeanCurv;
} Smesh;


//----------- Functions 
void freeSmesh(Smesh *mesh);
void resetSmesh(Smesh *mesh);
void resetSvertex(Svertex *vert);

//-----------------------

inline Svertex *getOtherVertInEdge(Sedge *edge, Svertex *vert)
{
	return ((edge->verts[0]->id != vert->id)? edge->verts[0]:edge->verts[1]);
}


#endif