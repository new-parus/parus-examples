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
#include "my_malloc.h"

#include "funcs.h"

int main(int argc, char **argv)
{
 int array_size;
 int box_size;
 int num_nodes=0;
 int layer_n_box;
 int num_layers;
 int tmp;
 int i=0;
 int j;
 int num_input_edges;
 int layer=0;
 int tmp_num_nodes=0;
 int *input_edges=NULL;
 char str[300];
 int first,last;
 FILE *f;
  
 if(argc<3)
 {
  printf("This program generates the graph of the algorithm for max,min finding in the array.\n");
  printf("Parametrs are: %s <array_size> <box_size>\n\n",argv[0]);
  printf("Box size must be smaller than the array size.\n"); 
  return -1;
 }
 array_size=atoi(argv[1]);
 box_size=atoi(argv[2]);
 
 printf("array_size = %d\n",array_size);
 printf("box_size = %d\n",box_size);
  
 f=fopen("graph.grf","w");
 if(f==NULL)
 {
  printf("Can't create file 'graph.grf'\n");
  return -1;
 }
 
 layer_n_box=array_size;
 while(layer_n_box>1)
 {
  tmp=layer_n_box/box_size;
  if(layer_n_box%box_size!=0) tmp++;
  layer_n_box=tmp;
  num_nodes+=layer_n_box;
  printf("Layer %d: num nodes on this layer is %d\n",i,layer_n_box);
  i++;
 }
 num_layers=i;
  
 
 fprintf(f,"<GRAPH_BEGIN>\n");
 fprintf(f," header \"header\"\n");
 fprintf(f," root \"\"\n");
 fprintf(f," tail \"\"\n");
 fprintf(f," num_nodes %d\n",num_nodes);
 fprintf(f,"<NODES_BEGIN>\n"); 
 layer_n_box=array_size;
 while(layer_n_box>1)
 {
  tmp=layer_n_box/box_size;
  if(layer_n_box%box_size!=0) tmp++;
  layer_n_box=tmp;
  for(i=tmp_num_nodes;i<tmp_num_nodes+layer_n_box;i++)
  {
   fprintf(f," <NODE_BEGIN>\n");
   fprintf(f,"  number %d\n",i);
   fprintf(f,"  type 0\n");
   if(layer==0)
   {
    fprintf(f,"  weight %d\n",box_size);
   }
   else
   {
    fprintf(f,"  weight %d\n",get_num_input_edges(i,array_size,box_size));
   }
   fprintf(f,"  layer %d\n",layer);
   
   num_input_edges=get_num_input_edges(i,array_size,box_size);
   //printf("%d\n",num_input_edges);
   input_edges=(int *)malloc(num_input_edges*sizeof(int));
   if(input_edges==NULL)
   {
    printf("Memory error\n");
    fclose(f);
    return -1;
   }
   
   get_input_edges(i,array_size,box_size,num_input_edges,input_edges);
   
   fprintf(f,"  num_input_edges %d\n",num_input_edges);
   fprintf(f,"  edges ( ");
   for(j=0;j<num_input_edges;j++)
   {
    fprintf(f,"%d ",input_edges[j]);
   }
   fprintf(f,")\n");
   free(input_edges);
   input_edges=NULL;
   
   if((layer>0)&&(layer<num_layers-1))
   {
    fprintf(f,"  num_output_edges 1\n");
    fprintf(f,"  edges ( %d )\n",i);
    fprintf(f,"  head \"head_%d\"\n",i);
    fprintf(f,"  body \"body\"\n");
    fprintf(f,"  tail \"tail\"\n");
   }
   if(layer==0)
   {
    fprintf(f,"  num_output_edges 1\n");
    fprintf(f,"  edges ( %d )\n",i);
    fprintf(f,"  head \"head_%d\"\n",i);
    fprintf(f,"  body \"body\"\n");
    fprintf(f,"  tail \"tail_first\"\n");
   }
   if(layer==num_layers-1)
   {
    fprintf(f,"  num_output_edges 0\n");
    fprintf(f,"  edges (  )\n");
    fprintf(f,"  head \"head_%d\"\n",i);
    fprintf(f,"  body \"body_last\"\n");
    fprintf(f,"  tail \"tail\"\n");
   }
   
   fprintf(f," <NODE_END>\n\n");
  }
  tmp_num_nodes+=layer_n_box;
  //printf("Layer %d: num nodes on this layer is %d\n",layer,layer_n_box);
  layer++;
 }
 
 fprintf(f,"<NODES_END>\n\n");
 fprintf(f," num_edges %d\n\n",num_nodes-1);
 fprintf(f,"<EDGES_BEGIN>\n");
 
 for(i=0;i<num_nodes-1;i++)
 {
  fprintf(f," <EDGE_BEGIN>\n");
  fprintf(f,"  number %d\n",i);
  fprintf(f,"  weight %d\n",sizeof(double));
  fprintf(f,"  type GRAPH_NONE\n");
  fprintf(f,"  num_var 3\n");
  fprintf(f,"  num_send_nodes 1\n");
  fprintf(f,"  send_nodes ( %d )\n",i);
  fprintf(f,"  num_recv_nodes 1\n");
  fprintf(f,"  recv_nodes ( %d )\n",get_recv_node(i,array_size,box_size));
  
  fprintf(f,"  <SEND_BEGIN>\n\n");
  fprintf(f,"   <CHUNK_BEGIN>\n");
  fprintf(f,"    name         \"min\"\n");
  fprintf(f,"    type         GRAPH_DOUBLE\n");
  fprintf(f,"    left_offset  \"0\"\n");
  fprintf(f,"    right_offset \"1\"\n");
  fprintf(f,"   <CHUNK_END>\n\n");
  fprintf(f,"   <CHUNK_BEGIN>\n");
  fprintf(f,"    name         \"max\"\n");
  fprintf(f,"    type         GRAPH_DOUBLE\n");
  fprintf(f,"    left_offset  \"0\"\n");
  fprintf(f,"    right_offset \"1\"\n");
  fprintf(f,"   <CHUNK_END>\n\n");
  fprintf(f,"   <CHUNK_BEGIN>\n");
  fprintf(f,"    name         \"sum\"\n");
  fprintf(f,"    type         GRAPH_DOUBLE\n");
  fprintf(f,"    left_offset  \"0\"\n");
  fprintf(f,"    right_offset \"1\"\n");
  fprintf(f,"   <CHUNK_END>\n\n");
  fprintf(f,"  <SEND_END>\n");
  
  fprintf(f,"  <RECIEVE_BEGIN>\n\n");
  fprintf(f,"   <CHUNK_BEGIN>\n");
  fprintf(f,"    name         \"min_array[%d]\"\n",get_node_order_in_box(i,array_size,box_size));
  fprintf(f,"    type         GRAPH_DOUBLE\n");
  fprintf(f,"    left_offset  \"0\"\n");
  fprintf(f,"    right_offset \"1\"\n");
  fprintf(f,"   <CHUNK_END>\n\n");
  fprintf(f,"   <CHUNK_BEGIN>\n");
  fprintf(f,"    name         \"max_array[%d]\"\n",get_node_order_in_box(i,array_size,box_size));
  fprintf(f,"    type         GRAPH_DOUBLE\n");
  fprintf(f,"    left_offset  \"0\"\n");
  fprintf(f,"    right_offset \"1\"\n");
  fprintf(f,"   <CHUNK_END>\n\n");
  fprintf(f,"   <CHUNK_BEGIN>\n");
  fprintf(f,"    name         \"sum_array[%d]\"\n",get_node_order_in_box(i,array_size,box_size));
  fprintf(f,"    type         GRAPH_DOUBLE\n");
  fprintf(f,"    left_offset  \"0\"\n");
  fprintf(f,"    right_offset \"1\"\n");
  fprintf(f,"   <CHUNK_END>\n\n");
  fprintf(f,"  <RECIEVE_END>\n");
  
  fprintf(f," <EDGE_END>\n\n");
 }  /* End edge cycle */
 
 fprintf(f,"<EDGES_END>\n"); 
 fprintf(f,"<GRAPH_END>\n");
 fclose(f);
 
 for(i=0;i<num_nodes;i++)
 {
  sprintf(str,"head_%d",i);
  f=fopen(str,"w");
  if(f==NULL)
  {
   printf("Can't create file '%s'\n",str);
   return -1; 
  }
  fprintf(f," /* Header for the node %d */\n",i);
  fprintf(f," double min,max,sum;\n");
  fprintf(f," int size,i;\n");
  if(!get_layer(i,array_size,box_size))
  {
   fprintf(f," double *array;\n");
   fprintf(f," double *min_array,*max_array,*sum_array;\n\n");
   fprintf(f," FILE *f;\n");
   fprintf(f," Vector *vct=NULL;\n");
   first=i*box_size;
   last=(i+1)*box_size-1;
   if(last>=array_size) last=array_size-1;
   fprintf(f," size=%d;\n",last-first+1);
   fprintf(f," vct=new Vector;\n");
   fprintf(f," if(vct==NULL)\n");
   fprintf(f," {\n");
   fprintf(f,"  printf(\"Memory allocation error!\\n\");\n");
   fprintf(f,"  return -1;\n");
   fprintf(f," }\n\n");
   fprintf(f," array=(double *)malloc(size*sizeof(double));\n");
   fprintf(f," if(array==NULL)\n");
   fprintf(f," {\n");
   fprintf(f,"  printf(\"Memory allocation error!\\n\");\n");
   fprintf(f,"  return -1;\n");
   fprintf(f," }\n\n");
   fprintf(f," min_array=array;\n");
   fprintf(f," max_array=array;\n");
   fprintf(f," sum_array=array;\n");
   fprintf(f," f=fopen(\"vector_%d\",\"r\");\n",i);
   fprintf(f," if(f==NULL)\n");
   fprintf(f," {\n");
   fprintf(f,"  printf(\"Can't open file \\\"vector_%d\\\"\\n\");\n",i);
   fprintf(f,"  return -1;\n");
   fprintf(f," }\n\n");
   fprintf(f," if(vct->fread(f) == -1 )\n");
   fprintf(f," {\n");
   fprintf(f,"  printf(\"Can't read vector from file \\\"vector_%d\\\"!\\n\");\n",i);
   fprintf(f,"  return -1;\n");
   fprintf(f," }\n\n");
   fprintf(f," fclose(f);\n");
   fprintf(f," for(i=0;i<%d;i++) array[i]=vct->element(i);\n",last-first+1);
   fprintf(f," delete vct;\n\n");
  }
  else
  {
   fprintf(f," double *min_array,*max_array,*sum_array;\n\n");
   fprintf(f," size=%d;\n",get_num_input_edges(i,array_size,box_size));
   fprintf(f," min_array=(double *)malloc(size*sizeof(double));\n");
   fprintf(f," max_array=(double *)malloc(size*sizeof(double));\n");
   fprintf(f," sum_array=(double *)malloc(size*sizeof(double));\n");
   fprintf(f," if((min_array==NULL)||(max_array==NULL)||(sum_array==NULL))\n");
   fprintf(f," {\n");
   fprintf(f,"  printf(\"Memory allocation error!\\n\");\n");
   fprintf(f,"  return -1;\n");
   fprintf(f," }\n\n");
  }
  fclose(f);
 }
 
 
 return 0;
}
