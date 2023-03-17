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

// General Curvature defines --------------------------------------

#define CURV_CURVATURE_COMPUTATION_EPPSILON 0.000001

// Defines particular to a object type -------------------------------
// Ellipsoid
#define CURV_ELLIPSOID_LAMBDA_DIFF 0.001

//------------ Matrix

#define subtractV3(A,B,C)	(A)[0]=(B)[0]-(C)[0];	(A)[1]=(B)[1]-(C)[1];	(A)[2]=(B)[2]-(C)[2];
#define addV3(A,B,C)		(A)[0]=(B)[0]+(C)[0];	(A)[1]=(B)[1]+(C)[1];	(A)[2]=(B)[2]+(C)[2];
#define multV3S(A,B,S)		(A)[0]=(B)[0]*(S);	(A)[1]=(B)[1]*(S);	(A)[2]=(B)[2]*(S);
#define multM4S(A,B,S)		(A)[0][0]=(B)[0][0]*(S);  (A)[0][1]=(B)[0][1]*(S);  (A)[0][2]=(B)[0][2]*(S);  (A)[0][3]=(B)[0][3]*(S); \
	(A)[1][0]=(B)[1][0]*(S);  (A)[1][1]=(B)[1][1]*(S);  (A)[1][2]=(B)[1][2]*(S);  (A)[1][3]=(B)[1][3]*(S);	\
	(A)[2][0]=(B)[2][0]*(S);  (A)[2][1]=(B)[2][1]*(S);  (A)[2][2]=(B)[2][2]*(S);  (A)[2][3]=(B)[2][3]*(S);	\
	(A)[3][0]=(B)[3][0]*(S);  (A)[3][1]=(B)[3][1]*(S);  (A)[3][2]=(B)[3][2]*(S);  (A)[3][3]=(B)[3][3]*(S);


//----------------------------------------------------
//			functions
//----------------------------------------------------

void setIdentity(float mat[4][4])
{
	mat[0][0] = mat[0][1] = mat[0][2] = mat[0][3] = 0;
	mat[1][0] = mat[1][1] = mat[1][2] = mat[1][3] = 0;
	mat[2][0] = mat[2][1] = mat[2][2] = mat[2][3] = 0;
	mat[3][0] = mat[3][1] = mat[3][2] = mat[3][3] = 0;
	mat[0][0] = mat[1][1] = mat[2][2] = mat[3][3] = 1;
}

void setZero(float mat[4][4])
{
	mat[0][0] = mat[0][1] = mat[0][2] = mat[0][3] = 0;
	mat[1][0] = mat[1][1] = mat[1][2] = mat[1][3] = 0;
	mat[2][0] = mat[2][1] = mat[2][2] = mat[2][3] = 0;
	mat[3][0] = mat[3][1] = mat[3][2] = mat[3][3] = 0;
}

void setOne(float mat[4][4])
{
	mat[0][0] = mat[0][1] = mat[0][2] = mat[0][3] = 1;
	mat[1][0] = mat[1][1] = mat[1][2] = mat[1][3] = 1;
	mat[2][0] = mat[2][1] = mat[2][2] = mat[2][3] = 1;
	mat[3][0] = mat[3][1] = mat[3][2] = mat[3][3] = 1;
}


void transposeM4(float m1[4][4], float m2[4][4])
{
	int i, j;
	for (i = 0 ; i < 4 ; i++)
		for (j = 0 ; j < 4 ; j++)
			m1[j][i] = m2[i][j] ;
}


void multM4M4(float res[4][4], float m1[4][4], float m2[4][4])
{
	int i, j, k;

	for (i = 0 ; i < 4 ; i++)
		for (j = 0 ; j < 4 ; j++)
		{
			res[i][j] = 0.0 ;
			for (k = 0 ; k < 4 ; k++)
				res[i][j] += m1[i][k] * m2[k][j];
		}
}

void addM4M4(float res[4][4], float m1[4][4], float m2[4][4])
{
	int i, j;
	for (i = 0 ; i < 4 ; i++)
		for (j = 0 ; j < 4 ; j++)
			res[i][j] = m1[i][j] + m2[i][j];
} 

   
void subtractM4M4(float res[4][4], float m1[4][4], float m2[4][4])
{
	int i, j;
	for (i = 0 ; i < 4 ; i++)
		for (j = 0 ; j < 4 ; j++)
			res[i][j] = m1[i][j] - m2[i][j];
} 
   


