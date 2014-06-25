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


include ../../config
EXECS= graphgen wsfilter

OBJS= ./aflib/aflibFFT.o\
      ./filter/lspwsfilter.o\
      ./libsystem/module_header.o

NETCDF_HOME= /usr/local/netcdf

LOCAL_INCLUDE= -I ./aflib\
               -I ./filter\
               -I ./libsystem\
               -I $(NETCDF_HOME)/include



all: $(EXECS)


wav2nc:
	$(MAKE) -C wav2nc NETCDF_HOME=$(NETCDF_HOME)


graphgen: graph_gen.c
	$(CC) $(CC_FLAGS) $(CC_INCLUDE) -o graphgen graph_gen.c



wsfilter: $(OBJS) wsfilter.o
	$(MPI_CCLINKER) $(MPI_CCLINKER_FLAGS) $(MPI_LIB_PATH) -L $(NETCDF_HOME)/lib wsfilter.o $(OBJS) -o wsfilter $(MPI_LIBS) -lnetcdf

clean:
	/bin/rm -f *.o $(EXECS) $(OBJS)

.c.o:
	$(MPI_cc) $(MPI_cc_FLAGS) $(MPI_cc_INCLUDE) $(LOCAL_INCLUDE) -c $*.c -o $*.o

.cpp.o:
	$(MPI_CC) $(MPI_CC_FLAGS) $(MPI_CC_INCLUDE) $(LOCAL_INCLUDE) -c $*.cpp -o $*.o

.PHONY: wav2nc
