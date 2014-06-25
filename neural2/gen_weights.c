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

int main(int argc,char **argv)
{
	int i, j, l;
	int num_inputs,num_layers;
	double buf;
	
	FILE *f;
	
	if (argc < 3)
	{
		printf("\n\nThis program generates the weights for the neural network.\n");
		printf("You must specify the number of inputs and the number of layers.\n\n");
		printf("%s number_of_inputs number_of_layers\n\n"); 
		printf("For example: %s 10 3\n\n",argv[0]);
		return 0;
	}
	
	num_inputs = atoi(argv[1]);
	num_layers = atoi(argv[2]);
		
	f=fopen("weights.dat","wb");
	perror(NULL);
	for (l=0; l<num_layers; l++)
	for (i=0; i<num_inputs; i++)
	for (j=0; j<num_inputs; j++)
	{
		if ( i == j+1 || i == 0 && j == num_inputs-1 ) 
			buf=1;
		else
			buf=0;
		
		fwrite(&buf,sizeof(double),1,f);
	}
	
	fclose(f);
	return 0;
	
}
