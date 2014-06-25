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

include ../../../config

LIB_DIR=$(INSTALL_DIR)/lib
#cc_INCLUDE=$(MPI_cc_INCLUDE) -I $(INSTALL_DIR)/include
#CC_INCLUDE=$(MPI_CC_INCLUDE) -I $(INSTALL_DIR)/include

LIB_PATH= -L $(LIB_DIR)
LIBS= -lparus_common

EXECS	  = vect

all: $(EXECS) 



vect: vect.o 
	$(MPI_CCLINKER)	$(MPI_CCLINKER_FLAGS)  -o vect vect.o $(LIB_PATH) $(LIBS)

clean:
	/bin/rm -f *.o $(EXECS) $(EXECS)_*

.c.o:
	$(MPI_cc) $(MPI_cc_FLAGS) $(cc_INCLUDE) -c $*.c -o $*.o  

.cpp.o:
	$(MPI_CC) $(MPI_CC_FLAGS) $(CC_INCLUDE) -c $*.cpp -o $*.o
