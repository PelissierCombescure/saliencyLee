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
#ifndef MACRO_H
#define MACRO_H

#define MY_DEBUG


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// #define M_PI 3.14159265358979323846264
#ifndef M_PI
#define M_PI 3.14159265
#endif
#define BUFSIZE  1024


#define ROUND(A)				(floor((A)+0.5))
#define MIN(A,B)				(((A)<(B))? (A):(B))
#define MAX(A,B)				(((A)>(B))? (A):(B))
#define ABS(A)					(((A)>=0)? (A):-(A))
#define SIGN3(A)					(((A)>0)? 1:(((A)<0)? -1:0))

#define IDX2D(I,J,N)			((I)*(N)+(J))
#define IDX3D(I,J,K,NY,NZ)		(((I)*(NY)+(J))*(NZ)+(K))
#define IDX3DV(V,N)				(((V)[0]*(N)[1]+(V)[1])*(N)[2]+(V)[2])

#define ASSIGN2V(A,B)			{(A)[0]=(B)[0];(A)[1]=(B)[1];}
#define ASSIGN3V(A,B)			{(A)[0]=(B)[0];(A)[1]=(B)[1];(A)[2]=(B)[2];}
#define ASSIGN4V(A,B)			{(A)[0]=(B)[0];(A)[1]=(B)[1];(A)[2]=(B)[2];(A)[3]=(B)[3];}
#define ASSIGN2V2(T,X,Y)		{(T)[0]=(X);(T)[1]=(Y);}
#define ASSIGN3V3(T,X,Y,Z)		{(T)[0]=(X);(T)[1]=(Y);(T)[2]=(Z);}
#define ASSIGN4V4(T,X,Y,Z,A)	{(T)[0]=(X);(T)[1]=(Y);(T)[2]=(Z);(T)[3]=(A);}
#define ASSIGN_ELBOND(A,B)		(A).elem1=(B).elem1;(A).elem2=(B).elem2;(A).bondtype=(B).bondtype;
#define ASSIGN_COLOR(A,B)		ASSIGN4V((A).ambient,(B).ambient);ASSIGN4V((A).diffuse,(B).diffuse);ASSIGN4V((A).specular,(B).specular);ASSIGN4V((A).display,(B).display);
#define ASSIGN_VERTEX(A,B)		ASSIGN3V((A).coords,(B).coords);ASSIGN3V((A).normal,(B).normal);ASSIGN_COLOR((A).color,(B).color);
#define SQR(A)					((A)*(A))
#define CUBE(A)					((A)*(A)*(A))

#define DIST2V(A,B)				sqrt(SQR((A)[0]-(B)[0])+SQR((A)[1]-(B)[1]))
#define DIST3V(A,B)				sqrt(SQR((A)[0]-(B)[0])+SQR((A)[1]-(B)[1])+SQR((A)[2]-(B)[2]))
#define DIST3VSQR(A,B)			(SQR((A)[0]-(B)[0])+SQR((A)[1]-(B)[1])+SQR((A)[2]-(B)[2]))
#define LEN3V(A)				sqrt((A)[0]*(A)[0]+(A)[1]*(A)[1]+(A)[2]*(A)[2])
#define LEN4V(A)				sqrt((A)[0]*(A)[0]+(A)[1]*(A)[1]+(A)[2]*(A)[2]+(A)[3]*(A)[3])
#define LEN3(A,B,C)				sqrt((A)*(A)+(B)*(B)+(C)*(C))
#define NORMALIZE3V(A)			{double temp;temp=LEN3V(A);(A)[0]/=temp;(A)[1]/=temp;(A)[2]/=temp;}
#define NORMALIZE4V(A)			{double temp;temp=LEN4V(A);(A)[0]/=temp;(A)[1]/=temp;(A)[2]/=temp;(A)[3]/=temp;}
#define NORMALIZEXY3V(A)		{double temp;temp=sqrt(SQR((A)[0])+SQR((A)[1]));(A)[0]/=temp;(A)[1]/=temp;}
#define ADD3V(T,A,B)			{(T)[0]=((A)[0]+(B)[0]);(T)[1]=((A)[1]+(B)[1]);(T)[2]=((A)[2]+(B)[2]);}
#define AVG2(A,B)				(((A)+(B))*0.5)
#define AVG2V(A)				(((A)[0]+(A)[1])*0.5)
#define AVG3V2(T,A,B)			{(T)[0]=((A)[0]+(B)[0])*.5;(T)[1]=((A)[1]+(B)[1])*.5;(T)[2]=((A)[2]+(B)[2])*.5;}
#define DOT_PROD3V(A,B)			((A)[0]*(B)[0]+(A)[1]*(B)[1]+(A)[2]*(B)[2])
#define CROSS_PROD3V(T,A,B)		{(T)[0]=(A)[1]*(B)[2]-(A)[2]*(B)[1];(T)[1]=(A)[2]*(B)[0]-(A)[0]*(B)[2];(T)[2]=(A)[0]*(B)[1]-(A)[1]*(B)[0];}
#define SET_COLOR3V(T,R,G,B)	(T)[0]=(R);(T)[1]=(G);(T)[2]=(B);
#define SET_COLOR4V(T,R,G,B,A)	(T)[0]=(R);(T)[1]=(G);(T)[2]=(B);(T)[3]=(A);
#define LIST_COMMA3V(V)			(V)[0], (V)[1], (V)[2]
#define LIST_COMMA4V(V)			(V)[0], (V)[1], (V)[2], (V)[3]
#define TRIM(X,A,B)				(X) = ((X)<(A))? (A):(((X)>(B))? (B):(X))

