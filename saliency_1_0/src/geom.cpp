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
#include <float.h>
#include "geom.h"
#include "macro.h"

void freeSmesh(Smesh *mesh)
{
	for (int i=0; i<mesh->nverts; i++) {
		myfree(mesh->verts[i].edges);
		myfree(mesh->verts[i].tris);
	}
	myfree(mesh->verts);
	myfree(mesh->curvatures);
	mesh->nverts = 0;

	myfree(mesh->tris);
	mesh->ntris = 0;
	myfree(mesh->edges);
	mesh->nedges = 0;
}

void resetSmesh(Smesh *mesh)
{
	mesh->verts = 0;
	mesh->tris = 0;
	mesh->nverts = mesh->ntris = 0;

	mesh->curvatures = 0;
	mesh->edges = 0;
	mesh->nedges = 0;
	mesh->name = "";
}


void resetSvertex(Svertex *vert)
{
	vert->edges = 0;
	vert->tris = 0;
	vert->nedges = vert->ntris = 0;
}

//-------------------------

void computeTriangleArea(Striangle *t)
{
	// Use the Hernon equation to determine the triangle area
	float a, b, c, s;

	a = DIST3V(t->verts[2]->coords, t->verts[0]->coords);
	b = DIST3V(t->verts[1]->coords, t->verts[0]->coords);
	c = DIST3V(t->verts[2]->coords, t->verts[1]->coords);
	s = (a+b+c) / 2.0f;

	t->area = sqrt(s * (s-a) * (s-b) * (s-c));
}

void computeTriangleAreas(Smesh *surf)
{
	for (int i=0; i<surf->ntris; i++) {
		computeTriangleArea(&(surf->tris[i]));
	}
}

void computeVertexAreas(Smesh *mesh)
{
	int i, j;

	for (i=0; i<mesh->nverts; i++) {
		mesh->verts[i].area = 0;
	}
	for (i=0; i<mesh->ntris; i++) {
		for (j=0; j<3; j++) {
			Svertex *v = mesh->tris[i].verts[j];
			v->area += mesh->tris[i].area/3.0;
		}
	}
}


void computeSmeshAreas(Smesh *mesh)
{
	computeTriangleAreas(mesh);
	computeVertexAreas(mesh);
}

void computeTriNormals(Smesh *surf)
{
	Tvector3 ab, ac;
	int i, j;

	for (i=0; i<surf->ntris; i++) {
		for (j=0; j<3; j++) {
			ab[j] = surf->tris[i].verts[1]->coords[j] - surf->tris[i].verts[0]->coords[j];
			ac[j] = surf->tris[i].verts[2]->coords[j] - surf->tris[i].verts[0]->coords[j];
		}
		CROSS_PROD3V(surf->tris[i].normal, ab, ac);
		NORMALIZE3V(surf->tris[i].normal);
	}
}


void freeVertNeighbors(Smesh *surf)
{
	int i;

	if (surf == 0)  return;
	for (i=0; i<surf->nverts; i++) {
		myfree(surf->verts[i].tris);
		myfree(surf->verts[i].edges);
		surf->verts[i].ntris = surf->verts[i].nedges = 0;
	}
}

void computeVertNeighbors(Smesh *surf)
{
	Tvector3 ab, ac;
	int i, j, k, vid, tid;

	if (surf == 0)  return;
	freeVertNeighbors(surf);

	for (i=0; i<surf->nverts; i++) {
		surf->verts[i].id = i;
		surf->verts[i].ntris = 0;
	}
	for (i=0; i<surf->ntris; i++) {
		for (j=0; j<3; j++) {
			vid = surf->tris[i].verts[j]->id;
			++(surf->verts[vid].ntris);
		}
	}
	for (i=0; i<surf->nverts; i++) {
		mymalloc(surf->verts[i].tris, Striangle *, surf->verts[i].ntris);
		surf->verts[i].ntris = 0;
	}

	for (i=0; i<surf->ntris; i++) {
		for (j=0; j<3; j++) {
			vid = surf->tris[i].verts[j]->id;
			tid = surf->verts[vid].ntris;
			surf->verts[vid].tris[tid] = &(surf->tris[i]);
			++(surf->verts[vid].ntris);
		}
	}
}


void computeVertNormals(Smesh *surf)
{
	double areaNormal[3];
	int i, j, k;

	if (surf == NULL)  return;
	computeVertNeighbors(surf);
	computeSmeshAreas(surf);

	for (i=0; i<surf->nverts; i++) {
		for (k=0; k<3; k++) {
			areaNormal[k] = 0;
		}
		for (j=0; j<surf->verts[i].ntris; j++) {
			for (k=0; k<3; k++) {
				areaNormal[k] += surf->verts[i].tris[j]->area*surf->verts[i].tris[j]->normal[k];
			}
		}
		if (LEN3V(areaNormal) !=0) {
			NORMALIZE3V(areaNormal);
		}
		ASSIGN3V(surf->verts[i].normal, areaNormal);
	}
}


