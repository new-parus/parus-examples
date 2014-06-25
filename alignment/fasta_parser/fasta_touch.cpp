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


#include "fasta.h"

#include <stdio.h>

int main(int argc,char **argv)
{

	Fasta fasta;
		
	if(argc!=3)
	{
		printf("\n This programm reads one fasta file and copies it to other fasta file\n");
		printf(" fasta_touch source destination\n\n");
		return 0;
	}

	fasta.read(argv[1]);
	fasta.write(argv[2]);
	
		
	return 0;
	
}