#define VEC1_SIM(V,K,E)		((ABS((V)[0]-(K)[0])<(E)))
#define VEC2_SIM(V,K,E)		((ABS((V)[0]-(K)[0])<(E))&&(ABS((V)[1]-(K)[1])<(E)))
#define VEC3_SIM(V,K,E)		((ABS((V)[0]-(K)[0])<(E))&&(ABS((V)[1]-(K)[1])<(E))&&(ABS((V)[2]-(K)[2])<(E)))
#define VEC4_SIM(V,K,E)		((ABS((V)[0]-(K)[0])<(E))&&(ABS((V)[1]-(K)[1])<(E))&&(ABS((V)[2]-(K)[2])<(E))&&(ABS((V)[3]-(K)[3])<(E)))

#define VEC1_SIM_EQ(V,K,E)		((ABS((V)[0]-(K)[0])<=(E)))
#define VEC2_SIM_EQ(V,K,E)		((ABS((V)[0]-(K)[0])<=(E))&&(ABS((V)[1]-(K)[1])<=(E)))
#define VEC3_SIM_EQ(V,K,E)		((ABS((V)[0]-(K)[0])<=(E))&&(ABS((V)[1]-(K)[1])<=(E))&&(ABS((V)[2]-(K)[2])<=(E)))
#define VEC4_SIM_EQ(V,K,E)		((ABS((V)[0]-(K)[0])<=(E))&&(ABS((V)[1]-(K)[1])<=(E))&&(ABS((V)[2]-(K)[2])<=(E))&&(ABS((V)[3]-(K)[3])<=(E)))

#define VEC2_LT_SIM(V,K,E)        (((K)[0]-(V)[0])>(E) ||  \
			(VEC1_SIM(V,K,E)&&((K)[1]-(V)[1])>(E)))

#define VEC4_LT_SIM(V,K,E)        (((K)[0]-(V)[0])>(E) ||  \
			(VEC1_SIM(V,K,E)&&((K)[1]-(V)[1])>(E)) ||  \
			(VEC2_SIM(V,K,E)&&((K)[2]-(V)[2])>(E)) ||  \
			(VEC3_SIM(V,K,E)&&((K)[3]-(V)[3])>(E)))


#define VEC3_EQ(V,K)        (((V)[0]==(K)[0])&&((V)[1]==(K)[1])&&((V)[2]==(K)[2]))
#define VEC3_LT(V,K)        (((V)[0]<(K)[0]) ||  \
			(((V)[0]==(K)[0])&&((V)[1] <(K)[1])) ||  \
			(((V)[0]==(K)[0])&&((V)[1]==(K)[1])&&((V)[2]<(K)[2])))
#define VEC3_GT(V,K)        (((V)[0] >(K)[0]) ||  \
			(((V)[0]==(K)[0])&&((V)[1]>(K)[1])) ||  \
			(((V)[0]==(K)[0])&&((V)[1]==(K)[1])&&((V)[2]>(K)[2])))

//#define VEC3_SIM(V,K,E)        ((ABS((V)[0]-(K)[0])<=(E))&&(ABS((V)[1]-(K)[1])<=(E))&&(ABS((V)[2]-(K)[2])<=(E)))


