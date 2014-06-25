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

int get_num_input_edges(int node_number,int array_size,int box_size);
int get_layer(int node_number,int array_size,int box_size);
int get_position_in_layer(int node_number,int array_size,int box_size);
int get_num_nodes_on_layer(int layer,int array_size,int box_size);
int get_input_edges(int node_number,int array_size,int box_size,int num_input_edges,int *input_edges);
int get_node_number(int layer,int position,int array_size,int box_size);
int get_recv_node(int edge,int array_size,int box_size);
int get_node_order_in_box(int node_number,int array_size,int box_size);

/*********************************************************************************************************/
int get_num_input_edges(int node_number,int array_size,int box_size)
{
 int layer;
 int position;
 int num_nodes_on_previous_layer;
 int beg,end;
 layer=get_layer(node_number,array_size,box_size);
 //printf("get_num_input_edges(%d,%d,%d) Layer %d\n",node_number,array_size,box_size,layer);
 if(layer==0) return 0;
 num_nodes_on_previous_layer=get_num_nodes_on_layer(layer-1,array_size,box_size);
 /*
 printf("get_num_input_edges(%d,%d,%d) num_nodes_on_previous_layer %d\n",
        node_number,
	array_size,
	box_size,
	num_nodes_on_previous_layer);
 */
 position=get_position_in_layer(node_number,array_size,box_size);
 /*
 printf("get_num_input_edges(%d,%d,%d) position %d\n",
        node_number,
	array_size,
	box_size,
	position);
 */
 beg=box_size*position;
 end=box_size*(position+1)-1;
 if(end>=num_nodes_on_previous_layer) end=num_nodes_on_previous_layer-1;
 /*
 printf("get_num_input_edges(%d,%d,%d) %d - %d\n",
        node_number,
	array_size,
	box_size,
	beg,end);
 */
 return end-beg+1; 
}
/*********************************************************************************************************/
int get_input_edges(int node_number,int array_size,int box_size,int num_input_edges,int *input_edges)
{
 int layer;
 int position;
 int i;
 int first_node;
 
 layer=get_layer(node_number,array_size,box_size);
 position=get_position_in_layer(node_number,array_size,box_size);
 first_node=box_size*position;
 
 for(i=0;i<num_input_edges;i++)
 {
  input_edges[i]=get_node_number(layer-1,first_node+i,array_size,box_size);
  /*
  printf("%d=input_edges[%d]=get_node_number(%d,%d,%d,%d);\n",
         input_edges[i],
	 i,
	 layer-1,
	 first_node+i,
	 array_size,
	 box_size);
  */
 }
 return 0;
}
/*********************************************************************************************************/
int get_layer(int node_number,int array_size,int box_size)
{
 int layer_n_box;
 int num_nodes=0;
 int tmp;
 int i=0;
 
 layer_n_box=array_size;
 while(layer_n_box>1)
 {
  tmp=layer_n_box/box_size;
  if(layer_n_box%box_size!=0) tmp++;
  layer_n_box=tmp;
  num_nodes+=layer_n_box;
  if(node_number<num_nodes) return i;
  i++;
 }
 return -1; 
}
/*********************************************************************************************************/
int get_position_in_layer(int node_number,int array_size,int box_size)
{
 int num_nodes=0;
 int layer;
 int i=0;
 
 layer=get_layer(node_number,array_size,box_size);
 for(i=0;i<layer;i++)
 {
  num_nodes+=get_num_nodes_on_layer(i,array_size,box_size);
 }
 
 return node_number-num_nodes;
}
/*********************************************************************************************************/
int get_num_nodes_on_layer(int layer,int array_size,int box_size)
{
 int layer_n_box;
 int num_nodes=0;
 int tmp;
 int i=0;
 
 layer_n_box=array_size;
 while(layer_n_box>1)
 {
  tmp=layer_n_box/box_size;
  if(layer_n_box%box_size!=0) tmp++;
  layer_n_box=tmp;
  num_nodes+=layer_n_box;
  if(i==layer) return layer_n_box;
  i++;
 }
 return -1;
}
/********************************************************************************************************/
int get_node_number(int layer,int position,int array_size,int box_size)
{
 int num_nodes=0;
 int i=0;
 
 for(i=0;i<layer;i++)
 {
  num_nodes+=get_num_nodes_on_layer(i,array_size,box_size);
  //printf("num_nodes=%d\n",num_nodes);
 }
 return num_nodes+position;
}
/********************************************************************************************************/
int get_recv_node(int edge,int array_size,int box_size)
{
 int layer;
 int position;
 int recv_position;
 
 layer=get_layer(edge,array_size,box_size);
 position=get_position_in_layer(edge,array_size,box_size);
 recv_position=position/box_size;
 return get_node_number(layer+1,recv_position,array_size,box_size);
}
/********************************************************************************************************/
int get_node_order_in_box(int node_number,int array_size,int box_size)
{
 int layer;
 int position;
 int box_position;
 layer=get_layer(node_number,array_size,box_size);
 position=get_position_in_layer(node_number,array_size,box_size);
 box_position=position%box_size;
 return box_position;
 
}
/********************************************************************************************************/
