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

#ifndef __FUNCS_H__
#define __FUNCS_H__

extern int get_num_input_edges(int node_number,int array_size,int box_size);
extern int get_layer(int node_number,int array_size,int box_size);
extern int get_position_in_layer(int node_number,int array_size,int box_size);
extern int get_num_nodes_on_layer(int layer,int array_size,int box_size);
extern int get_input_edges(int node_number,int array_size,int box_size,int num_input_edges,int *input_edges);
extern int get_node_number(int layer,int position,int array_size,int box_size);
extern int get_recv_node(int edge,int array_size,int box_size);
extern int get_node_order_in_box(int node_number,int array_size,int box_size);

#endif /* __FUNCS_H__ */
