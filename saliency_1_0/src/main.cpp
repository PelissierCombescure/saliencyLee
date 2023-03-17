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
//#include "GL/glut.h"
//#include "iui2cpp.h"
//#include "timerms.h"
#define _MAIN_GLOBAL_DEFINE
#include "main.h"
#include "geom.h"
//#include "macro.h"
//#include "saliency.h"
//#include "matrix.h"
//#include "grid.h"
//#include "gauss.h"

// #pragma comment(lib, "iui2.lib")
// #define BUFSIZE  1024

//----------------------------------------------------
//			Global variables
//----------------------------------------------------

//==== Lighting global variables ====
// GLfloat light0_ambient[] =  {0.1f, 0.1f, 0.3f, 1.0f};
// GLfloat light0_diffuse[] = { .7f, .7f, 1.0f, 1.0f};
// GLfloat light0_position[] = {-.5f, .5f, 1.0f, 0.0f};

// GLfloat light1_ambient[] =  {0.1f, 0.1f, 0.3f, 1.0f};
// GLfloat light1_diffuse[] =  {.9f, .6f, 0.f, 1.0f};
// GLfloat light1_position[] = {1.0f, -1.0f, 1.0f, 0.0f};

float scaleFactor = 1.2;
int screenWid = INIT_WIDTH;
int screenHgt = INIT_HEIGHT;

int drawMode = 0; // 0: mesh rendering , 1: curvatures, 2: saliency
int nDrawMode = 3;


Smesh gObject;
// char gFilename[BUFSIZE];
// char gProgname[BUFSIZE];

//----------------------------------------------------
//			External variables
//----------------------------------------------------

//----------------------------------------------------
//			External functions
//----------------------------------------------------
//extern bool isFileExist(char *filename);
extern void readModel(Smesh *mesh, char *filename);
// extern void writeModel(Smesh *mesh, char *filename);
extern void getProgName(char *progname, char *fullpath);

// extern void computeNormals(Smesh *surf);
// extern void computeCurvatures(Smesh *mesh);
// extern void computeEdges(Smesh *mesh);


// //----------------------------------------------------
// //			functions
// //----------------------------------------------------

// void drawSmesh(Smesh *mesh)
// {
// 	int i, j;

// 	glBegin(GL_TRIANGLES);
// 	for (i=0; i<mesh->ntris; i++) {
// 		for (j=0; j<3; j++) {
// 			glNormal3fv(mesh->tris[i].verts[j]->normal);
// 			glVertex3fv(mesh->tris[i].verts[j]->coords);
// 		}
// 	}
// 	glEnd();
// }


// inline void computeCurvatureColor(Tcolor4 color, Smesh *mesh, int verti)
// {
// 	Scurvature *curv = &(mesh->curvatures[verti]);
// 	float minc = 50, maxc = 255, cdiff = maxc-minc;
// 	float curvature, ratio, c, cutoff = 0.5;

// 	ASSIGN4V4(color, minc, minc, minc, 255);
// 	if (!(curv->isValid))  return;

// 	float meancurv = computeMeanCurvature(mesh, verti);
// 	curvature = ABS(meancurv);
// 	if (meancurv > 0) {
// 		ratio = cutoff*ABS(mesh->maxMeanCurv);
// 		c = (ratio!=0)? minc+curvature*cdiff/ratio : 0;
// 		c = (c>255)? 255:c;
// 		color[0] = c;
// 	} else {
// 		ratio = cutoff*ABS(mesh->minMeanCurv);
// 		c = (ratio!=0)? minc+curvature*cdiff/ratio : 0;
// 		c = (c>255)? 255:c;
// 		color[2] = c;
// 	}
// }


// void drawCurvatures(Smesh *mesh)
// {
// 	Tcolor4 color = { 0, 0, 0, 255 };
// 	int i, j;

// 	glDisable(GL_LIGHTING);