void computeNormals(Smesh *surf)
{
	computeTriNormals(surf);
	computeVertNormals(surf);
}


//========================
//      Edges
//========================
inline bool isVertInTri(Striangle *tri, Svertex *vert)
{
	for (int i=0; i<3; i++) {
		if (tri->verts[i]->id == vert->id)  return true;
	}
	return false;
}


inline Svertex *getNextAdjVert(Striangle *tri, Svertex *vert)
{
	Svertex *adjvert;
	int i;
	for (i=0; i<3; i++) {
		if (tri->verts[i]->id == vert->id) {
			break;
		}
	}
	adjvert = tri->verts[(i+1)%3];
	return adjvert;
}

/*inline Svertex *getNextVert(Svertex *vert, Striangle *tri, Svertex *adjvert)
{
	for (int i=0; i<3; i++) {
		if (tri->verts[i]->id != vert->id && tri->verts[i]->id != adjvert->id) {
			return tri->verts[i];
		}
	}
	return 0;
}*/

inline Striangle *getNextTri(Svertex *vert, Striangle *tri, Svertex *adjvert)
{
	Striangle *opptri;
	for (int i=0; i<vert->ntris; i++) {
		opptri = vert->tris[i];
		if (opptri->id != tri->id && isVertInTri(opptri, adjvert)) {
			return opptri;
		}
	}
	return 0;
}


void computeEdges(Smesh *mesh)
{
	bool *visited, *added;
	Svertex *adjVert, *nextVert;
	Striangle *adjTri, *nextTri;
	int nedges=0, nedgestotal=0, maxntris, *intbuf;
	int i, j, k, ei = 0, vid, vi1, vi2;

	mymalloc(visited, bool, mesh->nverts);
	mymalloc(added, bool, mesh->nverts);
	maxntris = mesh->verts[0].ntris;
	for (i=0; i<mesh->nverts; i++) {
		visited[i] = added[i] = false;
		if (mesh->verts[i].ntris > maxntris)  maxntris = mesh->verts[i].ntris;
	}
	mymalloc(intbuf, int, 2*maxntris);
	for (i=0; i<mesh->nverts; i++) {
		nedges = 0;
		for (j=0; j<mesh->verts[i].ntris; j++) {
			for (k=0; k<3; k++) {
				vid = mesh->verts[i].tris[j]->verts[k]->id;
				if (vid != mesh->verts[i].id && !added[vid]) {
					intbuf[nedges] = vid;
					nedges++;
					added[vid] = true;
				}
			}
		}
		for (j=0; j<nedges; j++)  added[intbuf[j]] = false;
		mymalloc(mesh->verts[i].edges, Sedge *, nedges);
		nedgestotal += nedges;
	}
	nedgestotal *= 0.5;
	mesh->nedges = 1.10f * nedgestotal;
	mymalloc(mesh->edges, Sedge, mesh->nedges);

	for (i=0, ei=0; i<mesh->nverts; i++) {
		if (visited[i])  continue;

		for (j=0; j<mesh->verts[i].ntris; j++) {
			adjVert = &(mesh->verts[i]);
			adjTri = mesh->verts[i].tris[j];
			for (k=0; k<2; k++) {
				adjVert = getNextAdjVert(mesh->verts[i].tris[j], adjVert);
				vid = adjVert->id;
				if (!visited[vid] && !added[vid]) {
					nextTri = getNextTri(&(mesh->verts[i]), adjTri, adjVert);
					mesh->edges[ei].verts[0] = &(mesh->verts[i]);
					mesh->edges[ei].verts[1] = adjVert;
					mesh->edges[ei].tris[0] = adjTri;
					mesh->edges[ei].tris[1] = nextTri;

					mesh->verts[i].edges[mesh->verts[i].nedges] = &(mesh->edges[ei]);
					mesh->verts[i].nedges++;
					adjVert->edges[adjVert->nedges] = &(mesh->edges[ei]);
					adjVert->nedges++;
					ei++;
					added[vid] = true;
				}
			}
		}
		for (j=0; j<mesh->verts[i].nedges; j++) {
			vid = getOtherVertInEdge(mesh->verts[i].edges[j], &(mesh->verts[i]))->id;
			added[vid] = false;
		}
		visited[i] = true;
	}
	mesh->nedges = ei;

	printf("# mesh: %d vertices, %d triangles, %d edges=%d\n", mesh->nverts, mesh->ntris, mesh->nedges, nedgestotal);
	myfree(visited);
	myfree(added);
	myfree(intbuf);
}



