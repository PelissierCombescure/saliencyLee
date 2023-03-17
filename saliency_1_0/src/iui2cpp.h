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
#ifndef _UI_H
#define _UI_H

/* mouse */
#define UI_LEFT_BUTTON 0
#define UI_MIDDLE_BUTTON 1
#define UI_RIGHT_BUTTON 2

#define UI_MOUSE_DOWN 0
#define UI_MOUSE_UP 1

/* keyboard */
#define UI_KEY_DOWN 0
#define UI_KEY_UP 1

#define UI_LEFT_ARROW 100
#define UI_UP_ARROW 101
#define UI_RIGHT_ARROW 102
#define UI_DOWN_ARROW 103
#define UI_PAGE_UP 104
#define UI_PAGE_DOWN 105

/* Modifiers return mask. */ 
#define UI_ACTIVE_SHIFT               1
#define UI_ACTIVE_CTRL                2
#define UI_ACTIVE_ALT                 4
  
/* mode */
#define UI_ARCBALL_MODE 0
#define UI_TRACKBALL_MODE 1
#define UI_WALK_MODE 2
  
#define UI_DEFAULT_MODE UI_ARCBALL_MODE

#define _EXTERN_C	extern "C"

/* 
 * Initialize the arcball and walk objects
 * Also, set the stack matrix to be identity matrix and
 * window to be a rectangle of length 1.
 * Default mode is ARCBALL_MODE
 */ 
_EXTERN_C void uiInit();  

/*
 * Set the width and height of viewing window when 
 * user resizes the window 
 */
_EXTERN_C void uiReshape(unsigned int, unsigned int);

/* 
 * Changes the movement mode to the specified mode
 * If the mode is set to WALK_MODE, continue from the old position in 
 * the previous WALK_MODE.
 * If the mode is set to ARCBALL_MODE, continue from the last position 
 */
_EXTERN_C void uiMode(int);

/*
 * Return the matrix for transforming objects from the initial position
 * to the final position. 
 * Call the corresponding matrix routine wrt to movement mode 
 */
_EXTERN_C void uiMatrix(float [4][4]);

/*
 * Reset the movement of the arcball and walk objects
 * Also, set the stack matrix to be identity matrix.
 * This function works in only ARCBALL_MODE right now
 * - Added by Chang Ha Lee
 */
_EXTERN_C void uiReset();

/*
 * Set the movement of the arcball and walk objects using the matrix
 * given by users.
 * The matrix is assumed to be a similarity transformation
 * This function works in only ARCBALL_MODE right now
 * - Added by Chang Ha Lee
 */
_EXTERN_C void uiSetMatrix(float [4][4]);

/* 
 * Copy the matrix [s] to the stack
 */
_EXTERN_C void uiStack(float [4][4]);

/* 
 * Set the parameters for the arcball movement
 */
_EXTERN_C void uiSetArcballParameters(float, float, float);

/* 
 * Set the parameters for the walkthorugh
 */
_EXTERN_C void uiSetWalkParameters(float, float, float, float, float);

/* 
 * Sets the camera position to (x, y, z)
 * Must be called when the look at position changes 
 */
_EXTERN_C void uiSetCameraPosition(float, float, float);

/* 
 * Set the mouse and keyboard modifiers
 */
_EXTERN_C void uiSetModifiers(int); 

/* 
 * Get the mouse and keyboard modifiers
 */
_EXTERN_C int uiGetModifiers();
 
/* 
 * Key down listener
 */
_EXTERN_C void uiKeyDownListener(unsigned char, int, int);

/* 
 * Key up listener
 */
_EXTERN_C void uiKeyUpListener(unsigned char, int, int);

/* 
 * Special key down listener
 * If the current mode is WALK_MODE, call the routine in walk object
 */
_EXTERN_C void uiSpecialKeyDownListener(unsigned char, int , int);

/* 
 * Special key up listener
 * If the current mode is WALK_MODE, call the routine in walk object
 */
_EXTERN_C void uiSpecialKeyUpListener(unsigned char, int , int);

/* 
 * Mouse action listener
 * Call the corresponding routine wrt to the current mode
 */
_EXTERN_C void uiMouseListener(int, int, int, int);

 /* 
 * Mouse drag listener
 * Call the corresponding routine wrt to the current mode
 */ 
_EXTERN_C void uiMouseDragListener(int, int, int);
  
#endif    
 
 