// 	glBegin(GL_TRIANGLES);
// 	for (i=0; i<mesh->ntris; i++) {
// 		for (j=0; j<3; j++) {
// 			computeCurvatureColor(color, mesh, mesh->tris[i].verts[j]->id);
// 			glColor3ubv(color);
// 			glVertex3fv(mesh->tris[i].verts[j]->coords);
// 		}
// 	}
// 	glEnd();
// }


// void computeSaliencyColor(Tcolor4 color, Smesh *mesh, int id, int mode)
// {
// 	float cval = mesh->verts[id].saliency; 

// 	if ((glMsaliency.maxsal - glMsaliency.minsal) > 0.01)  {
// 		cval = (mesh->verts[id].saliency - glMsaliency.minsal) / (glMsaliency.maxsal - glMsaliency.minsal);
// 	}
// 	TRIM(cval, 0, 1.0);
// 	getColorCode(color, cval, 0, 1.0);
// }


// void drawSaliency(Smesh *mesh, int mode)
// {
// 	Tcolor4 color = { 0, 0, 0, 255 };
// 	int i, j, k;

// 	glDisable(GL_LIGHTING);

// 	glBegin(GL_TRIANGLES);
// 	for (i=0; i<mesh->ntris; i++) {
// 		for (j=0; j<3; j++) {
// 			computeSaliencyColor(color, mesh, mesh->tris[i].verts[j]->id, mode);
// 			glColor3ubv(color);
// 			glVertex3fv(mesh->tris[i].verts[j]->coords);
// 		}
// 	}
// 	glEnd();

// 	if (isDrawEachSaliencyLevel) printdg("* Current level; %d/%d,  [%.1f, %.1f]\n", glCurrSalLevel, glMsaliency.nhier-1, glMgaussianVar[glCurrSalLevel].val[0], glMgaussianVar[glCurrSalLevel].val[1]);

// }


// void redraw(void)
// {
// 	float m[4][4];

// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// 	uiMatrix(m);
// 	glPushMatrix();
// 	glMultMatrixf((GLfloat *)m);
// 	glTranslatef(-gObject.center[0], -gObject.center[1], -gObject.center[2]);

// 	START_TIMER;

// 	switch (drawMode) {
// 	case 0:
// 		glEnable(GL_LIGHTING);
// 		drawSmesh(&gObject);
// 		printf("* render \'%s\' model\n", gObject.name);
// 		break;
// 	case 1:
// 		drawCurvatures(&gObject);
// 		printf("* show mean curvature of \'%s\' model\n", gObject.name);
// 		break;
// 	case 2:	
// 		drawSaliency(&gObject, 0);
// 		printf("* show saliency of \'%s\' model\n", gObject.name);
// 		break;
// 	default:
// 		drawSmesh(&gObject);
// 		break;
// 	}

// 	glPopMatrix();

// 	glutSwapBuffers();

// 	printdg("* saliency in (%f, %f)\n", glMsaliency.minsal, glMsaliency.maxsal);
// 	printf("* Time to render: %f sec\n\n", END_TIMER);	
// }





// void reshape(int w, int h)
// {
// 	float modelWid, modelHgt;

// 	screenWid = w;
// 	screenHgt = h;

// 	if (w <= h) {
// 		modelHgt = scaleFactor*(float)h/(float)w;
// 		modelWid = scaleFactor;
// 	} else {
// 		modelHgt = scaleFactor;
// 		modelWid = scaleFactor*(float)w/(float)h;
// 	}

// 	uiReshape(w, h);
// 	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
// 	glMatrixMode(GL_PROJECTION);
// 	glLoadIdentity();
// 	glOrtho(-modelWid, modelWid,
// 			-modelHgt, modelHgt,
// 			-10*scaleFactor, 10*scaleFactor);
// 	glMatrixMode(GL_MODELVIEW);
// 	glLoadIdentity();
// }

