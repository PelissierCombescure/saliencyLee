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
#ifndef _MAIN_H
#define _MAIN_H

#define INIT_WIDTH	512
#define INIT_HEIGHT INIT_WIDTH


//----  Global Variables

#ifndef _MAIN_GLOBAL_DEFINE
#define _MAIN_EXTERN extern
#else
#define _MAIN_EXTERN
#endif

#ifdef _MAIN_GLOBAL_DEFINE

_MAIN_EXTERN bool isAllowToReadFromFile = true;
_MAIN_EXTERN bool isDrawEachSaliencyLevel = false;
_MAIN_EXTERN bool useSupress = true;

_MAIN_EXTERN bool isBatchMode = false;
_MAIN_EXTERN bool isWriteMeshSaliency = true;

_MAIN_EXTERN int glCurrSalLevel = 0;

#else

_MAIN_EXTERN bool isAllowToReadFromFile;
_MAIN_EXTERN bool isDrawEachSaliencyLevel;
_MAIN_EXTERN bool useSupress;

_MAIN_EXTERN bool isBatchMode;
_MAIN_EXTERN bool isWriteMeshSaliency;

_MAIN_EXTERN int glCurrSalLevel;

#endif

#undef _MAIN_EXTERN
#undef _MAIN_GLOBAL_DEFINE


void HSVtoRGB(float *r, float *g, float *b, float h, float s, float v );
unsigned char *getColorCode(unsigned char color[3], float val, float minv, float maxv);

#endif