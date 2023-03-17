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
#include "timerms.h"


/*
    state: 0 - start timer, 1 - end timer
	printon: 0 - do not print times, 1 - print times
 */
double timerMS(int state, int printon)
{
	static struct TIMEB starttime, endtime;
	char *timeline;
	double elapsedTime;

	if (state == 0) {
		FTIME(&starttime);
		timeline = ctime(&(starttime.time));
	} else {
		FTIME(&endtime);
		timeline = ctime(&(endtime.time));
		if (printon) printf("The starting time: %.19s.%03u %s", timeline, starttime.millitm, &timeline[20]);
		if (printon) printf("The ending time  : %.19s.%03u %s\n", timeline, endtime.millitm, &timeline[20]);

		elapsedTime = difftime(endtime.time, starttime.time);
		//printf("Elapsed time is %.3f seconds.\n", elapsedTime + (endtime.millitm-starttime.millitm)/1000.0);
		return (elapsedTime + (endtime.millitm-starttime.millitm)/1000.0);
	}
	return 0;
}




/*
    state: 0 - start timer, 1 - end timer
	printon: 0 - do not print times, 1 - print times
 */
double timerMSmult(int id, int state, int printon)
{
	static struct TIMEB starttime[MAX_NUM_TIMER];
	static struct TIMEB endtime;
	char *timeline;
	double elapsedTime;
	//DWORD mtime;
	//mtime = timeGetTime(); 

	if (id < 0 || id > MAX_NUM_TIMER-1)  return -1;

	if (state == 0) {
		FTIME(&(starttime[id]));
		timeline = ctime(&(starttime[id].time));
	} else {
		FTIME(&endtime);
		timeline = ctime(&(endtime.time));
		if (printon) printf("The starting time: %.19s.%03u %s", timeline, starttime[id].millitm, &timeline[20]);
		if (printon) printf("The ending time  : %.19s.%03u %s\n", timeline, endtime.millitm, &timeline[20]);

		elapsedTime = difftime(endtime.time, starttime[id].time);
		//printf("Elapsed time is %.3f seconds.\n", elapsedTime + (endtime.millitm-starttime[id].millitm)/1000.0);
		return (elapsedTime + (endtime.millitm-starttime[id].millitm)/1000.0);
	}
	return 0;
}

