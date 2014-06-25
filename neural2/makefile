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
# Make file for neural network example.
#
include ../../config

EXECS= gen_input text2bin gen_graph gen_weights

all: $(EXECS)

gen_input: gen_input.c
	$(CC) $(CC_FLAGS) gen_input.c -o gen_input

gen_weights: gen_weights.c
	$(CC) $(CC_FLAGS) gen_weights.c -o gen_weights

gen_graph: gen_graph.cpp
	$(CCC) $(CCC_FLAGS) gen_graph.cpp -o gen_graph

text2bin: text2bin.c
	$(CC)  $(CC_FLAGS) text2bin.c -o text2bin

clean:
	rm -f $(EXECS)

delete:
	rm -f *.dat graph.grf header.c