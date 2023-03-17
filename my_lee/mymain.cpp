#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <float.h>

#include "myglm.h"
//#include "mygeom.h"

using namespace std;
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

//----------------------------------------------------
//			Global variables
//----------------------------------------------------
#define BUFSIZE  1024

bool fileHasVertNormal = false;

//----------------------------------------------------
//			Basic functions
//----------------------------------------------------

#define ASSIGN3V3(T,X,Y,Z)		{(T)[0]=(X);(T)[1]=(Y);(T)[2]=(Z);}

#define mymalloc(block, type, n)	block = (type *)malloc((n)*sizeof(type)); \
									if (block == 0)  {	\
										printf("Memory allocation error for \"" #block "\" %.3f MB\n", (double)(n)*sizeof(type)/1000000);	\
										exit(0);\
									}	
									//printf("# Memory allocated for \"" #block "\"\n");



Smesh gObject;
char gFilename[BUFSIZE];
char gProgname[BUFSIZE];

void mytoUpperCase(char *str)
{
	for (int i=0; i<(int)strlen(str); i++) {
		if ('a' <= str[i] && str[i] <= 'z') {
			str[i] += 'A' - 'a';
		}
	}
    printf("fin mytoUpperCase\n");
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
    printf("fin mygetExtension\n");
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
    printf("fin getProgName\n");	
}


// void myreadObjModel(Smesh *mesh, char *filename)
// {
	// double		x_center, y_center, z_center;		//center position of data
	// double	    x_half_range, y_half_range, z_half_range;	//half range in each direction
	// double	    max_half_range;				//maximum of x,y,z_half_range  
	// Svertex *verts;
	// int nverts;

	// GLMmodel* model = glmReadOBJ(filename);
	// //printf("%d vertices, %d normals, %d triangles\n", model->numvertices, model->numnormals, model->numtriangles);

	// int _num_vertices = model->numvertices;
	// GLfloat* vert = model->vertices;
	// double        x_upper_bound = -8.0e30;	//upper limit of data in x
	// double        x_lower_bound = 8.0e+30;	//lower limit of data in x	
	// double        y_upper_bound = -8.0e+30;	//upper limit of data in y 
	// double        y_lower_bound = 8.0e+30;	//lower limit of data in y
	// double        z_upper_bound = -8.0e+30;	//upper limit of data in z
	// double        z_lower_bound = 8.0e+30;	//lower limit of data in z

	// int i, vid;
	// Svector3 v3;
	// ClinkedList<Svector3> **normals;
	// mymalloc(normals, ClinkedList<Svector3> *, _num_vertices);
	// for (i=0; i<_num_vertices; i++) {
	// 	normals[i] = new ClinkedList<Svector3>;
	// }


	// mesh->nverts = 0; //_num_vertices;
	// mymalloc(mesh->verts, Svertex, model->numtriangles*3);
	// int ndx = 3;
	// for(i=0; i<_num_vertices; i++) {
	// 	float tmp_nx = model->normals[ndx];
	// 	float tmp_x = vert[ndx++];
	// 	float tmp_ny = model->normals[ndx];
	// 	float tmp_y = vert[ndx++];
	// 	float tmp_nz = model->normals[ndx];
	// 	float tmp_z = vert[ndx++];

		
	// 	if(i < 10) {
	// 		cout << tmp_x << ", " << tmp_y << ", " << tmp_z << endl;
	// 	}

	// 	mesh->verts[mesh->nverts].id = mesh->nverts;
	// 	mesh->verts[mesh->nverts].edges = 0;
	// 	mesh->verts[mesh->nverts].tris = 0;
	// 	mesh->verts[mesh->nverts].nedges = mesh->verts[mesh->nverts].ntris = 0;

	// 	ASSIGN3V3(mesh->verts[mesh->nverts].coords, tmp_x, tmp_y, tmp_z);
	// 	mesh->nverts++;

	// 	// update bounding box param.
	// 	x_upper_bound = (x_upper_bound < tmp_x)? tmp_x:x_upper_bound;
	// 	x_lower_bound = (x_lower_bound > tmp_x)? tmp_x:x_lower_bound;
	// 	y_upper_bound = (y_upper_bound < tmp_y)? tmp_y:y_upper_bound;
	// 	y_lower_bound = (y_lower_bound > tmp_y)? tmp_y:y_lower_bound;
	// 	z_upper_bound = (z_upper_bound < tmp_z)? tmp_z:z_upper_bound;
	// 	z_lower_bound = (z_lower_bound > tmp_z)? tmp_z:z_lower_bound; 
	// }

	// // Normalize the dataset to [-1.0, 1.0] range
	// x_center = (x_upper_bound + x_lower_bound)/2.0;
	// y_center = (y_upper_bound + y_lower_bound)/2.0;
	// z_center = (z_upper_bound + z_lower_bound)/2.0;

	// x_half_range = (x_upper_bound - x_lower_bound)/2.0;
	// y_half_range = (y_upper_bound - y_lower_bound)/2.0;
	// z_half_range = (z_upper_bound - z_lower_bound)/2.0;

	// max_half_range = (x_half_range >= y_half_range)? x_half_range:y_half_range;
	// max_half_range = (max_half_range >= z_half_range)? max_half_range:z_half_range;

	// cout << x_center << ", " << y_center << ", " << z_center << endl;
	// cout << max_half_range << endl;

	// ASSIGN3V3(mesh->center, x_center, y_center, z_center);
	// mesh->extent = max_half_range;
	
	// int _num_triangles = model->numtriangles;
	// mesh->ntris = 0;
	// mymalloc(mesh->tris, Striangle, _num_triangles);

	// int edge_num = 0;
	// for(int k = 0; k<_num_triangles; k++) {
	// 	int vert[3], nid[3];
	// 	vert[0] = (model->triangles[k]).vindices[0]-1;
	// 	vert[1] = (model->triangles[k]).vindices[1]-1;
	// 	vert[2] = (model->triangles[k]).vindices[2]-1;

	// 	nid[0] = (model->triangles[k]).nindices[0]-1;
	// 	nid[1] = (model->triangles[k]).nindices[1]-1;
	// 	nid[2] = (model->triangles[k]).nindices[2]-1;

	// 	if(k == 828) {
	// 		cout << vert[0] << ", " << vert[1] << ", " << vert[2] << endl;
	// 	}

	// 	for (int j=0; j<3; j++) {
	// 		v3.nid = nid[j];
	// 		v3.id = vert[j];
	// 		ASSIGN3V(v3.v, model->normals+nid[j]*3);

	// 		if (normals[v3.id]->isEmpty()) {
	// 			ASSIGN3V(mesh->verts[vert[j]].normal, v3.v);
	// 			normals[v3.id]->pushBack(v3);
	// 		} else if ((vid = findNormal(model, normals, &v3)) < 0) {
	// 			mesh->verts[mesh->nverts].id = mesh->nverts;
	// 			mesh->verts[mesh->nverts].edges = 0;
	// 			mesh->verts[mesh->nverts].tris = 0;
	// 			mesh->verts[mesh->nverts].nedges = mesh->verts[mesh->nverts].ntris = 0;

	// 			ASSIGN3V(mesh->verts[mesh->nverts].coords, mesh->verts[vert[j]].coords);
	// 			ASSIGN3V(mesh->verts[mesh->nverts].normal, v3.v);
	// 			v3.id = mesh->nverts;
	// 			normals[vert[j]]->pushBack(v3);
	// 			vert[j] = mesh->nverts;
	// 			mesh->nverts++;
	// 		} else {
	// 			vert[j] = vid;
	// 		}
	// 	}

	// 	mesh->tris[mesh->ntris].id = mesh->ntris;
	// 	ASSIGN3V3(mesh->tris[mesh->ntris].verts, &(mesh->verts[vert[0]]), &(mesh->verts[vert[1]]), &(mesh->verts[vert[2]]));
	// 	mesh->ntris++;
	// }
	// int _num_edges = edge_num;
	// int _num_materials = 0, _num_textures = 0;
	// // read texture information
	// glmDelete(model);

	// for (i=0; i<_num_vertices; i++) {
	// 	delete normals[i];
	// }
	// myfree(normals);

// 	printf("fin myreadObjModel\n");
// }

void myresetSmesh(Smesh *mesh)
{
	mesh->verts = 0;
	mesh->tris = 0;
	mesh->nverts = mesh->ntris = 0;

	mesh->curvatures = 0;
	mesh->edges = 0;
	mesh->nedges = 0;
	mesh->name = "";
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

	mymalloc(mesh->tris, Striangle, 3*(mesh->ntris));
	k = 0;
	while (fscanf(fp, "%d", &num_v) != EOF)  { 
		fscanf(fp, "%d", &vert0);
		fscanf(fp, "%d", &vert1);
		for (j=0;j<num_v - 2;j++)  {
			fscanf(fp, "%d", &vert2);

			printf("%d\n", j);

			std::cout << "toto:" << mesh->tris[k].verts[0] << std::endl;
		
			std::cout << "titi" << &(mesh->verts[vert0]) << std::endl;

			mesh->tris[k].verts[0] = &(mesh->verts[vert0]);
			mesh->tris[k].verts[1] = &(mesh->verts[vert1]);
			mesh->tris[k].verts[2] = &(mesh->verts[vert2]);
			
			vert1 = vert2;
			mesh->tris[k].id = k;
			k++;
		}
		if (color_present) {
			// read in the color
			fscanf(fp,"%d %d %d\n", &color[0], &color[1], &color[2]);
			for(j = k-1; j >= k - num_v +  2; j--)  {
				mesh->tris[j].color[0] = color[0];
				mesh->tris[j].color[1] = color[1];
				mesh->tris[j].color[2] = color[2];
			}
		}
	}
	mesh->ntris = k;

	printf("      Read %d Triangles;  %d Vertices\n", mesh->ntris, mesh->nverts);
	printf("      the range of volume is:\n");

	return;
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
	} else {
		printf("# Unknown file format: %s\n", ext);
		exit(-1);
	}
}



void myprocessArg()
{
	char *progname;
	char *s;
    //sprintf(gFilename, "%s", "/home/pelissier/These/papier_internationale2/Resultats/dinosaur/dinosaur.obj");
	sprintf(gFilename, "%s", "/home/pelissier/These/papier_internationale2/Code/lee/saliency_1_0/src/sphere.off");
	printf("## read %s\n", gFilename);
	mygetProgName(gProgname, gFilename);
    printf("fin processArg\n");	
}



int main()
{
	myprocessArg();
	myreadModel(&gObject, gFilename);
    printf("gFilename %s\n", gFilename);
    printf("gProgname %s\n", gProgname);

}


