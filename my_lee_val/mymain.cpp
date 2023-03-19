#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <float.h>
#include <cmath>
#include <cfloat>

#include "my_structs.h"
#include "mylinkedlist.h"
#include "mymacro.h"

// General --------------------------------------
#define CURV_CURVATURE_COMPUTATION_EPPSILON 0.000001
#define CURV_ELLIPSOID_LAMBDA_DIFF 0.001

#define BUFSIZE  1024
#define G_TABLE_RANGE  3
#define TABLE_SIZE  31
#define GRID_NUM  64 

//----------------------------------------------------
//			Global variables
//----------------------------------------------------
Smesh gObject;
char gFilename[BUFSIZE];
char gProgname[BUFSIZE];

bool fileHasVertNormal = false;

SsaliencyList glSaliency;
SMsaliencyList glMsaliency;
Sfloat2 *glMgaussianVar;
float glGaussianVar[2] = { 1, 2 };
float glStartMvar = 2;
float glMvarInc = 1;
int glNumMultiSaliency = 5;
bool useAreaForSaliency = true;
bool useAmplification = false;
bool useHierarchicalScales = false;
float glUnitRatio = 0.003;  // unit ratio relative the diagonal of bounding box
float glCenterSurroundFactor = 2;
int glGridNum = GRID_NUM;

SlookupTable glGaussSqrTable;
SlookupTable glGaussTable;
int glGaussTableSize = TABLE_SIZE;
float glSmoothSize = 1;

bool isAllowToReadFromFile = true;
bool useSupress = true;

bool *gVisited = 0;
float *gDistance = 0;

Sgrids glGrid;



//----------------------------------------------------
//			Basic functions
//----------------------------------------------------

#define ASSIGN3V3(T,X,Y,Z)		{(T)[0]=(X);(T)[1]=(Y);(T)[2]=(Z);}

//------------ Matrix
#define SQR(A)					((A)*(A))
#define subtractV3(A,B,C)	(A)[0]=(B)[0]-(C)[0];	(A)[1]=(B)[1]-(C)[1];	(A)[2]=(B)[2]-(C)[2];
#define addV3(A,B,C)		(A)[0]=(B)[0]+(C)[0];	(A)[1]=(B)[1]+(C)[1];	(A)[2]=(B)[2]+(C)[2];
#define multV3S(A,B,S)		(A)[0]=(B)[0]*(S);	(A)[1]=(B)[1]*(S);	(A)[2]=(B)[2]*(S);
#define multM4S(A,B,S)		(A)[0][0]=(B)[0][0]*(S);  (A)[0][1]=(B)[0][1]*(S);  (A)[0][2]=(B)[0][2]*(S);  (A)[0][3]=(B)[0][3]*(S); \
	(A)[1][0]=(B)[1][0]*(S);  (A)[1][1]=(B)[1][1]*(S);  (A)[1][2]=(B)[1][2]*(S);  (A)[1][3]=(B)[1][3]*(S);	\
	(A)[2][0]=(B)[2][0]*(S);  (A)[2][1]=(B)[2][1]*(S);  (A)[2][2]=(B)[2][2]*(S);  (A)[2][3]=(B)[2][3]*(S);	\
	(A)[3][0]=(B)[3][0]*(S);  (A)[3][1]=(B)[3][1]*(S);  (A)[3][2]=(B)[3][2]*(S);  (A)[3][3]=(B)[3][3]*(S);



