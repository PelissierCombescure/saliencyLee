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


		  Mesh Saliency Version 1.0
                  ------------------------------------

Introduction
------------

This is the demo version of the implenmentation of the ideas as proposed 
in our paper "Mesh Saliency". 
Details of this research are available at http://www.cs.umd.edu/gvil/projects/mesh_saliency.shtml



System requirements
----------------

To run the software in this package, you will need Windows 98/NT/2000/XP.

To make changes and recompile the source code, you will need the Visual C++ 
6.0 or above.



Package Contents
----------------

The main content is a Microsoft windows application project which includes
all the source code in C++ format. 

The .exe file provide here is for people to execute the code without 
recompilation.

The 3D models used in the paper can be found at the following places.

http://graphics.stanford.edu/data/voldata/
http://www.cyberware.com/samples/

We also include a sample sphere data file 'sphere.off' where mesh saliency is
zero across its whole surface.


Installation
------------

It should be sufficient to open the .dsw project file. Then start from there to 
modify and compile the source code.



Program Interface
------------

The user can circularly change the display mode by pressing the 'L' key. The display
modes are:  Model rendering => Curvature Display => Saliency Display =>

You can find the descriptions about commandline arguments by executing 'saliency -h'
at the command line.

Version Information
-------------------

Version 1.0:  Initial release