// void mouse(int button, int state, int x, int y)
// {  
// 	int i = glutGetModifiers();
// 	uiSetModifiers(i);
// 	uiMouseListener(button, state, x, y);
// 	glutPostRedisplay();
// }


// void motion(int x, int y)
// { 
// 	uiMouseDragListener(1, x, y);
// 	glutPostRedisplay();
// }


// void keyListener(unsigned char key, int x, int y)
// {
// 	static int count = 1;
// 	float zero[] = { 0, 0 ,0 ,1};
// 	float p2[] = { 0.2, 0.2 ,0.2 ,1}, p8[] = { 0.8, 0.8 ,0.8 ,1};
// 	float view[4] = {0, 0, 1, 1}, up[4] = {0, 1, 0, 1}, m[4][4];
// 	int i;

// 	switch(key){
// 	case 'i':
// 		isDrawEachSaliencyLevel = (isDrawEachSaliencyLevel)? false:true;
// 		if (isDrawEachSaliencyLevel) {
// 			getSingleSaliencyFromGaussians(&gObject, &glMsaliency, glCurrSalLevel);
// 			printf("## draw level %d\n", glCurrSalLevel);
// 		} else {
// 			getMultiSaliencyFromGaussians(&gObject, &glMsaliency);
// 			printf("## draw Multiresolution Saliency\n");
// 		}
// 		break;
// 	case 'l':
// 		drawMode = (drawMode + 1) % nDrawMode;
// 		break;
// 	case 'L':
// 		drawMode = (drawMode + nDrawMode - 1) % nDrawMode;
// 		break;
// 	case '-': case '_':
// 		glCurrSalLevel = (glCurrSalLevel + glMsaliency.nhier - 1) % glMsaliency.nhier;
// 		printf("** Curr Saliency Level = %d / %d\n", glCurrSalLevel, glMsaliency.nhier-1);
// 		if (isDrawEachSaliencyLevel) {
// 			getSingleSaliencyFromGaussians(&gObject, &glMsaliency, glCurrSalLevel);
// 			printf("## draw level %d\n", glCurrSalLevel);
// 		}
// 		break;
// 	case '+': case '=':
// 		glCurrSalLevel = (glCurrSalLevel + 1) % glMsaliency.nhier;
// 		printf("** Curr Saliency Level = %d / %d\n", glCurrSalLevel, glMsaliency.nhier-1);
// 		if (isDrawEachSaliencyLevel) {
// 			getSingleSaliencyFromGaussians(&gObject, &glMsaliency, glCurrSalLevel);
// 			printf("## draw level %d\n", glCurrSalLevel);
// 		}
// 		break;
// 	} 
// 	glutPostRedisplay();
// }

// void specialKeyPressed(int key, int x, int y)
// {
// 	float initMouseMove = 10.0;
// 	static float mouseMove = initMouseMove;
// 	int xPos = 0, yPos = 0;
// 	float m[4][4];

// 	xPos = screenWid*0.5;
// 	yPos = screenHgt*0.5;

// 	switch (key) {
// 	case GLUT_KEY_UP:
// 		uiSetModifiers(glutGetModifiers());
// 		uiMouseListener(UI_LEFT_BUTTON, UI_MOUSE_DOWN, xPos, yPos);

// 		yPos -= mouseMove;
// 		if (glutGetModifiers() == GLUT_ACTIVE_SHIFT)  yPos -= mouseMove;
// 		uiMouseDragListener(UI_LEFT_BUTTON, xPos, yPos);
// 		uiMouseListener(UI_LEFT_BUTTON, UI_MOUSE_DOWN, x, y);
// 		break;
// 	case GLUT_KEY_DOWN:
// 		uiSetModifiers(glutGetModifiers());
// 		uiMouseListener(UI_LEFT_BUTTON, UI_MOUSE_DOWN, xPos, yPos);

