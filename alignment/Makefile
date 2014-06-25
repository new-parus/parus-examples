#
# This file is a part of the PARUS project and  makes the core of the parus system
# Copyright (C) 2006  Alexey N. Salnikov (salnikov@cmc.msu.ru)
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#


all: tree_to_program

SOURCES= \
		main.cpp\
		programm_graph.cpp\
		fasta_parser/fasta.cpp\
		tree_parser/phil_tree.cpp\
		tree_parser/tree_read.cpp\
		tree_parser/file_read.cpp		
		
HEADERS= \
		programm_graph.h\
		fasta_parser/fasta.h\
		tree_parser/phil_tree.h\
		tree_parser/tree_read.h\
		tree_parser/file_read.h


tree_to_program: $(SOURCES) $(HEADERS)
	g++ -g $(SOURCES) -o tree_to_program

clean:
	rm -f ./tree_to_program
