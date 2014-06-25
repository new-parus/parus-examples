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

include ../config

all:
#	$(MAKE) -C  ./egor
#	$(MAKE) -W -C ./egor2
	$(MAKE) -C ./cascade
#	$(MAKE) -C ./wav
#	$(MAKE) -C ./wav1
	$(MAKE) -C ./example1
	$(MAKE) -C ./neural
	$(MAKE) -C ./neural2
	$(MAKE) -C ./simple_example
	$(MAKE) -C ./scalarmul
	$(MAKE) -C ./Builder
	$(MAKE) -C ./tree_test

clean:
#	$(MAKE) clean -C ./egor
#	$(MAKE) clean -C ./egor2
	$(MAKE) clean -C ./cascade
#	$(MAKE) clean -C ./wav
#	$(MAKE) clean -C ./wav1
	$(MAKE) clean -C ./example1
	$(MAKE) clean -C ./neural
	$(MAKE) clean -C ./neural2
	$(MAKE) clean -C ./simple_example
	$(MAKE) clean -C ./scalarmul
	$(MAKE) clean -C ./Builder
	$(MAKE) clean -C ./tree_test

