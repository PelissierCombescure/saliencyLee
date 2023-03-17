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
#include <float.h>
#include <limits.h>
#include "qsort.h"

//----------------------------------------------------
//			Global variables
//----------------------------------------------------


//----------------------------------------------------
//			External variables
//----------------------------------------------------


//----------------------------------------------------
//			External functions
//----------------------------------------------------



//----------------------------------------------------
//			functions
//----------------------------------------------------


//---------  Sort Integer Values

#define SORT_DATA_ASSIGN(a,b)	(a).val=(b).val; (a).id=(b).id;


//---------  Sort Float Values


/* sort float values descending
@input:
@output
// sorts list[m] through list[n].
//   assumes that:
//   (a) the key is list[i].val
//       m <= i <= n
//   (b) list[n+1].val == -HUGE;
//
 */
void qsortFloatDescend(SfloatSortData *list, int m, int n)
{
 	int i, j;
 	float key;
	SfloatSortData tempData;

	if (m < n)  {
		i = m;
		j = n + 1;

		key = list[m].val;

		do {
			do {
				i++;
			} while ( (list[i].val > key) );
 			do {
				j--;
			} while ((list[j].val < key));
			if (i < j) {
				// swap i and j-th elements 
				SORT_DATA_ASSIGN(tempData, list[i]);
				SORT_DATA_ASSIGN(list[i], list[j]);
				SORT_DATA_ASSIGN(list[j], tempData);
			}
		} while (i < j);

		// swap m and j-th elements
		SORT_DATA_ASSIGN(tempData, list[m]);
		SORT_DATA_ASSIGN(list[m], list[j]);
		SORT_DATA_ASSIGN(list[j], tempData);

		qsortFloatDescend(list, m, j-1);
		qsortFloatDescend(list, j+1, n);
	}
}


void qsortFloatAscend(SfloatSortData *list, int m, int n)
{
 	int i, j;
 	float key;
	SfloatSortData tempData;

	if (m < n)  {
		i = m;
		j = n + 1;

		key = list[m].val;

		do {
			do {
				i++;
			} while ( (list[i].val < key) );
 			do {
				j--;
			} while ((list[j].val > key));
			if (i < j) {
				// swap i and j-th elements 
				SORT_DATA_ASSIGN(tempData, list[i]);
				SORT_DATA_ASSIGN(list[i], list[j]);
				SORT_DATA_ASSIGN(list[j], tempData);
			}
		} while (i < j);

		// swap m and j-th elements
		SORT_DATA_ASSIGN(tempData, list[m]);
		SORT_DATA_ASSIGN(list[m], list[j]);
		SORT_DATA_ASSIGN(list[j], tempData);

		qsortFloatAscend(list, m, j-1);
		qsortFloatAscend(list, j+1, n);
	}
}


// allocation size for list should be n+1
void sortFloatDataDescend(SfloatSortData *list, int n)
{
	list[n].val = -FLT_MAX; //-HUGE_VAL;
	qsortFloatDescend(list, 0, n);
}


// allocation size for list should be n+1
void sortFloatDataAscend(SfloatSortData *list, int n)
{
	list[n].val = FLT_MAX; //-HUGE_VAL;
	qsortFloatAscend(list, 0, n);
}



// allocation size for list should be n+1
// <mode>  Sort_Descending(0): descending, Sort_Ascending(1): ascending
void sortFloatData(SfloatSortData *list, int n, int mode)
{
	if (mode == Sort_Descending) {
		sortFloatDataDescend(list, n);
	} else { //if (mode == Sort_Ascending) 
		sortFloatDataAscend(list, n);
	}
}


