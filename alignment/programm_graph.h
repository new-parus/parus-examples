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

#ifndef __PROGRAMM_GRAPH_H__
#define __PROGRAMM_GRAPH_H__

#include "tree_parser/phil_tree.h"
#include "fasta_parser/fasta.h"


/**
 * This class allows create program in PARUS 
 * graph file format.
 *
 * Attantion this program reqiries muscle http://www.drive5.com/muscle/.
 * 
 */
class Programm_graph
{
 protected:
	parus::Phil_tree tree;
	Fasta fasta; 

 public:
	int read(const char *fasta_file,const char *tree_file,int layer);
	int write(const char *path_to_muscle);
		
};

#endif /* __PROGRAMM_GRAPH_H__ */

