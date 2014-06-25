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

#include<stdio.h>
#include<stdlib.h>

int main(int argc,char **argv)
{
 int size;
 int i,j;
 int num_box;
 int box_size;
 int tmp_size;
 int begin,end;
 char str[300];
 FILE *f;
 if(argc<3)
 {
  printf("Vector generator. Please type the vector length and the box size\n\n");
  return -1;
 }
 size=atoi(argv[1]);
 box_size=atoi(argv[2]);
 
 num_box=size/box_size;
 if(size%box_size) num_box++;
 
 
 for(i=0;i<num_box;i++)
 { 
  sprintf(str,"vector_%d",i);
  f=fopen(str,"w");
  begin=box_size*i;
  end=box_size*(i+1)-1;
  if(end>=size) end=size-1;
  tmp_size=end-begin+1;
  fprintf(f,"Vector %d\n",tmp_size);
  for(j=0;j<tmp_size;j++)
  {
   fprintf(f,"1 ");
  }
  fclose(f);
 }
 return 0;
}