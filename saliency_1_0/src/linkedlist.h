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
#ifndef _LINKEDLIST_H
#define _LINKEDLIST_H

#include <stdlib.h>

template <class T> struct SlinkedItem {
	T data;
	SlinkedItem *next;
} ;


template <class T> class ClinkedList {
private:
	SlinkedItem<T> *first;
	SlinkedItem<T> *last;
	SlinkedItem<T> *curr;
	T delimiter;
	int num;
	bool isOrdered;
	bool isAscending;
	bool isDuplicated;
protected:
public:
	ClinkedList() { first = last = curr = 0; num = 0; isOrdered = true; isAscending = true;  isDuplicated = false; }
	~ClinkedList();
	void clear();
	void setDelimiter(T value)  { delimiter = value; }
	bool isEmpty()  { return (!first); }
	bool isEnd()  {  return (!curr); }
	bool isNotEnd()  {  return (curr); }
	bool ordered()  { return isOrdered; }
	bool duplicated()  { return isDuplicated; }
	bool ascending()  { return isAscending; }
	bool descending()  { return !isAscending; }
	void setAllowDuplicated(bool val)  { isDuplicated = val; }
	void setAscending()  { isAscending = true; }
	void setDescending()  { isAscending = false; }
	int getNum()  { return num; }
	void setFirst()  { curr = first; }
	T getFirst();
	T getNext();
	T getLast();
	void pushBack(T value);
	void pushFront(T value);
	T popFirst();
	T popLast();

	void addOrdered(T value);
	void mergeOrdered(ClinkedList *other); // merge all items of <other> into <this>, and make <other> empty
	ClinkedList operator=(ClinkedList &other); // not defined
	ClinkedList operator+(ClinkedList &other); // not defined
} ;



//============ Function Definitions

/* ~ClinkedList
@input:
@output
 */
template <class T> 
ClinkedList<T>::~ClinkedList()
{
	SlinkedItem<T> *next;

//printf("#### first: %x\n", first);
	curr = first;
	while (curr) {
		next = curr->next;
		delete curr;
		curr = next;
	}
}


/* clear
@input:
@output
 */
template <class T> 
void ClinkedList<T>::clear()
{
	SlinkedItem<T> *next;

	curr = first;
	while (curr) {
		next = curr->next;
		delete curr;
		curr = next;
	}
	first = last = 0;
	num = 0;
	isOrdered = true;
}


/* getFirst
@input:
@output
 */
template <class T> 
T ClinkedList<T>::getFirst()
{
	curr = first;
	if (curr) {
		return curr->data;
	} else {
		return delimiter; // it's not done
	}
}


/* getNext
@input:
@output
 */
template <class T> 
T ClinkedList<T>::getNext()
{
	if (curr && curr->next) {
		curr = curr->next;
		return curr->data;
	} else {
		curr = 0;
		return delimiter; // it's not done
	}
}


/* getLast
@input:
@output
 */
template <class T> 
T ClinkedList<T>::getLast()
{
	if (last) {
		return last->data;
	} else {
		return delimiter;
	}
}



/* pushBack
@input:
@output
 */
template <class T> 
void ClinkedList<T>::pushBack(T value)
{
	SlinkedItem<T> *newitem = new SlinkedItem<T>;
	newitem->data = value;
	newitem->next = 0;
	if (num == 0) {
		first = last = newitem;
	} else {
		last->next = newitem;
		last = newitem;
	}
	num++;
	isOrdered = false;
}


/* pushFront
@input:
@output
 */
template <class T> 
void ClinkedList<T>::pushFront(T value)
{
	SlinkedItem<T> *newitem = new SlinkedItem<T>;
	newitem->data = value;
	if (num == 0) {
		newitem->next = 0;
		first = last = newitem;
	} else {
		newitem->next = first;
		first = newitem;
	}
	num++;
	isOrdered = false;
}


/* popFirst
@input:
@output
 */
template <class T> 
T ClinkedList<T>::popFirst()
{
	T value;
	if (first) {
		value = first->data;
		curr = first->next;
		delete first;
		first = curr;
		num--;
		if (num == 0) {
			last = 0;
			isOrdered = true;
		}
		return value;
	} else {
		return delimiter;
	}
}


/* popLast
@input:
@output
 */
template <class T> 
T ClinkedList<T>::popLast()
{
	T value;
	if (last) {
		value = last->data;
		if (num == 1) {
			delete last;
			first = last = curr = 0;
			isOrdered = true;
		} else {
			for (curr = first; curr->next != last; curr = curr->next) {
				;
			}
			delete last;
			last = curr;
			last->next = 0;
		}
		num--;
		return value;
	} else {
		return delimiter;
	}
}

//-------- Ordered List --------------
/* addOrdered
@input:
@output
 */
template <class T> 
void ClinkedList<T>::addOrdered(T value)
{
	SlinkedItem<T> *newitem = new SlinkedItem<T>;

	newitem->data = value;
	if (num == 0) {
		newitem->next = 0;
		first = last = newitem;
	} else if (value < first->data) {
		newitem->next = first;
		first = newitem;
	} else {
		curr = first;
		while (curr->next && curr->next->data <= value) {
			curr = curr->next;
		}
		if (!isDuplicated && value == curr->data) {
			delete newitem;
			return;
		}
		if (curr == last)  last = newitem;
		newitem->next = curr->next;
		curr->next = newitem;
	}
	num++;
}


/* mergeOrdered
 merge all items of <l2> into <this>, and make <l2> empty
@input:
@output
 */
template <class T> 
void ClinkedList<T>::mergeOrdered(ClinkedList *l2)
{
	ClinkedList<T> *l1 = new ClinkedList<T>;

	T val1, val2;
	if (l2->isEmpty())  return;
	if (num == 0) {
		for (val1 = l2->getFirst(); l2->isNotEnd(); val1=l2->getNext()) {
			pushBack(val1);
		}
		curr = first;
		isOrdered = l2->ordered();
		if (l2->ascending()) {
			setAscending();
		} else {
			setDescending();
		}
	} else {
		l1->mergeOrdered(this);
		clear();

		val1 = l1->popFirst();
		val2 = l2->popFirst();
		while (l1->isNotEnd() && l2->isNotEnd()) {
			if (val1 == val2) {
				if (isDuplicated) {
					pushBack(val1);
					pushBack(val2);
				} else {
					pushBack(val1);
				}
				val1 = l1->popFirst();
				val2 = l2->popFirst();
			} else if (val1 < val2) {
				pushBack(val1);
				val1 = l1->popFirst();
			} else if (val1 > val2) {
				pushBack(val2);
				val2 = l2->popFirst();
			}
		}
		if (l1->isEnd() && l2->isEnd()) {
			pushBack(val1);
			if (isDuplicated)  pushBack(val2);
		}
		if (l1->isNotEnd()) {
			pushBack(val1);
			while (l1->isNotEnd())  pushBack(l1->popFirst());
		}
		if (l2->isNotEnd()) {
			pushBack(val2);
			while (l2->isNotEnd())  pushBack(l2->popFirst());
		}
	}
}



#endif