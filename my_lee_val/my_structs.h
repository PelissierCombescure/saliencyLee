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
	//std::string name;
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

//----  Data Structures

typedef struct {
	float val[2];
} Sfloat2;

typedef struct {
	float gaussian[2];
	float rawSaliency;
} Ssaliency;

typedef struct {
	float var[2];
	int nlist;

	Ssaliency *list;
	float minRawSal;
	float maxRawSal;
} SsaliencyList;

typedef struct {
	Sfloat2 *gaussian;
	float *rawSaliency;
} SMsaliency;


typedef struct {
	Sfloat2 *var;
	int nhier;
	int nlist;

	SMsaliency *list;
	float *minRawSal;
	float *maxRawSal;
	float minsal;
	float maxsal;
} SMsaliencyList;

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

typedef struct {
	float *val;
	int nval;
	float minv, maxv;
	float interval;
} SlookupTable;