#define SET_MIN(M,X)	if ((X) < (M))  (M) = (X);
#define SET_MAX(M,X)	if ((X) > (M))  (M) = (X);
#define SET_MIN_MAX(MIN,MAX,X)	{if ((X) < (MIN))  (MIN) = (X); if ((X) > (MAX))  (MAX) = (X);}


//inline int round(float val)		{ return ((int)(val+0.5)); }
inline double lg(double val)		{ return (log(val) / log(2)); }
inline bool isEven(int x)  { return ((x%2) == 0); }


inline bool cubeOutOfBound(int x[3], int nx, int ny, int nz)
{
	if (x[0] < 0 || x[1] < 0 || x[2] < 0)  return true;
	if (x[0] >= nx || x[1] >= ny || x[2] >= nz)  return true;
	return false;
}
inline bool cubeOutOfBound(int x[3], int n[3])  { return cubeOutOfBound(x, n[0], n[1], n[2]); }
inline bool cubeOutOfBound(int x[3], int n)  { return cubeOutOfBound(x, n, n, n); }


inline float Dist(float a, float b)  {  return ABS(a-b); }
inline float Dist2v(float a[2])		 {  return ABS(a[0]-a[1]); }
inline float Dist(float a[2], float b[2])  {  return DIST2V(a,b); }
inline float Dist3v(float a[3], float b[3])  {  return DIST3V(a,b); }


#define mymalloc(block, type, n)	block = (type *)malloc((n)*sizeof(type)); \
									if (block == 0)  {	\
										printf("Memory allocation error for \"" #block "\" %.3f MB\n", (double)(n)*sizeof(type)/1000000);	\
										exit(0);\
									}	
									//printf("# Memory allocated for \"" #block "\"\n");


#define mycalloc(block, type, n)	block = (type *)malloc((n)*sizeof(type)); \
									if (block == 0)  {	\
										printf("Memory allocation error for \"" #block "\" %.3f MB\n", (double)(n)*sizeof(type)/1000000);	\
										exit(0);\
									}	\
									memset(block, 0, n);
									//printf("# Memory allocated for \"" #block "\"\n");


#define myfree(block)		if (block != 0) {	\
								free(block);	\
								block = 0;		\
							}		\
							//printf("# Memory for \"" #block "\" is freed\n");	
							//


// inline void printProgress(int i, int n)
// {
// 	static float percent = 1;
// 	static int step = 10;

// 	if (i == 0) {
// 		percent = 1;
// 		if (n > 2000) {
// 			step = n/200;
// 		} else if (n <= 100) {
// 			step = 1;
// 		}
// 	}

// 	if (i == n-1) {
// 		printf("[100%%]\n");
// 	} else if ((i+1)%step == 0)  {
// 		if ( 10.0*(i+1)/(float)n >= percent) {
// 			printf("[%d%%]", percent*10);
// 			percent++;
// 		} else {
// 			printf(".");
// 		}
// 	}
// }


#include <stdarg.h>
#include <ctype.h>

//  print for debug
//  print debug information if MY_DEBUG is define, otherwise do nothing
inline void printdg(char *szTypes, ... )
{
#ifdef MY_DEBUG
	char buf[2048];
    va_list vl;
    int i, k;

    va_start( vl, szTypes );
    for(i=0, k=0; szTypes[i] != '\0'; ++i ) {
        union Printable_t {
            int		d;
            double	f;
            char	c;
            char*	s;
			long	x;
        } Printable;

		buf[k++] = szTypes[i];
		if (szTypes[i] == '%') {
			i++;
			while(isdigit(szTypes[i]) || szTypes[i] == '.')  buf[k++] = szTypes[i++];
			buf[k++] = szTypes[i];
			buf[k] = '\0';  k=0;

			switch( szTypes[i] )  {  // Type to expect.
			case 'd':
				Printable.d = va_arg( vl, int );
				printf(buf, Printable.d );
		        break;
			case 'f':
				Printable.f = va_arg( vl, double );
				printf(buf, Printable.f );
				break;
			case 'x':
				Printable.x = va_arg( vl, long );
				printf(buf, Printable.x );
				break;
			case 'c':
	            Printable.c = va_arg( vl, char );
				printf( buf, Printable.c );
				break;
	        case 's':
				Printable.s = va_arg( vl, char * );
				printf( buf, Printable.s );
				break;
			default:
				//printf(buf);
	            break;
			}
		}
    }
	buf[k] = '\0';
	//printf(buf);
    va_end( vl );
#endif
}



#endif