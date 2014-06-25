/*
 *  This file is a part of the PARUS project.
 *  Copyright (C) 2006  Alexey N. Salnikov, Alexander S. Ivanov
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
 * Alexander S. Ivanov (alex@angel.cs.msu.su)
 *
 */

#include <stdio.h>
#include <errno.h>

int main(int argc, char **argv)
{
	int n;
	int i;
	FILE *f;
	
	if (argc < 2)
	{
		printf("\n\nThis program generates an input vector for the neural network.\n");
		printf(" You must specify the  size of the input vector.\n");
		printf(" The size of the input vector must be close to number of inputs in\n");
		printf(" the neural network.\n\n");
		printf(" For example: %s 10\n\n",argv[0]);
		return 0;
	}
	
	n = atoi(argv[1]);
	
	f=fopen("input.dat","wt");
	perror(NULL);
		
	for (i=0; i<n; i++)
		fprintf(f,"%f ",(float)i);
	
	return 0;
}
