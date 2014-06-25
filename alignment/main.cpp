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

#include "programm_graph.h"

#include <stdlib.h>


int main(int argc, char **argv)
{
	Programm_graph grf;

	int layer;
	int flag;

	if(argc<5)
	{
		printf("\n\nError in command line parameters:\n");
		printf("     %s <fasta_file> <tree_file> <layer> <path_to_muscle>\n\n",argv[0]);
		return -1;
	}

	layer=atoi(argv[3]);
	
	flag=grf.read(argv[1],argv[2],layer);
	if(flag)
	{
	    printf("Tree '%s' for Fasta '%s' with height %d can't be read.\n",argv[1],argv[2],layer);
	    return -1; 
	}
	
	flag=grf.write(argv[4]);
	if(flag)
	{
	    printf("Can't write graph and Fasta files to disk.\n");
	    return -1;
	}
	
	return 0;
}