// 		yPos += mouseMove;
// 		if (glutGetModifiers() == GLUT_ACTIVE_SHIFT)  yPos += mouseMove;
// 		uiMouseDragListener(UI_LEFT_BUTTON, xPos, yPos);
// 		uiMouseListener(UI_LEFT_BUTTON, UI_MOUSE_DOWN, x, y);
// 		break;
// 	case GLUT_KEY_LEFT:
// 		uiSetModifiers(glutGetModifiers());
// 		uiMouseListener(UI_LEFT_BUTTON, UI_MOUSE_DOWN, xPos, yPos);

// 		xPos -= mouseMove;
// 		uiMouseDragListener(UI_LEFT_BUTTON, xPos, yPos);
// 		uiMouseListener(UI_LEFT_BUTTON, UI_MOUSE_DOWN, x, y);
// 		break;
// 	case GLUT_KEY_RIGHT:
// 		uiSetModifiers(glutGetModifiers());
// 		uiMouseListener(UI_LEFT_BUTTON, UI_MOUSE_DOWN, xPos, yPos);

// 		xPos += mouseMove;
// 		uiMouseDragListener(UI_LEFT_BUTTON, xPos, yPos);
// 		uiMouseListener(UI_LEFT_BUTTON, UI_MOUSE_DOWN, x, y);
// 		break;
// 	case GLUT_KEY_HOME:
// 		uiReset();
// 		break;
// 	case GLUT_KEY_PAGE_UP:
// 		mouseMove += 1.0;
// 		printf("mouseMove = %f\n", mouseMove);
// 		break;
// 	case GLUT_KEY_PAGE_DOWN:
// 		if (mouseMove > 1.0)  mouseMove -= 1.0;
// 		printf("mouseMove = %f\n", mouseMove);
// 		break;
// 	}
// 	uiSpecialKeyUpListener(key, x, y);
// 	glutPostRedisplay();
// }


// void initUI()
// {
// 	uiInit(); 
// 	uiMode(UI_ARCBALL_MODE);
// 	uiSetArcballParameters(0.8, 13, 1.0); 
// 	uiSetWalkParameters(1.5, 1.5, 0.05, 10, 20);
// 	uiSetCameraPosition(0, 0.0, 30);
// }

// void initGL(int argc, char **argv)
// {
// 	glutInit(&argc, argv);
// 	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
// 	glutInitWindowSize(screenWid, screenHgt);

// 	glutCreateWindow("Sample");
// 	glutDisplayFunc(redraw);
// 	glutReshapeFunc(reshape);
// 	glutMouseFunc(mouse);
// 	glutMotionFunc(motion);
// 	glutSpecialFunc(&specialKeyPressed);
// 	glutKeyboardFunc(&keyListener);

// 	glEnable(GL_DEPTH_TEST);
// 	glClearColor(1.0, 0.98, 0.94, 1);
  
// 	//==========  Set up OpenGL lights
// 	glEnable(GL_LIGHTING);
// 	glEnable( GL_NORMALIZE );

// 	glEnable(GL_LIGHT0);
// 	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
// 	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
// 	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

// 	glEnable(GL_LIGHT1);
// 	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
// 	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
// 	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
  
// }

// //--------------------
// void getProgName(char *progname, char *fullpath);


// void usage(char *fullpath)
// {
// 	char pname[BUFSIZE];

