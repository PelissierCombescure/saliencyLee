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
#include <fstream.h>
#include <iostream.h>
#include <string.h>
#include "macro.h"
#include "geom.h"
#include "main.h"
#include "glm.h"
#include "linkedlist.h"
#include "ply.h"

//using namespace std;

bool BIG_ENDIAN = false;


//----------------------------------------------------
//			Global variables
//----------------------------------------------------
bool fileHasVertNormal = false;
//----------------------------------------------------
//			External variables
//----------------------------------------------------

//----------------------------------------------------
//			External functions
//----------------------------------------------------

//----------------------------------------------------
//			functions
//----------------------------------------------------


bool isFileExist(char *filename)
{
	bool isexist = false;
	FILE *fp = fopen(filename, "r");
	if (fp == 0) {
		isexist = false;
	} else {
		isexist = true;
		fclose(fp);
	}

	return isexist;
}

//----------------------------
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


//----------------------------

void writeOffModel(Smesh *mesh, char *filename);


/* read the model polygons and store them as tris

  [ST][C][N][4][n]OFF	# Header keyword
   Vertices, possibly with normals, colors, and/or texture coordinates, in that order,
    if the prefixes N, C, ST are present.
 */
void readOffModel(Smesh *mesh, char *filename)
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

	resetSmesh(mesh);

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

		resetSvertex(&(mesh->verts[i]));
  
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

	mymalloc(mesh->tris, Striangle, mesh->ntris);
	k = 0;
	while (fscanf(fp, "%d", &num_v) != EOF)  { 
		fscanf(fp, "%d", &vert0);
		fscanf(fp, "%d", &vert1);
		for (j=0;j<num_v - 2;j++)  {
			fscanf(fp, "%d", &vert2);
  
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

	//printf("      Read %d Triangles;  %d Vertices\n", mesh->ntris, mesh->nverts);
	//printf("      the range of volume is:\n");

	return;
}





/* write the model polygons and store them as tris
 */
void writeOffModel(Smesh *mesh, char *filename)
{ 
	FILE *fp;
	long i;

	// read model triangles
	if (!(fp = fopen(filename, "w"))) { 
		fprintf(stderr, "write_model(): can't open input file %s\n", filename);
		exit(-1);
	}

	if (fileHasVertNormal)  fprintf(fp, "N");
	fprintf(fp, "OFF\n");
	fprintf(fp, "%d %d %d\n", (mesh->nverts), (mesh->ntris), 0);

	// write vertices
	for (i=0; i<mesh->nverts; i++) {
		fprintf(fp, "%f %f %f", LIST_COMMA3V(mesh->verts[i].coords));
		if (fileHasVertNormal) {
			fprintf(fp, " %f %f %f", LIST_COMMA3V(mesh->verts[i].normal));
		}
		fprintf(fp, "\n");
	}

	// write triangles
	for (i=0; i<mesh->ntris; i++) {
		fprintf(fp, "%d %d %d %d\n", 3, mesh->tris[i].verts[0]->id, mesh->tris[i].verts[1]->id, mesh->tris[i].verts[2]->id);
	}

	fclose(fp);
}

//-----------------  OBJ file format --------------------------------------
typedef struct Svector3 {
	float v[3];
	int id;
	int nid;
} Svector3;


int findNormal(GLMmodel* model, ClinkedList<Svector3> **normals, Svector3 *normal)
{
	ClinkedList<Svector3> *vnormals;
	Svector3 n;
	int i, j, vid = -1;

	vnormals = normals[normal->id];
	for (i=0; i<vnormals->getNum(); i++) {
		if (i == 0) {
			n = vnormals->getFirst();
		} else {
			n = vnormals->getNext();
		}
		//if (VEC3_SIM_EQ(n.v, normal->v, 0.001))
		if (n.nid == normal->nid) {
			return n.id;
		}
	}
	return vid;
}


void readObjModel(Smesh *mesh, char *filename)
{
	double		x_center, y_center, z_center;		//center position of data
	double	    x_half_range, y_half_range, z_half_range;	//half range in each direction
	double	    max_half_range;				//maximum of x,y,z_half_range  
	Svertex *verts;
	int nverts;

	GLMmodel* model = glmReadOBJ(filename);
	//printf("%d vertices, %d normals, %d triangles\n", model->numvertices, model->numnormals, model->numtriangles);

	int _num_vertices = model->numvertices;
	GLfloat* vert = model->vertices;
	double        x_upper_bound = -8.0e30;	//upper limit of data in x
	double        x_lower_bound = 8.0e+30;	//lower limit of data in x	
	double        y_upper_bound = -8.0e+30;	//upper limit of data in y 
	double        y_lower_bound = 8.0e+30;	//lower limit of data in y
	double        z_upper_bound = -8.0e+30;	//upper limit of data in z
	double        z_lower_bound = 8.0e+30;	//lower limit of data in z

	int i, vid;
	Svector3 v3;
	ClinkedList<Svector3> **normals;
	mymalloc(normals, ClinkedList<Svector3> *, _num_vertices);
	for (i=0; i<_num_vertices; i++) {
		normals[i] = new ClinkedList<Svector3>;
	}


	mesh->nverts = 0; //_num_vertices;
	mymalloc(mesh->verts, Svertex, model->numtriangles*3);
	int ndx = 3;
	for(i=0; i<_num_vertices; i++) {
		float tmp_nx = model->normals[ndx];
		float tmp_x = vert[ndx++];
		float tmp_ny = model->normals[ndx];
		float tmp_y = vert[ndx++];
		float tmp_nz = model->normals[ndx];
		float tmp_z = vert[ndx++];

		
		if(i < 10) {
			cout << tmp_x << ", " << tmp_y << ", " << tmp_z << endl;
		}

		mesh->verts[mesh->nverts].id = mesh->nverts;
		mesh->verts[mesh->nverts].edges = 0;
		mesh->verts[mesh->nverts].tris = 0;
		mesh->verts[mesh->nverts].nedges = mesh->verts[mesh->nverts].ntris = 0;

		ASSIGN3V3(mesh->verts[mesh->nverts].coords, tmp_x, tmp_y, tmp_z);
		mesh->nverts++;

		// update bounding box param.
		x_upper_bound = (x_upper_bound < tmp_x)? tmp_x:x_upper_bound;
		x_lower_bound = (x_lower_bound > tmp_x)? tmp_x:x_lower_bound;
		y_upper_bound = (y_upper_bound < tmp_y)? tmp_y:y_upper_bound;
		y_lower_bound = (y_lower_bound > tmp_y)? tmp_y:y_lower_bound;
		z_upper_bound = (z_upper_bound < tmp_z)? tmp_z:z_upper_bound;
		z_lower_bound = (z_lower_bound > tmp_z)? tmp_z:z_lower_bound; 
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

	cout << x_center << ", " << y_center << ", " << z_center << endl;
	cout << max_half_range << endl;

	ASSIGN3V3(mesh->center, x_center, y_center, z_center);
	mesh->extent = max_half_range;
	
	int _num_triangles = model->numtriangles;
	mesh->ntris = 0;
	mymalloc(mesh->tris, Striangle, _num_triangles);

	int edge_num = 0;
	for(int k = 0; k<_num_triangles; k++) {
		int vert[3], nid[3];
		vert[0] = (model->triangles[k]).vindices[0]-1;
		vert[1] = (model->triangles[k]).vindices[1]-1;
		vert[2] = (model->triangles[k]).vindices[2]-1;

		nid[0] = (model->triangles[k]).nindices[0]-1;
		nid[1] = (model->triangles[k]).nindices[1]-1;
		nid[2] = (model->triangles[k]).nindices[2]-1;

		if(k == 828) {
			cout << vert[0] << ", " << vert[1] << ", " << vert[2] << endl;
		}

		for (int j=0; j<3; j++) {
			v3.nid = nid[j];
			v3.id = vert[j];
			ASSIGN3V(v3.v, model->normals+nid[j]*3);

			if (normals[v3.id]->isEmpty()) {
				ASSIGN3V(mesh->verts[vert[j]].normal, v3.v);
				normals[v3.id]->pushBack(v3);
			} else if ((vid = findNormal(model, normals, &v3)) < 0) {
				mesh->verts[mesh->nverts].id = mesh->nverts;
				mesh->verts[mesh->nverts].edges = 0;
				mesh->verts[mesh->nverts].tris = 0;
				mesh->verts[mesh->nverts].nedges = mesh->verts[mesh->nverts].ntris = 0;

				ASSIGN3V(mesh->verts[mesh->nverts].coords, mesh->verts[vert[j]].coords);
				ASSIGN3V(mesh->verts[mesh->nverts].normal, v3.v);
				v3.id = mesh->nverts;
				normals[vert[j]]->pushBack(v3);
				vert[j] = mesh->nverts;
				mesh->nverts++;
			} else {
				vert[j] = vid;
			}
		}

		mesh->tris[mesh->ntris].id = mesh->ntris;
		ASSIGN3V3(mesh->tris[mesh->ntris].verts, &(mesh->verts[vert[0]]), &(mesh->verts[vert[1]]), &(mesh->verts[vert[2]]));
		mesh->ntris++;
	}
	int _num_edges = edge_num;
	int _num_materials = 0, _num_textures = 0;
	// read texture information
	glmDelete(model);

	for (i=0; i<_num_vertices; i++) {
		delete normals[i];
	}
	myfree(normals);
}

void writeObjModel(Smesh *mesh, char *filename)
{
}


//----------------------- PLY file --------------------------
typedef struct Face {
  int intensity; /* this user attaches intensity to faces */
  int nverts;    /* number of vertex indices in list */
  int* verts;              /* vertex index list */
} Face;

int getSizeOf(int iType)
{
	if (iType == PLY_CHAR)
	{
		return sizeof(char);
	}
	else if (iType == PLY_SHORT)
	{
		return sizeof(short);
	}
	else if (iType == PLY_INT)
	{
		return sizeof(int);
	}
	else if (iType == PLY_UCHAR)
	{
		return sizeof(unsigned char);
	}
	else if (iType == PLY_USHORT)
	{
		return sizeof(unsigned short);
	}
	else if (iType == PLY_UINT)
	{
		return sizeof(unsigned int);
	}
	else if (iType == PLY_FLOAT)
	{
		return sizeof(float);
	}
	else if (iType == PLY_DOUBLE)
	{
		return sizeof(double);
	}

	return 0;
}

void endianTest()
{
	unsigned char EndianTest[2] = { 1, 0 };
	short x;
  
	x = *(short *) EndianTest;

	if (x == 0x10)
	{
		BIG_ENDIAN = true;
	}
	else
	{
		BIG_ENDIAN = false;
	}
}

// function to read model file in OBJ format
void readPlyModel(Smesh *mesh, char* filename) { 
	int j;
	PlyFile *ply;
	int nelems;
	char **elist;
	int file_type;
	float version;
	int nprops;
	int num_elems;
	PlyProperty **plist;
	char *elem_name;
	int num_comments;
	char **comments;
	int num_obj_info;
	char **obj_info;

	double        x_upper_bound = -8.0e300;	//upper limit of data in x
	double        x_lower_bound = 8.0e+300;	//lower limit of data in x	
	double        y_upper_bound = -8.0e+300;	//upper limit of data in y 
	double        y_lower_bound = 8.0e+300;	//lower limit of data in y
	double        z_upper_bound = -8.0e+300;	//upper limit of data in z
	double        z_lower_bound = 8.0e+300;	//lower limit of data in z
	double		x_center, y_center, z_center;		//center position of data
	double	    x_half_range, y_half_range, z_half_range;	//half range in each direction
	double	    max_half_range;				//maximum of x,y,z_half_range  

	endianTest();

	// open a PLY file for reading
	ply = ply_open_for_reading(filename, &nelems, &elist, &file_type, &version);
	if (ply == 0) { 
		fprintf(stderr, "read_model(): can't open input file %s\n", filename);
		exit(-1);
	}

	// print what we found out about the file
	//printf ("version %f\n", version);
	printf ("type %d\n", file_type);

	// go through each kind of element that we learned is in the file
	// and read them

	resetSmesh(mesh);

	//printf("Number of elements = %d\n", nelems);

	for (int m = 0; m < nelems; m++) {

		// get the description of the first element
		elem_name = elist[m];
		plist = ply_get_element_description (ply, elem_name, &num_elems, &nprops);

		// print the name of the element, for debugging 
		//printf ("element %s %d %d\n", elem_name, num_elems, nprops);
		//toLowerCase(elem_name);

		// if we're on vertex elements, read them in
		if (equal_strings ("vertex", elem_name)) {
			int iOffset = 0;
			// set up for getting vertex elements 
			for (int prop = 0; prop<nprops; prop++) 
			{
				if (equal_strings ("x", plist[prop]->name)) {
					plist[prop]->internal_type = PLY_FLOAT;
					plist[prop]->offset = 0;
					ply_get_property (ply, elem_name, (plist[prop]));
				}
				else if (equal_strings ("y", plist[prop]->name)) 
				{
					plist[prop]->internal_type = PLY_FLOAT;
					plist[prop]->offset = sizeof(float);
					ply_get_property (ply, elem_name, (plist[prop]));
				}
				else if (equal_strings ("z", plist[prop]->name)) 
				{
					plist[prop]->internal_type = PLY_FLOAT;
					plist[prop]->offset = 2*sizeof(float);
					ply_get_property (ply, elem_name, (plist[prop]));
				}
			}

			// grab all the vertex elements
			int _num_vertices = num_elems;
			float fvector[3];
			//printf("Number of vertices = %d\n", _num_vertices);
			mesh->nverts = _num_vertices;
			mymalloc(mesh->verts, Svertex, mesh->nverts);

			for (j = 0; j < num_elems; j++) {
				// grab and element from the file
				ply_get_element (ply, (void *)fvector);
				resetSvertex(&(mesh->verts[j]));
				ASSIGN3V(mesh->verts[j].coords, fvector);
				mesh->verts[j].id = j;

				// update bounding box param.
				x_upper_bound = (x_upper_bound < fvector[0])? fvector[0]:x_upper_bound;
				x_lower_bound = (x_lower_bound > fvector[0])? fvector[0]:x_lower_bound;
				y_upper_bound = (y_upper_bound < fvector[1])? fvector[1]:y_upper_bound;
				y_lower_bound = (y_lower_bound > fvector[1])? fvector[1]:y_lower_bound;
				z_upper_bound = (z_upper_bound < fvector[2])? fvector[2]:z_upper_bound;
				z_lower_bound = (z_lower_bound > fvector[2])? fvector[2]:z_lower_bound; 
				
				//if(j%100000==0) printf("V[%d]=(%.2f, %.2f, %.2f)\n", j, fvector[0], fvector[1], fvector[2]);
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
			//printf("max half range = %.6g\n", max_half_range);

			ASSIGN3V3(mesh->center, x_center, y_center, z_center);
			mesh->extent = max_half_range;
		}

		Face face_struct;
		// if we're on face elements, read them in 
		if (equal_strings ("face", elem_name)) {

			// set up for getting face elements 
			for(int prop = 0; prop<nprops; prop++) {
				if (equal_strings ("vertex_indices", plist[prop]->name)
					|| equal_strings ("vertex_index", plist[prop]->name)) 
				{
					plist[prop]->internal_type = PLY_INT;
					plist[prop]->offset = offsetof(Face, verts);
					plist[prop]->count_internal = PLY_INT;
					plist[prop]->count_offset = offsetof(Face, nverts);
					ply_get_property (ply, elem_name, (plist[prop]));
				}
			}

			// grab all the face elements 
			int num_non_triangles = 0;
			int edge_num = 0;
			mesh->ntris = num_elems;
			mymalloc(mesh->tris, Striangle, num_elems);

			for (j = 0; j < num_elems; j++) {
				// grab and element from the file 
				ply_get_element (ply, (void *) &face_struct);

				
				int nverts;
				int vert[3];
				nverts = face_struct.nverts;
				vert[0] = face_struct.verts[0];
				vert[1] = face_struct.verts[1];
				vert[2] = face_struct.verts[2];

				//if(j%100000== 0) {
				//	printf("nverts = %d, F[%d] = (%d, %d, %d)\n", nverts, j, vert[0], vert[1], vert[2]);
				//}
				if(nverts > 3) num_non_triangles++;

				for (int k=0; k<3; k++) {
					mesh->tris[j].verts[k] = &(mesh->verts[vert[k]]);
				}
			}
			int _num_triangles = num_elems;

		}
	}
	int _num_materials  = 0;

	// grab and print out the comments in the file 
	comments = ply_get_comments (ply, &num_comments);
	for (m = 0; m < num_comments; m++)
		printf ("comment = '%s'\n", comments[m]);

	// grab and print out the object information 
	obj_info = ply_get_obj_info (ply, &num_obj_info);
	for (m = 0; m < num_obj_info; m++)
		printf ("obj_info = '%s'\n", obj_info[m]);

	// close the PLY file 
	ply_close (ply);
}

//---------------- SMF (Qslim) -------------------------------------------

void writeSmfModel(Smesh *mesh, char *filename)
{
	FILE *fp;
	int i, j;

	fp = fopen(filename, "w");
	if (fp == NULL)  printf("Cannot open the file \'%s\'\n", filename);

	for (i=0; i<mesh->nverts; i++) {
		fprintf(fp, "v  %f %f %f\n", mesh->verts[i].coords[0], mesh->verts[i].coords[1], mesh->verts[i].coords[2]);
	}
	for (i=0; i<mesh->ntris; i++) {
		fprintf(fp, "f  %d %d %d\n", mesh->tris[i].verts[0]->id+1, mesh->tris[i].verts[1]->id+1, mesh->tris[i].verts[2]->id+1);
	}
	fclose(fp);
}

void readSmfModel(Smesh *mesh, char *filename)
{
	FILE *fp;
	char buf[512];
	int nverts = 0, nfaces = 0;
	int curr, vi, fi;
	int i, j;

	double        x_upper_bound = -8.0e300;	//upper limit of data in x direction
	double        x_lower_bound = 8.0e+300;	//lower limit of data in x direction	
	double        y_upper_bound = -8.0e+300;	//upper limit of data in y direction 
	double        y_lower_bound = 8.0e+300;	//lower limit of data in y direction
	double        z_upper_bound = -8.0e+300;	//upper limit of data in z direction
	double        z_lower_bound = 8.0e+300;	//lower limit of data in z direction
	double		x_center, y_center, z_center;		//center position of data
	double	    x_half_range, y_half_range, z_half_range;	//half range in each direction
	double	    max_half_range;				//maximum of x,y,z_half_range  

	if (mesh->verts != NULL) {
		free(mesh->verts);
		mesh->verts = NULL;
	}
	if (mesh->tris != NULL) {
		free(mesh->tris);
		mesh->tris = NULL;
	}

	fp = fopen(filename, "r");

	// get the number of vertices and triangles
	while (fgets(buf, 512, fp)) {
		if (buf[0] == '#')  continue;
		else if (buf[0] == 'v')  nverts++;
		else if (buf[0] == 'f')  nfaces++;
	}
	mesh->nverts = nverts;
	mesh->verts = (Svertex *)malloc(nverts*sizeof(Svertex));
	mesh->ntris = nfaces;
	mesh->tris = (Striangle *)malloc(nfaces*sizeof(Striangle));

	// get the mesh
	rewind(fp);
	vi = fi = 0;
	while (fgets(buf, 512, fp)) {
		curr = 0;
		while (isspace(buf[curr])) curr++;
		if (buf[curr] == '#')  continue;
		if (buf[curr] == 'v') {
			curr++;
			while (isspace(buf[curr])) curr++;
			mesh->verts[vi].coords[0] = atof(buf+curr);
			while (isdigit(buf[curr]) || buf[curr] == '.' || buf[curr] == '-') curr++;
			while (isspace(buf[curr])) curr++;
			mesh->verts[vi].coords[1] = atof(buf+curr);
			while (isdigit(buf[curr]) || buf[curr] == '.' || buf[curr] == '-') curr++;
			while (isspace(buf[curr])) curr++;
			mesh->verts[vi].coords[2] = atof(buf+curr);
			mesh->verts[vi].id = vi;

			mesh->verts[vi].edges = 0;
			mesh->verts[vi].tris = 0;
			mesh->verts[vi].nedges = mesh->verts[vi].ntris = 0;

			x_upper_bound = (x_upper_bound < mesh->verts[vi].coords[0])? mesh->verts[vi].coords[0]:x_upper_bound;
			x_lower_bound = (x_lower_bound > mesh->verts[vi].coords[0])? mesh->verts[vi].coords[0]:x_lower_bound;
			y_upper_bound = (y_upper_bound < mesh->verts[vi].coords[1])? mesh->verts[vi].coords[1]:y_upper_bound;
			y_lower_bound = (y_lower_bound > mesh->verts[vi].coords[1])? mesh->verts[vi].coords[1]:y_lower_bound;
			z_upper_bound = (z_upper_bound < mesh->verts[vi].coords[2])? mesh->verts[vi].coords[2]:z_upper_bound;
			z_lower_bound = (z_lower_bound > mesh->verts[vi].coords[2])? mesh->verts[vi].coords[2]:z_lower_bound; 

			vi++;
		} else if (buf[curr] == 'f') {
			curr++;
			while (isspace(buf[curr])) curr++;
			mesh->tris[fi].verts[0] = &(mesh->verts[atoi(buf+curr)-1]);
			while (isdigit(buf[curr]) || buf[curr] == '.' || buf[curr] == '-') curr++;
			while (isspace(buf[curr])) curr++;
			mesh->tris[fi].verts[1] = &(mesh->verts[atoi(buf+curr)-1]);
			while (isdigit(buf[curr]) || buf[curr] == '.' || buf[curr] == '-') curr++;
			while (isspace(buf[curr])) curr++;
			mesh->tris[fi].verts[2] = &(mesh->verts[atoi(buf+curr)-1]);

			mesh->tris[fi].id = fi;
			fi++;
		}
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


	fclose(fp);
}


//-----------------------------------------------------------

void getDirName(char *dirname, char *fullpath)
{
	int i;
	int ci, starti, endi;

	ci = starti = endi = 0;
	for (i=0; i<(int)strlen(fullpath); i++) {
		if (fullpath[i] == '\\' || fullpath[i] == '/') {
			ci = 0;
			endi = i;
			i++;
		}
	}

	for (i=0; i<= endi; i++) {
		dirname[i] = fullpath[i];
	}
	if (endi == 0) {
		dirname[0] = '\0';
	} else {
		dirname[i] = '\0';
	}
}


void getProgName(char *progname, char *fullpath)
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
}

void getExtension(char *ext, char *fullpath)
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
}

void toUpperCase(char *str)
{
	for (int i=0; i<(int)strlen(str); i++) {
		if ('a' <= str[i] && str[i] <= 'z') {
			str[i] += 'A' - 'a';
		}
	}
}

void toLowerCase(char *str)
{
	for (int i=0; i<(int)strlen(str); i++) {
		if ('a' <= str[i] && str[i] <= 'z') {
			str[i] += 'a' - 'A';
		}
	}
}


void readModel(Smesh *mesh, char *filename)
{
	char name[256], ext[256];
	getProgName(name, filename);
	getExtension(ext, filename);
	toUpperCase(ext);
	printf("# Read \'%s\' in \'%s\' format\n", name, ext);

	mesh->edges = 0;
	mesh->curvatures = 0;
	mesh->nedges = 0;
	mesh->tris = 0;
	mesh->verts = 0;
	mesh->nverts = mesh->ntris = 0;

	if (strcmp(ext, "OFF") == 0) {
		readOffModel(mesh, filename);
	} else if (strcmp(ext, "OBJ") == 0) {
		readObjModel(mesh, filename);
	} else if (strcmp(ext, "PLY") == 0) {
		readPlyModel(mesh, filename);
	} else if (strcmp(ext, "SMF") == 0) {
		readSmfModel(mesh, filename);
	} else {
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
	printf("# %d vertices, %d triangles", mesh->nverts, mesh->ntris);
	printf(", extent: %f\n", mesh->extent);
}

void writeModel(Smesh *mesh, char *filename)
{
	char name[256], ext[256];
	getProgName(name, filename);
	getExtension(ext, filename);
	toUpperCase(ext);
	printf("# Write \'%s\' in \'%s\' format\n", name, ext);

	if (strcmp(ext, "OFF") == 0) {
		writeOffModel(mesh, filename);
	} else if (strcmp(ext, "OBJ") == 0) {
		writeObjModel(mesh, filename);
	} else if (strcmp(ext, "SMF") == 0) {
		writeSmfModel(mesh, filename);
	} else {
		printf("# Unknown file format: %s\n", ext);
		exit(-1);
	}
}

