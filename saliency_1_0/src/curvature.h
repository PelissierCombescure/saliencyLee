#ifndef _CURVATURE_H
#define _CURVATURE_H

#define CURV_BOTH		0
#define CURV_MIN_ONLY	1
#define CURV_MAX_ONLY	2
#define CURV_ORG_ONLY	3


typedef float Tmatrix4[4][4];

typedef struct {
	float coord[4];
	float E1[3];
	float E2[3];
	int isValid;
	int id;
} Scurvature;

typedef struct {
	Scurvature *curvs;
	int ncurvs;
	int delta1, delta2;
} Scurvatures;

void mallocCurvatures(Scurvatures *curvs, int n);
void computeCurvatures(StriLayer *mesh, Scurvatures *curvs, int delta);

#endif