//  	getProgName(pname, fullpath);
// 	printf("** Usage:\n");
// 	printf("%s <options> <filename>\n", pname);
// 	printf(" * Options:\n");
// 	printf("  -[a|ax] : consider(a) or does NOT consider(ax) vertex areas for saliency computation\n");
// 	printf("  -[f|fx] : Allow(f) or does NOT allow(fx) to read saliency from files\n");
// 	printf("  -l <size>: Apply smoothing filter to the final saliency. if <size>==0, does not smooth.\n");
// 	printf("  -m <start> <inc> <num>: Multiple resolution saliency: Gaussian stdev as multiplies of average edge length.\n");
// 	printf("  -r weight : Use <weight> %% of the object diagonal as a unit for multiscale saliency.\n");
// 	printf("  -[s|sx] : Does(s) or does NOT(sx) suppress saliency\n");
// 	printf("  -u : use hierarchical scales\n");
// 	printf("  -w[x] : write normalized mesh saliency in a file and use (w) or does NOT(wx) use amplification.\n");
// 	printf("  -wa : write normalized mesh saliency in a file and use amplification with a default value (cut top 30%%).\n");
// 	printf("  -wc <ratio> : write normalized mesh saliency in a file and use amplification (cut top <ratio>).\n");
// 	printf("  -x <factor> : center surround factor\n");
// 	printf("  -z <amplify_high>: scale top percentage by a factor of <amplify_high>\n");

//}


// void processArg(int argc, char **argv)
// {
// 	char *progname;
// 	char *s;
	
// 	progname = argv[0];
// 	while (--argc > 0 && (*++argv)[0]=='-') {
// 		for (s = argv[0]+1; *s; s++) {
// 			switch (*s) {
// 			case 'a':
// 				useAreaForSaliency = true;
// 				if (*(s+1) == 'x') {
// 					useAreaForSaliency = false;
// 					s++;
// 				}
// 				break;
// 			case 'b':
// 				isBatchMode = true;
// 				break;
// 			case 'f':
// 				isAllowToReadFromFile = true;
// 				if (*(s+1) == 'x') {
// 					isAllowToReadFromFile = false;
// 					s++;
// 				}
// 				break;
// 			case 'i':
// 				screenWid = screenHgt = atoi((*++argv));
// 				printf("# Window size = (%d X %d)\n", screenWid, screenHgt);
// 				argc--;
// 				break;
// 			case 'l':
// 				glSmoothSize = atof((*++argv));
// 				argc--;
// 				if (glSmoothSize > 0)  printdg("# Apply smoothing (size = %.1f)\n", glSmoothSize);
// 				break;
// 			case 'm':
// 				glStartMvar = atof((*++argv));
// 				argc--;
// 				glMvarInc = atof((*++argv));
// 				argc--;
// 				glNumMultiSaliency = atoi((*++argv));
// 				argc--;
// 				break;
// 			case 'r':
// 				glUnitRatio = atof((*++argv)) * 0.01;
// 				argc--;
// 				printdg("# Unit for computing saliency: %.2f*diagonal\n", glUnitRatio);
// 				break;
// 			case 's':
// 				useSupress = true;
// 				if (*(s+1) == 'x') {
// 					useSupress = false;
// 					s++;
// 				}
// 				break;
// 			case 'u':
// 				useHierarchicalScales = true;
// 				break;
// 			case 'w':
// 				isWriteMeshSaliency = true;
// 				if (*(s+1) == 'x') {
// 					useAmplification = false;
// 					s++;
// 				} else if (*(s+1) == 'a') {
// 					useAmplification = true;
// 					s++;
// 					glSmoothSize = 3;
// 					printdg("## Amplification: cut high %.0f %%\n", 100.0*glCutPercentage);
// 				} else if (*(s+1) == 'c') {
// 					useAmplification = true;
// 					s++;
// 					glCutPercentage = atof((*++argv));
// 					argc--;
// 					printdg("## Amplification: cut high %.0f %%\n", 100.0*glCutPercentage);
// 				}
// 				break;
// 			case 'x':
// 				glCenterSurroundFactor = atof((*++argv));
// 				argc--;
// 				break;

// 			case 'z':
// 				glAmplifyTopValue = atof((*++argv));
// 				argc--;
// 				break;
// 			default:
// 				usage (progname);
// 				exit (-1);
// 				break;
// 			}
// 		}
// 	}
// 	if (argc > 0) {
// 		sprintf(gFilename, "%s", *argv);
// 	} else {
// 		sprintf(gFilename, "%s", "sphere.off");
// 	}
// 	printf("## read %s\n", gFilename);
// 	getProgName(gProgname, gFilename);
	
