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
#ifndef _TIMERMS_H
#define _TIMERMS_H

#include <stdio.h>
#include <sys/timeb.h>
#include <time.h>

#define MAX_NUM_TIMER	10

#ifndef PLATFORM_UNIX
#define TIMEB	_timeb
#define FTIME	_ftime
#else
#define TIMEB	timeb
#define FTIME	ftime
#endif


#define START_TIMER				timerMS(0,0)
#define END_TIMER				timerMS(1,0)
#define START_TIMER_VERBOSE		timerMS(0,1)
#define END_TIMER_VERBOSE		timerMS(1,1)


#define START_TIMERI(I)			timerMSmult(I,0,0)
#define END_TIMERI(I)			timerMSmult(I,1,0)
#define START_TIMERI_VERBOSE(I)	timerMSmult(I,0,1)
#define END_TIMERI_VERBOSE(I)	timerMSmult(I,1,1)


double timerMS(int state, int printon);
double timerMSmult(int id, int state, int printon);

#endif