void energyMatrix(Tmatrix4 retM, Tpoint3 v)
{
	retM[0][0] = v[0]*v[0];
	retM[0][1] = v[0]*v[1];
	retM[0][2] = v[0]*v[2];
	retM[0][3] = 0;

	retM[1][0] = v[1]*v[0];
	retM[1][1] = v[1]*v[1];
	retM[1][2] = v[1]*v[2];
	retM[1][3] = 0;

	retM[2][0] = v[2]*v[0];
	retM[2][1] = v[2]*v[1];
	retM[2][2] = v[2]*v[2];
	retM[2][3] = 0;

	retM[3][0] = 0;
	retM[3][1] = 0;
	retM[3][2] = 0;
	retM[3][3] = 1;
}


//<<<<<<<<<<<< Matrix


inline int triangleHasVertex(Striangle *tri, Svertex *vert)
{
	return (tri->verts[0] == vert || tri->verts[1] == vert || tri->verts[2] == vert);
}


bool isNotInNeighbors(SvertPointers *neighbors, Svertex *vert)
{
	int i;
	for (i=0; i<neighbors->nverts; i++) {
		if (vert->id == neighbors->verts[i]->id)  return false;
	}
	return true;
}


void getNeighborVerts(SvertPointers *neighbors, Smesh *mesh, Svertex *vert)
{
	int i, j;

	neighbors->nverts = 0;
	myfree(neighbors->verts);
	mymalloc(neighbors->verts, Svertex *, 2*vert->ntris);
	for (i=0; i<vert->ntris; i++) {
		for (j=0; j<3; j++) {
			if (vert->tris[i]->verts[j]->id != vert->id && isNotInNeighbors(neighbors, vert->tris[i]->verts[j])) {
				neighbors->verts[neighbors->nverts] = vert->tris[i]->verts[j];
				neighbors->nverts++;
			}
		}
	}
}