#define mymalloc(block, type, n)	block = (type *)malloc((n)*sizeof(type)); \
									if (block == 0)  {	\
										printf("Memory allocation error for \"" #block "\" %.3f MB\n", (double)(n)*sizeof(type)/1000000);	\
										exit(0);\
									}	
									//printf("# Memory allocated for \"" #block "\"\n");


void mytoUpperCase(char *str)
{
	for (int i=0; i<(int)strlen(str); i++) {
		if ('a' <= str[i] && str[i] <= 'z') {
			str[i] += 'A' - 'a';
		}
	}
    //printf("fin mytoUpperCase\n");
}

void mygetExtension(char *ext, char *fullpath)
{
	int i;
	int ci, starti, endi;

	starti = endi = 0;
	ci = -1;
	for (i=0; i<(int)strlen(fullpath); i++) {
		if (fullpath[i] == '\\' || fullpath[i] == '/') {
			ci = -1;
			i++;
		} else if (fullpath[i] == '.') {
			ci = 0;
			i++;
		}
		if (ci >= 0) {
			ext[ci++] = fullpath[i];
		}
	}
	if (ci >= 0) {
		ext[ci] = '\0';
	} else {
		ext[0] = '\0';
	}
    //printf("fin mygetExtension\n");
}

void mygetProgName(char *progname, char *fullpath)
{
	int i;
	int ci, starti, endi;

	ci = starti = endi = 0;
	for (i=0; i<(int)strlen(fullpath); i++) {
		if (fullpath[i] == '\\' || fullpath[i] == '/') {
			ci = 0;
			i++;
		} else if (fullpath[i] == '.') {
			progname[ci++] = '\0';
		}
		progname[ci++] = fullpath[i];
	}
	progname[ci] = '\0';
    //printf("fin getProgName\n");	
}


//-----------------  OFF file format --------------------------------------
void myresetSmesh(Smesh *mesh)
{
	mesh->verts = 0;
	mesh->tris = 0;
	mesh->nverts = mesh->ntris = 0;

	mesh->curvatures = 0;
	mesh->edges = 0;
	mesh->nedges = 0;
	mesh->name = strdup("");
}

void myresetSvertex(Svertex *vert)
{
	vert->edges = 0;
	vert->tris = 0;
	vert->nedges = vert->ntris = 0;
}

void myreadOffModel(Smesh *mesh, char *filename)
{
	char		line[1024];
	FILE		*fp;
	int		i, j, k;
	int		num_v;
	int		color[3], color_present;
	int		dummy_int;
	int Num_tris;
	int vert0, vert1, vert2;
	long		file_offset;

	double        x_upper_bound = -8.0e300;	//upper limit of data in x direction
	double        x_lower_bound = 8.0e+300;	//lower limit of data in x direction	
	double        y_upper_bound = -8.0e+300;	//upper limit of data in y direction 
	double        y_lower_bound = 8.0e+300;	//lower limit of data in y direction
	double        z_upper_bound = -8.0e+300;	//upper limit of data in z direction
	double        z_lower_bound = 8.0e+300;	//lower limit of data in z direction
	double		x_center, y_center, z_center;		//center position of data
	double	    x_half_range, y_half_range, z_half_range;	//half range in each direction
	double	    max_half_range;				//maximum of x,y,z_half_range  

	// read model triangles
	if (!(fp = fopen(filename, "rb"))) { 
		fprintf(stderr, "read_model(): can't open input file %s\n", filename);
		exit(-1);
	}

	myresetSmesh(mesh);

	// try to deal with options in file
	fscanf(fp, "%s \n", line);
	for (i=0; i<strlen(line); i++) {
		if (line[i] == 'N') {
			fileHasVertNormal = true;
			//printf("# File has vertex normal\n");
		}
	}

	// count max verts and number of tris
	mesh->nverts = mesh->ntris = 0;
	fscanf(fp, "%d %d %d\n", &(mesh->nverts), &(mesh->ntris), &dummy_int);
	// read all the verts into coords and norms and tris into tri_list
	mymalloc(mesh->verts, Svertex, mesh->nverts);

	x_center = y_center = z_center = 0;
	for (i=0; i<mesh->nverts; i++) { 
		mesh->verts[i].id = i;
		double tmp_x, tmp_y, tmp_z;
		double tmp_nx, tmp_ny, tmp_nz;

		myresetSvertex(&(mesh->verts[i]));
  
		fscanf(fp, "%lf %lf %lf", &tmp_x, &tmp_y, &tmp_z);
		if (fileHasVertNormal) {
			fscanf(fp, " %lf %lf %lf", &tmp_nx, &tmp_ny, &tmp_nz);
			ASSIGN3V3(mesh->verts[i].normal, tmp_nx, tmp_ny, tmp_nz);
		}
		fscanf(fp, "\n");
		mesh->verts[i].coords[0] = tmp_x;
		mesh->verts[i].coords[1] = tmp_y;
		mesh->verts[i].coords[2] = tmp_z;
		x_upper_bound = (x_upper_bound < mesh->verts[i].coords[0])? mesh->verts[i].coords[0]:x_upper_bound;
		x_lower_bound = (x_lower_bound > mesh->verts[i].coords[0])? mesh->verts[i].coords[0]:x_lower_bound;
		y_upper_bound = (y_upper_bound < mesh->verts[i].coords[1])? mesh->verts[i].coords[1]:y_upper_bound;
		y_lower_bound = (y_lower_bound > mesh->verts[i].coords[1])? mesh->verts[i].coords[1]:y_lower_bound;
		z_upper_bound = (z_upper_bound < mesh->verts[i].coords[2])? mesh->verts[i].coords[2]:z_upper_bound;
		z_lower_bound = (z_lower_bound > mesh->verts[i].coords[2])? mesh->verts[i].coords[2]:z_lower_bound; 
		x_center += tmp_x;
		y_center += tmp_y;
		z_center += tmp_z;
	}
    
	// Normalize the dataset to [-1.0, 1.0] range
	x_center = (x_upper_bound + x_lower_bound)/2.0;
	y_center = (y_upper_bound + y_lower_bound)/2.0;
	z_center = (z_upper_bound + z_lower_bound)/2.0;

	x_half_range = (x_upper_bound - x_lower_bound)/2.0;
	y_half_range = (y_upper_bound - y_lower_bound)/2.0;
	z_half_range = (z_upper_bound - z_lower_bound)/2.0;

	max_half_range = (x_half_range >= y_half_range)? x_half_range:y_half_range;
	max_half_range = (max_half_range >= z_half_range)? max_half_range:z_half_range;

	ASSIGN3V3(mesh->center, x_center, y_center, z_center);
	mesh->extent = max_half_range;

	// figure out how many tris to read
	file_offset = ftell(fp);
	fgets(line, sizeof(line), fp);
	sscanf(line, "%d", &k);

	Num_tris = (k - 2);
	strtok(line, " ");
	i = 1;
	while (strtok(NULL," ") != NULL) i++;
	color_present = (i > k+2);

	for(i=1; i < mesh->ntris; i++)  {
		fgets(line, sizeof(line), fp);
		sscanf(line, "%d", &k);
		Num_tris += (k - 2);
	}
	fseek(fp, file_offset, 0);

	mymalloc(mesh->tris, Striangle, (mesh->ntris));
	
	k = 0;

	//printf("\n>>>%d\n", mesh->ntris);
	int rv;
	do {
		rv = fscanf(fp, "%d%*[ ]%d%*[ ]%d%*[ ]%d", &num_v, &vert0, &vert1, &vert2);
		if (rv == 1) {
			/* read just a word -- no data character after it */
		} else if (rv >= 2) {
			mesh->tris[k].verts[0] = &(mesh->verts[vert0]);
			mesh->tris[k].verts[1] = &(mesh->verts[vert1]);
			mesh->tris[k].verts[2] = &(mesh->verts[vert2]);
			
			//vert1 = vert2;
			mesh->tris[k].id = k;
			k++;
			//printf("%d %d %d\n",vert0, vert1, vert2);
		}
	} while (rv != EOF);

	mesh->ntris = k;
	printf("myreadOffModel : Read %d Triangles;  %d Vertices\n", mesh->ntris, mesh->nverts);
	//printf("      the range of volume is:\n");

	return;
}


//-----------------  READ file format --------------------------------------

void normalizeModel(Smesh *mesh)
{
	char		line[1024];
	FILE		*fp;
	long		i, j, k;
	long		num_v;
	int		color[3], color_present;
	int		dummy_int;
	int Num_tris;
	int vert0, vert1, vert2;
	long		file_offset;

	double        x_upper_bound = -8.0e300;	//upper limit of data in x direction
	double        x_lower_bound = 8.0e+300;	//lower limit of data in x direction	
	double        y_upper_bound = -8.0e+300;	//upper limit of data in y direction 
	double        y_lower_bound = 8.0e+300;	//lower limit of data in y direction
	double        z_upper_bound = -8.0e+300;	//upper limit of data in z direction
	double        z_lower_bound = 8.0e+300;	//lower limit of data in z direction
	double		x_center, y_center, z_center;		//center position of data
	double	    x_half_range, y_half_range, z_half_range;	//half range in each direction
	double	    max_half_range;				//maximum of x,y,z_half_range  

	x_center = y_center = z_center = 0;
	for (i=0; i<mesh->nverts; i++) { 
		double tmp_x = mesh->verts[i].coords[0];
		double tmp_y = mesh->verts[i].coords[1];
		double tmp_z = mesh->verts[i].coords[2];
		x_upper_bound = (x_upper_bound < mesh->verts[i].coords[0])? mesh->verts[i].coords[0]:x_upper_bound;
		x_lower_bound = (x_lower_bound > mesh->verts[i].coords[0])? mesh->verts[i].coords[0]:x_lower_bound;
		y_upper_bound = (y_upper_bound < mesh->verts[i].coords[1])? mesh->verts[i].coords[1]:y_upper_bound;
		y_lower_bound = (y_lower_bound > mesh->verts[i].coords[1])? mesh->verts[i].coords[1]:y_lower_bound;
		z_upper_bound = (z_upper_bound < mesh->verts[i].coords[2])? mesh->verts[i].coords[2]:z_upper_bound;
		z_lower_bound = (z_lower_bound > mesh->verts[i].coords[2])? mesh->verts[i].coords[2]:z_lower_bound; 
		x_center += tmp_x;
		y_center += tmp_y;
		z_center += tmp_z;
	}
    
	// Normalize the dataset to [-1.0, 1.0] range
	x_center = (x_upper_bound + x_lower_bound)/2.0;
	y_center = (y_upper_bound + y_lower_bound)/2.0;
	z_center = (z_upper_bound + z_lower_bound)/2.0;

	x_half_range = (x_upper_bound - x_lower_bound)/2.0;
	y_half_range = (y_upper_bound - y_lower_bound)/2.0;
	z_half_range = (z_upper_bound - z_lower_bound)/2.0;

	max_half_range = (x_half_range >= y_half_range)? x_half_range:y_half_range;
	max_half_range = (max_half_range >= z_half_range)? max_half_range:z_half_range;

	ASSIGN3V3(mesh->center, x_center, y_center, z_center);
	mesh->extent = max_half_range;

	for (i=0;i<mesh->nverts;i++) {   
		mesh->verts[i].coords[0] =  (mesh->verts[i].coords[0]-x_center)/max_half_range;
		mesh->verts[i].coords[1] =  (mesh->verts[i].coords[1]-y_center)/max_half_range;
		mesh->verts[i].coords[2] =  (mesh->verts[i].coords[2]-z_center)/max_half_range;
	}
	ASSIGN3V3(mesh->center, 0, 0, 0);
	mesh->extent = 1;
}

void myreadModel(Smesh *mesh, char *filename)
{		
	char name[256], ext[256];
	mygetProgName(name, filename);
	mygetExtension(ext, filename);
	mytoUpperCase(ext);
	printf("# Read \'%s\' in \'%s\' format\n", name, ext);

    mesh->edges = 0;
	mesh->curvatures = 0;
	mesh->nedges = 0;
	mesh->tris = 0;
	mesh->verts = 0;
	mesh->nverts = mesh->ntris = 0;

    if (strcmp(ext, "OFF") == 0) {
		myreadOffModel(mesh, filename);
	} 
	// if (strcmp(ext, "OBJ") == 0) {
	// 	myreadObjModel(mesh, filename);
	// }
	else {
		printf("# Unknown file format: %s\n", ext);
		exit(-1);
	}

	if (mesh->extent < 1) {
		normalizeModel(mesh);
	}

	// assign ids to triangles
	for (int i = 0; i < mesh->ntris; i++)
	{
		mesh->tris[i].id = i;
	}
	mymalloc(mesh->name, char, sizeof(name)+1);
	sprintf(mesh->name, "%s", name);
	//mesh->name = name;
	printf("# %d vertices, %d triangles", mesh->nverts, mesh->ntris);
	printf(", extent: %f\n", mesh->extent);
	printf(">>> 1. Fin myreadModel\n");
}

//-----------------  Compute Normals --------------------------------------

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

void mycomputeNormals(Smesh *surf)
{
	computeTriNormals(surf);
	computeVertNormals(surf);
	printf(">>> 2. Fin ComputeNormal\n");
}



//-----------------  Compute Edges --------------------------------------
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

inline Svertex *getOtherVertInEdge(Sedge *edge, Svertex *vert)
{
	return ((edge->verts[0]->id != vert->id)? edge->verts[0]:edge->verts[1]);
}

void mycomputeEdges(Smesh *mesh)
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
	printf(">>> 3. Fin computeEdges\n");
}

