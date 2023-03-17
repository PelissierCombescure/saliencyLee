#undef _globals
#include <fstream.h>
#include <iostream.h>
#include "macros.h"
#include "geometry.h"
//#include "functions.h"

/* read the model polygons and store them as tris */
void read_off_model(char *filename)
{ 
  char		line[128];
  FILE		*fp;
  long		i, j, k;
  long		num_v;
  int		color[3], color_present;
  int		dummy_int, num_polys;
  VertType	*Vert_list;
  int vert0, vert1, vert2;
  long		file_offset;

  double        x_upper_bound = -8.0e300;	//upper limit of data in x direction
  double        x_lower_bound = 8.0e+300;	//lower limit of data in x direction	
  double        y_upper_bound = -8.0e+300;	//upper limit of data in y direction 
  double        y_lower_bound = 8.0e+300;	//lower limit of data in y direction
  double        z_upper_bound = -8.0e+300;	//upper limit of data in z direction
  double        z_lower_bound = 8.0e+300;	//lower limit of data in z direction
  double		x_center, y_center, z_center;		//center position of data
  double	    x_half_range, y_half_range, z_half_range;	//half range in each direction
  double	    max_half_range;				//maximum of x,y,z_half_range  

  /* read model triangles */
  if (!(fp = fopen(filename, "r")))
  { 
	FILE *fp1;

	fp1 = fopen("error_info", "a");
	fprintf(fp1, "read_model(): can't open input file %s\n", filename);
	fclose(fp1);

    exit(-1);
  }
/*  else{
  	FILE *fp2;
	fp2 = fopen("time_file2", "w");

		fprintf(fp2,"\nThe floating transformation takes seconds.\n");


	fclose(fp2);
  }
*/
  /* try to deal with options in file */
  fscanf(fp, "%s \n", line);

  /* count max verts and number of tris */
  Num_tris = Num_verts = 0;
  fscanf(fp, "%d %d %d\n", &Num_verts, &num_polys, &dummy_int);


  /* read all the verts into coords and norms and tris into tri_list*/
  ALLOCN(Vert_list, VertType, Num_verts);

  for (i=0;i<Num_verts;i++)
  { 
    Vert_list[i].id = i;
	double tmp_x, tmp_y, tmp_z;
  
    fscanf(fp, "%lf %lf %lf\n", &tmp_x, &tmp_y, &tmp_z);
	Vert_list[i].coord[0] =  tmp_x;
	Vert_list[i].coord[1] =  tmp_y;
	Vert_list[i].coord[2] =  tmp_z;
	x_upper_bound = (x_upper_bound < Vert_list[i].coord[0])? Vert_list[i].coord[0]:x_upper_bound;
    x_lower_bound = (x_lower_bound > Vert_list[i].coord[0])? Vert_list[i].coord[0]:x_lower_bound;
    y_upper_bound = (y_upper_bound < Vert_list[i].coord[1])? Vert_list[i].coord[1]:y_upper_bound;
    y_lower_bound = (y_lower_bound > Vert_list[i].coord[1])? Vert_list[i].coord[1]:y_lower_bound;
    z_upper_bound = (z_upper_bound < Vert_list[i].coord[2])? Vert_list[i].coord[2]:z_upper_bound;
    z_lower_bound = (z_lower_bound > Vert_list[i].coord[2])? Vert_list[i].coord[2]:z_lower_bound; 
  }
  
  
  // Normalize the dataset to [-1.0, 1.0] range
  x_center = (x_upper_bound + x_lower_bound)/2.0;
  y_center = (y_upper_bound + y_lower_bound)/2.0;
  z_center = (z_upper_bound + z_lower_bound)/2.0;

  x_half_range = (x_upper_bound - x_lower_bound)/2.0;
  y_half_range = (y_upper_bound - y_lower_bound)/2.0;
  z_half_range = (z_upper_bound - z_lower_bound)/2.0;

  max_half_range = (x_half_range >= y_half_range)? x_half_range:y_half_range;
  max_half_range = (max_half_range >= z_half_range)? max_half_range:z_half_range;

  for (i=0;i<Num_verts;i++)
  {   
	Vert_list[i].coord[0] =  (Vert_list[i].coord[0]-x_center)/max_half_range;
	Vert_list[i].coord[1] =  (Vert_list[i].coord[1]-y_center)/max_half_range;
	Vert_list[i].coord[2] =  (Vert_list[i].coord[2]-z_center)/max_half_range;
  }


  /* figure out how many tris to read */
  file_offset = ftell(fp);
  fgets(line, sizeof(line), fp);
  sscanf(line, "%d", &k);
  Num_tris = (k - 2);
  strtok(line, " ");
  i = 1;
  while (strtok(NULL," ") != NULL) i++;
  color_present = (i > k+2);

  for(i = 1; i < num_polys; i++)
  { fgets(line, sizeof(line), fp);
    sscanf(line, "%d", &k);
    Num_tris += (k - 2);
  }
  fseek(fp, file_offset, 0);

  ALLOCN(Tri_list, TriType, Num_tris); 
  k = 0;
  while (fscanf(fp, "%d", &num_v) != EOF)
  { 
    fscanf(fp, "%d", &vert0);
    fscanf(fp, "%d", &vert1);
    for (j=0;j<num_v - 2;j++)
    { fscanf(fp, "%d", &vert2);
  
    Tri_list[k].vert_ptrs[0] = &Vert_list[vert0];
    Tri_list[k].vert_ptrs[1] = &Vert_list[vert1];
    Tri_list[k].vert_ptrs[2] = &Vert_list[vert2];
  
    vert1 = vert2;
    Tri_list[k].id = k;
    k++;
//	if(k%100 == 99) printf("%d!\n", k);
    }
    if (color_present)
    { /* read in the color */
      fscanf(fp,"%d %d %d\n", &color[0], &color[1], &color[2]);
      for(j = k-1; j >= k - num_v +  2; j--)
      { Tri_list[j].color[0] = color[0];
        Tri_list[j].color[1] = color[1];
        Tri_list[j].color[2] = color[2];
      }
    }
  }

  printf("      Read %d Triangles;  %d Vertices\n",
         Num_tris, Num_verts);
  printf("      the range of volume is:\n");

 // fflush(stdout);
 
  return;
}

