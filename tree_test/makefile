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


#
# This make file is a Makefile for parus system example
#



all: sequention_test generators


sequention_test:
	$(MAKE) -C sequention_tree_test

generators:
	$(MAKE) -C generator_tree

clean:
	$(MAKE) clean -C sequention_tree_test
	$(MAKE) clean -C generator_tree

fclean:
	/bin/rm -f *.o graph.grf c_mpi.cpp sch.grf head_* vector_* 