//-----------------  Compute Curvature --------------------------------------
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
void mycomputeCurvatures(Smesh *mesh)
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

		if (isnan(mesh->curvatures[v].coord[0]) || isnan(mesh->curvatures[v].coord[1]))
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
	printf(">>> 4. Fin computeCurvatures\n");
}

//-----------------  Saliency --------------------------------------

void myinitSsaliencyList(SsaliencyList *sal, Smesh *mesh)
{
	int n = mesh->nverts;
	int i;

	sal->nlist = n;
	mymalloc(sal->list, Ssaliency, sal->nlist);
	for (i=0; i<2; i++) {
		sal->var[i] = mesh->unit*glGaussianVar[i];
	}
	printf(">>> 6. Fin myinitSsaliencyList\n");
}

void myinitSMsaliencyList(SMsaliencyList *sal, Smesh *mesh)
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
			//printdg("var[%d][%d] = %f\n", i, k, sal->var[i].val[k] / mesh->unit);
		}
	}
	printf(">>> 7. Fin myinitSMsaliencyList\n");
}

inline float mycomputeRawSaliency(float gaussval[2])
{
	return ABS( (gaussval[0]) - (gaussval[1]) );
}

void mysupressSaliency(Smesh *mesh, SsaliencyList *sal)
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

void mynonlinearlyNormalizeSaliency(Smesh *mesh, SsaliencyList *sal)
{
	float mins = FLT_MAX, maxs = -FLT_MAX;
	int i;

	if (useSupress) {
		mysupressSaliency(mesh, sal);
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

int myreadSaliencyBin(char *filename, Smesh *mesh, SsaliencyList *sal)
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
		sal->list[i].rawSaliency = mycomputeRawSaliency(sal->list[i].gaussian);
	}
	mynonlinearlyNormalizeSaliency(mesh, sal);

	fclose(fp);
	return 1;
}

