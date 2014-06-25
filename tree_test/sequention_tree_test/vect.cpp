/*
 *  This file is a part of the PARUS project.
 *  Copyright (C) 2006  Alexey N. Salnikov
 *  
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * Alexey N. Salnikov (salnikov@cmc.msu.ru)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "linealge.h"
#include "my_time.h"

double mini(double *array, int size)
{
 double min;
 int i;
 
 min=array[0];
 for(i=0; i<size; i++)
   if(min>array[i]) min=array[i];
 
 return min;
}

double maxi(double *array, int size)
{
 double max;
 int i;
 
 max=array[0];
 for (i=0; i<size; i++)
   if(max<array[i]) max=array[i];
   
 return max;
}

double sum_array(double *array, int size)
{
 double sum;
 int i;
 
 sum=0;
 for (i=0; i<size; i++)
   sum+=array[i];
   
 return sum;  
}

int main(int argc,char **argv)
{

  FILE *f;
  int c;
  Vector *vct=NULL;
  int size,i,j;
  double *array, min, max, sum=0;
  

  px_my_time_type time_beg, time_end;
  
  double tmp_min,tmp_max,tmp_sum;
  
  int num_vectors;
  char str[200];	   
  
 
 if(argc<2)
 {
  printf("\t\tSum,Min, and Max of some vectors\n");
  printf("\t\tYou must type the number of vectors as a parameter.\n");
  return -1;   
 } 
 
 
 time_beg=px_my_cpu_time();
 
 for(i=0;i<atoi(argv[1]);i++)
 {
  printf("The work is started with the vector %d\n",i);
  sprintf(str,"vector_%d",i); 
 
  if((f=fopen(str, "r")) == NULL)
  {
	printf("Can't open file: '%s'\n",str);
	return -1;
  }

  vct=new Vector;
  if(vct==NULL)
  {
   printf("Can't create vector object\n");
   return -1;
  }
  
  if(vct->fread(f) == -1)
  {
   printf("Can't read vector\n");
   return -1;
  }
  
  fclose(f);
  
  size=vct->get_size();
  array=(double *)malloc(size*sizeof(double));
  if(array==NULL)
  {
   printf("Memory error\n");
   return -1;
  }
  
  for (j=0;j<size;j++)
  array[j]=vct->element(j);
 
  delete vct; 
   
  tmp_min=mini(array,size);
  tmp_max=maxi(array,size);
  tmp_sum=sum_array(array,size);
  
  free(array);
  
  if((i==0)||(min>tmp_min))
  {
   min=tmp_min;
  }
  
  if((i==0)||(max<tmp_max))
  {
   max=tmp_max;
  }
  
  sum+=tmp_sum;
         
  printf("For vector %d: Min: %lf\tMax: %lf\tSum: %lf\t\n",i,tmp_min,tmp_max,tmp_sum);
  
 }
 
 time_end=px_my_cpu_time();
 
 
 printf("\n\nMin: %lf\nMax: %lf\nSum: %lf\n\n",min,max,sum);
 printf("Time %f\n", (double)(time_end-time_beg));
 
 return 0;
}