// Algo:
//  - compute vertex normals
//  - compute the Matrix per vertex
//  - Compute principal curvatures from it
void computeCurvatures(Smesh *mesh)
{
	int				t, v, i;
	Striangle		*incidentTriangle;
	Svertex			*neighbor; 
	SvertPointers	neighborVerts = { 0, 0 };
	float			k_ij,	// The curvature between the vertex i and its neighbor j
					w_ij,	// Thw weightage factor a vertex i and its neighbor j
					total_w_ij;	// Sum total of all w_ij associated with the vertex i
	Tpoint3			N, E1, E2,
					diffV,
					tempV, tempV2,
					T_ij,	// The projection of the vector between the vertex and its neighbor onto the tangent plane of the vertex
					W_v_i,	// Ref. taubin's paper
					T_1_tilde, T_2_tilde;
	Tmatrix4		T_ij_T_ij_t,	// We dont have a MATRIX3<T> class. So we will use the MATRIX4 class.
					M_v_i,
					W_v_i_W_v_i_t,
					Q_v_i,
					final_v_i,
					identityMatrix,
					rotM1, transM1,
					tempM, tempM2;
	float			a11, a12, a21, a22, 
					b, c, tempF, 
					e1x, e1y, e2x, e2y,
					lambda1, lambda2,
					tempF1, tempF2,
					maxX, maxY;

	setIdentity(identityMatrix);

	// Algo:
	//  - compute triangle areas
	//  - determine the neighbors of each vertex
	//  - compute the vertex normals
	//  - compute vertex curvatures
	//  - increase widths to cover holes

	myfree(mesh->curvatures);
	mymalloc(mesh->curvatures, Scurvature, mesh->nverts);
	mesh->maxCurvature[0] = mesh->maxCurvature[1] = mesh->maxMeanCurv = -FLT_MAX;
	mesh->minCurvature[0] = mesh->minCurvature[1] = mesh->minMeanCurv = FLT_MAX;
	// Compute vertex curvatures
	for(v=0; v<mesh->nverts; v++)
	{
		// Algo: (for each vertex)
		// - For each neighbor
		//   - Compute T_ij
		//   - Compute k_ij
		//   - Compute w_ij
		// - Compute M_v
		// - Compute Q_v
		// - Compute T_1 and T_2

		if(mesh->verts[v].ntris <= 0)	{	// Its a spurious pt not included in the mesh
			mesh->curvatures[v].isValid = false;
			ASSIGN2V2(mesh->curvatures[v].coord, 0, 0);
			continue;
		} else {
			mesh->curvatures[v].isValid = true;
		}

		total_w_ij = 0;
		setZero(M_v_i);
		getNeighborVerts(&neighborVerts, mesh, &(mesh->verts[v]));
		for (i=0; i<neighborVerts.nverts; i++) {
			neighbor = neighborVerts.verts[i];

			// Compute T_ij
			subtractV3(diffV, neighbor->coords, mesh->verts[v].coords);
			multV3S(tempV, mesh->verts[v].normal, DOT_PROD3V(mesh->verts[v].normal, diffV));
			subtractV3(T_ij, diffV, tempV);
			NORMALIZE3V(T_ij);

			// Compute k_ij
			NORMALIZE3V(diffV);
			k_ij = DOT_PROD3V(mesh->verts[v].normal, diffV) * 2;

			// Compute w_ij
			//  - determine the common triangles
			//  - add up their area
			w_ij = 0;
			for (t=0; t<mesh->verts[v].ntris; t++) {  //for(t=0;t<vertexAdj[v].size;t++)
				if(	triangleHasVertex(mesh->verts[v].tris[t], &(mesh->verts[v])) &&
					triangleHasVertex(mesh->verts[v].tris[t], neighbor)) {
					w_ij += mesh->verts[v].tris[t]->area;
				}
			}
			total_w_ij += w_ij;

			energyMatrix(T_ij_T_ij_t, T_ij);
			multM4S(tempM, T_ij_T_ij_t, w_ij*k_ij);
			addM4M4(M_v_i, M_v_i, tempM);
		}
		myfree(neighborVerts.verts);

		// Normalize the values of w_ij
		multM4S(M_v_i, M_v_i, 1/total_w_ij);
		M_v_i[3][3] = 1;

		// Compute W_v_i
		ASSIGN3V3(tempV, 1,0,0);

		addV3(tempV2, tempV, mesh->verts[v].normal);
		if( DIST3V(tempV, mesh->verts[v].normal) > LEN3V(tempV2)) 
		{
			subtractV3(W_v_i, tempV, mesh->verts[v].normal);
		} else {
			addV3(W_v_i, tempV, mesh->verts[v].normal);
		}
		NORMALIZE3V(W_v_i);

		// Compute Q_v_i
		energyMatrix(Q_v_i, W_v_i);
		multM4S(tempM, Q_v_i, 2);
		subtractM4M4(Q_v_i, identityMatrix, tempM);
		Q_v_i[3][3] = 1;

		// Compute the final matrix  //final_v_i = Q_v_i.Transpose() * (M_v_i * Q_v_i);
		transposeM4(tempM, Q_v_i);
		multM4M4(tempM2, M_v_i, Q_v_i);
		multM4M4(final_v_i, tempM, tempM2);
		
		// Compute the eigenvalues and eigenvectors
		// Algo:
		//  - Determine T_1^tilde and T_2^tilde
		//  - compute theta_0 which zeros the element a21
		//  - compute theta_1 which zeros the element a12'
		//  - the required rotation is theta_0+theta_1
		//  - Rotate T_1^tilde and T_2^tilde to get E_1 and E_2
		//  - compute eigenvalues of M_v_i
		ASSIGN3V(N, mesh->verts[v].normal);
		ASSIGN3V3(T_1_tilde, Q_v_i[0][1], Q_v_i[1][1], Q_v_i[2][1]);
		ASSIGN3V3(T_2_tilde, Q_v_i[0][2], Q_v_i[1][2], Q_v_i[2][2]);

		a11 = final_v_i[1][1]; if(ABS(a11) < CURV_CURVATURE_COMPUTATION_EPPSILON) a11 = 0;
		a12 = final_v_i[1][2]; if(ABS(a12) < CURV_CURVATURE_COMPUTATION_EPPSILON) a12 = 0;
		a21 = final_v_i[2][1]; if(ABS(a21) < CURV_CURVATURE_COMPUTATION_EPPSILON) a21 = 0;
		a22 = final_v_i[2][2]; if(ABS(a22) < CURV_CURVATURE_COMPUTATION_EPPSILON) a22 = 0;

		// Compute lambda1 and lambda2
		if(a12!=0.0)
		{
			b = a11+a22;
			c = a11*a22 - a21*a12;

			tempF = b*b - 4*c;
			if(tempF>0)
			{
				lambda1 = - (-b + sqrt(tempF)) / 2.0;
				lambda2 = - (-b - sqrt(tempF)) / 2.0;
			}
			else
			{
				lambda1 = lambda2 = b * 0.5;
			}

			e1x = 1;
			e1y = - (a11-lambda1) / a12;

			e2x = 1;
			e2y = - (a11-lambda2) / a12;
		}
		else
		{
			lambda1 = a11;
			lambda2 = a22;

			e1x = 1; e1y = 0;
			e2x = 0; e2y = 1;
		}
				
		// Compute E1 and E2
		if( ABS(lambda2-lambda1) < CURV_ELLIPSOID_LAMBDA_DIFF)
		{
			multV3S(tempV, T_1_tilde, e1x);
			multV3S(tempV2, T_2_tilde, e1y);
			addV3(E1, tempV, tempV2); 
			tempF = LEN3V(E1);
			if(tempF==0.0)
			{
				Tpoint3 tempP1 = { 1.0, 0.0, 0.0 };
				CROSS_PROD3V(tempV, N, tempP1);
				if(LEN3V(tempV) == 0.0) {
					ASSIGN3V3(tempP1, 0.0, 1.0, 0.0);
				}
				CROSS_PROD3V(tempV, N, tempP1);
				if(LEN3V(tempV) == 0.0)
					ASSIGN3V3(tempP1, 0.0, 0.0, 1.0);

				CROSS_PROD3V(E1, N, tempP1); NORMALIZE3V(E1);
				CROSS_PROD3V(E2, N, E1);     NORMALIZE3V(E2);
			}
			else
			{
				NORMALIZE3V(E1);
				CROSS_PROD3V(E2, N, E1); NORMALIZE3V(E2);
			}
		}
		else
		{
			multV3S(tempV, T_1_tilde, e1x);
			multV3S(tempV2, T_2_tilde, e1y);
			addV3(E1, tempV, tempV2);
			NORMALIZE3V(E1);
			CROSS_PROD3V(E2, N, E1); NORMALIZE3V(E2);
		}
		
		tempF1 = lambda1; tempF2 = lambda2;
		lambda1 = 3 * tempF1 - tempF2;
		lambda2 = 3 * tempF2 - tempF1;

		if (ABS(lambda1) > ABS(lambda2)) {
			mesh->curvatures[v].coord[0] = lambda1;
			mesh->curvatures[v].coord[1] = lambda2;
			ASSIGN3V(mesh->curvatures[v].E1, E1);
			ASSIGN3V(mesh->curvatures[v].E2, E2);
		} else {
			mesh->curvatures[v].coord[0] = lambda2;
			mesh->curvatures[v].coord[1] = lambda1;
			ASSIGN3V(mesh->curvatures[v].E1, E2);
			ASSIGN3V(mesh->curvatures[v].E2, E1);
		}

		if (_isnan(mesh->curvatures[v].coord[0]) || _isnan(mesh->curvatures[v].coord[1]))
		{
			mesh->curvatures[v].isValid = false;
			ASSIGN2V2(mesh->curvatures[v].coord, 0, 0);
		}

		if (mesh->curvatures[v].coord[0] > mesh->maxCurvature[0])  mesh->maxCurvature[0] = mesh->curvatures[v].coord[0];
		if (mesh->curvatures[v].coord[1] > mesh->maxCurvature[1])  mesh->maxCurvature[1] = mesh->curvatures[v].coord[1];
		if (mesh->curvatures[v].coord[0] < mesh->minCurvature[0])  mesh->minCurvature[0] = mesh->curvatures[v].coord[0];
		if (mesh->curvatures[v].coord[1] < mesh->minCurvature[1])  mesh->minCurvature[1] = mesh->curvatures[v].coord[1];
		SET_MIN_MAX(mesh->minMeanCurv, mesh->maxMeanCurv, AVG2V(mesh->curvatures[v].coord));

	} // End of Compute vertex curvatures

	printf("# curvature computed: max (%f ~ %f), min (%f ~ %f)\n", mesh->minCurvature[0], mesh->maxCurvature[0], mesh->minCurvature[1], mesh->maxCurvature[1]);
	printf("# mean curvatures   : (%f ~ %f)\n", mesh->minMeanCurv, mesh->maxMeanCurv);
}