int myreadSaliency(char *filename, Smesh *mesh, SsaliencyList *sal)
{
	return myreadSaliencyBin(filename, mesh, sal);
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

inline int getGridIdxx(int x, int y, int z)
{ 
	return (x + glGridNum*(y + glGridNum*z));
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

inline float gauss(SlookupTable *table, float normdistsqr)
{
	return gaussFromSqrTable(table, normdistsqr);
}

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
		int idx = getGridIdxx(i0, i1, i2);
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

	saliency = mycomputeRawSaliency(gaussian);
	ASSIGN2V(gaussianlist, gaussian);
	return saliency;
} 

void mycomputeSaliency(Smesh *mesh, SsaliencyList *sal)
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
		
		//printProgress(i, mesh->nverts);
	}
	myfree(verts);
	myfree(vertsSmall);

	mynonlinearlyNormalizeSaliency(mesh, sal);
	myfree(gVisited);
	myfree(gDistance);
}

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

int writeSaliency(char *filename, Smesh *mesh, SsaliencyList *sal)
{
	return writeSaliencyBin(filename, mesh, sal);
}

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

int readMeshSaliency(char *filename, Smesh *mesh)
{
	return readMeshSaliencyBin(filename, mesh);
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
		int idx = getGridIdxx(i0, i1, i2);
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

inline void getVertNeighbors(Svertex ***nei, int *nnei, Svertex *vert, Smesh *mesh, float var)
{
	getVertNeighborsGrid(nei, nnei, vert, mesh, var);
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
		//printProgress(i, mesh->nverts);
	}
	myfree(verts);
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
	printf("\n>>> 9. Fin getMultiSaliencyFromGaussians\n");
}

