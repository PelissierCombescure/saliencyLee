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
#include <math.h>
#include "colors.h"

float hue = 350.0f;
float intensity = 0.25f;
float saturation = 0.25f;

void HSVtoRGB(float *r, float *g, float *b, float h, float s, float v )
{
	int i;
	float f, p, q, t;
	if( s == 0 ) {
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}
	h /= 60;			// sector 0 to 5
	i = (int)floor( h );
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );
	switch( i ) {
		case 0:
			*r = v;
			*g = t;
			*b = p;
			break;
		case 1:
			*r = q;
			*g = v;
			*b = p;
			break;
		case 2:
			*r = p;
			*g = v;
			*b = t;
			break;
		case 3:
			*r = p;
			*g = q;
			*b = v;
			break;
		case 4:
			*r = t;
			*g = p;
			*b = v;
			break;
		default:		// case 5:
			*r = v;
			*g = p;
			*b = q;
			break;
	}
}

void change_hue(float hue_inc) {
	hue = hue + hue_inc;
	while(hue > 360) hue = hue - 360;
	while(hue < 0)   hue = hue + 360;
}

void change_intensity(float intensity_inc) {
	intensity = intensity + intensity_inc;
	while(intensity > 1.0) intensity = intensity - 1.0f;
	while(intensity < 0)   intensity = intensity + 1.0f;
}

void change_saturation(float sat_inc) {
	saturation = saturation + sat_inc;
	while(saturation > 1.0) saturation = saturation - 1.0f;
	while(saturation < 0.0) saturation = saturation + 1.0f;
}


unsigned char *getColorCode(unsigned char color[3], float val, float minv, float maxv)
{
	float r, g, b, h, s = 0.8, v = 1;
	float cval = (val - minv) / (maxv - minv);
	float highh = 233;   // h 0~350

	h = highh - highh*cval;
	v = 0.5 + cval*(1-0.5);
	HSVtoRGB(&r, &g, &b, h, s, v);
	color[0] = r*255;
	color[1] = g*255;
	color[2] = b*255;
	
	return color;
}