// 	if (isAllowToReadFromFile) {
// 		printf("# Allow to read saliency map from files\n");
// 	} else {
// 		printf("# Does NOT allow to read saliency map from files\n");
// 	}
// }


// void getResultFilename(char *filename, char *progname, float var[2])
// {
// 	char *area = "", *suppress = "";

// 	if (useAreaForSaliency)  area = "a";

// 	if (useHierarchicalScales) {
// 		sprintf(filename, "results\\%s_%.1f_%.1f%s%s_d", progname, var[0], var[1], area, suppress);
// 	} else {
// 		sprintf(filename, "results\\%s_%.1f_%.1f%s%s", progname, var[0], var[1], area, suppress);
// 	}
// 	sprintf(filename, "%s_r%.1f", filename, glUnitRatio*100.0);
// 	sprintf(filename, "%s.sal", filename);

// 	printdg("FILE: %s\n", filename);
// }


// void initFunctions()
// {
// 	initGrid(&glGrid, &gObject, glGridNum);
// 	buildGrid(&glGrid, &gObject);
// 	buildGaussTable(&glGaussTable, glGaussTableSize);
// 	buildGaussTableSqr(&glGaussSqrTable, glGaussTableSize);
// }


int main(int argc, char **argv)
{
	//processArg(argc, argv);

	readModel(&gObject, "sphere.off");
	// scaleFactor = 1.2 * gObject.extent;

	// computeNormals(&gObject);
	// computeEdges(&gObject);
	// computeCurvatures(&gObject);
	// getSaliencyUnit(&gObject);

	// //----- compute saliency
	// char buf[BUFSIZE];
	// int i, j;

	// initFunctions();

	// START_TIMERI(0);
	// initSsaliencyList(&glSaliency, &gObject);
	// initSMsaliencyList(&glMsaliency, &gObject);

	// for (i=0; i<glMsaliency.nhier; i++) {
	// 	START_TIMERI(1);

	// 	ASSIGN2V(glSaliency.var, glMsaliency.var[i].val);
	// 	getResultFilename(buf, gProgname, glMgaussianVar[i].val);

	// 	if (!(isAllowToReadFromFile && readSaliency(buf, &gObject, &glSaliency))) {
	// 		computeSaliency(&gObject, &glSaliency);
	// 		writeSaliency(buf, &gObject, &glSaliency);
	// 	}
		
	// 	for (j=0; j<glSaliency.nlist; j++) {
	// 		ASSIGN2V(glMsaliency.list[j].gaussian[i].val, glSaliency.list[j].gaussian);
	// 		glMsaliency.list[j].rawSaliency[i] = glSaliency.list[j].rawSaliency;
	// 	}

	// 	printf(" # Time for computing %dth saliency out of %d: %.3f sec\n", i+1, glMsaliency.nhier, END_TIMERI(1));
	// }
	// getMultiSaliencyFromGaussians(&gObject, &glMsaliency);
	// printf("## Time for computing saliency: %.3f sec\n", END_TIMERI(0));
	// printdg("# saliency in (%f, %f)\n", glMsaliency.minsal, glMsaliency.maxsal);

	// initUI();
	// initGL(argc, argv);

	// if (isWriteMeshSaliency) {
	// 	sprintf(buf, "%s.sal", gObject.name);
	// printdg("#1 saliency in (%f, %f)\n", glMsaliency.minsal, glMsaliency.maxsal);
	// 	writeMeshSaliency(buf, &gObject, &glMsaliency);
	// printdg("#2 saliency in (%f, %f)\n", glMsaliency.minsal, glMsaliency.maxsal);
	// }

	// if (isBatchMode) return 1;

	// glutMainLoop();

	// return 0;
}