//-----------------  MAIN --------------------------------------
//printf("\n>>> 5. Fin myinitSsaliencyList\n");

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

inline int getGridIdx(int x[3]) 
{ 
	return getGridIdxx(x[0], x[1], x[2]);
}

inline float computeMeanCurvature(Smesh *mesh, int vid)
{
	return AVG2V(mesh->curvatures[vid].coord);
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

void initFunctions()
{
	initGrid(&glGrid, &gObject, glGridNum);
	buildGrid(&glGrid, &gObject);
	buildGaussTable(&glGaussTable, glGaussTableSize);
	buildGaussTableSqr(&glGaussSqrTable, glGaussTableSize);
	printf("\n>>> 5. Fin Init functions\n");
}

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

void mygetResultFilename(char *filename, char *progname, float var[2])
{
	char *area = strdup(""), *suppress = strdup("");

	if (useAreaForSaliency)  area = strdup("a");

	if (useHierarchicalScales) {
		sprintf(filename, "results/%s_%.1f_%.1f%s%s_d", progname, var[0], var[1], area, suppress);
	} else {
		sprintf(filename, "results/%s_%.1f_%.1f%s%s", progname, var[0], var[1], area, suppress);
	}
	sprintf(filename, "%s_r%.1f", filename, glUnitRatio*100.0);
	sprintf(filename, "%s.sal", filename);

	//printdg("FILE: %s\n", filename);
}

void myprocessArg()
{
	char *progname;
	char *s;
	sprintf(gFilename, "%s", "/home/pelissier/Téléchargements/my_lee_val/mesh_off/cube.off");
	printf("## read %s\n", gFilename);
	mygetProgName(gProgname, gFilename);
}


int main()
{
	myprocessArg();
    printf("gProgname %s, gFilename: %s\n", gProgname, gFilename);
	myreadModel(&gObject, gFilename);
	float scaleFactor = 1.2 * gObject.extent;
	mycomputeNormals(&gObject);
	mycomputeEdges(&gObject);
	mycomputeCurvatures(&gObject);
	getSaliencyUnit(&gObject);

	//----- compute saliency
	char buf[BUFSIZE];
	int i, j;

	initFunctions();

	// // START_TIMERI(0);
	myinitSsaliencyList(&glSaliency, &gObject);
	myinitSMsaliencyList(&glMsaliency, &gObject);

	// printf("--->%d", glMsaliency.nhier);
	for (i=0; i<glMsaliency.nhier; i++) {
	// 	//START_TIMERI(1);

		ASSIGN2V(glSaliency.var, glMsaliency.var[i].val);
		mygetResultFilename(buf, gProgname, glMgaussianVar[i].val);

		if (!(isAllowToReadFromFile && myreadSaliency(buf, &gObject, &glSaliency))) {
				mycomputeSaliency(&gObject, &glSaliency);
				writeSaliency(buf, &gObject, &glSaliency);
		}
		
		for (j=0; j<glSaliency.nlist; j++) {
			ASSIGN2V(glMsaliency.list[j].gaussian[i].val, glSaliency.list[j].gaussian);
			glMsaliency.list[j].rawSaliency[i] = glSaliency.list[j].rawSaliency;
		}
		//printf(" # Time for computing %dth saliency out of %d: %.3f sec\n", i+1, glMsaliency.nhier, END_TIMERI(1));
	}
	printf("\n>>> 8. Fin boucle for main\n");

	getMultiSaliencyFromGaussians(&gObject, &glMsaliency);



	printf("\n>>> Fin main\n");

}


