#!/bin/bash --login

#
# This file is a part of the PARUS project and  makes the core of the parus system
# Copyright (C) 2006  Alexey N. Salnikov
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
# Alexey N. Salnikov (salnikov@cmc.msu.ru)
#

#############################################################################
#                                                                           #
#                                                                           #
# Makes an alignment by the .fasta file that contains                       #
# nucleotide or amino asid sequences.                                       #
#                                                                           #
# Arguments are:                                                            #
#         make_align.sh align_name num_processors data.fasta                #
# for example:                                                              #
#         ./make_align.sh ltr5 4 seq.fasta                                  #
#                                                                           #
#                                                                           #
# 1. This stage we create a catalogue $align_name                           #
#    (catalogue name are reading  as a command line  parameter).            #
#    Then we change directory and:                                          #
#                                                                           #
# 2. We run 'muscle' program with arguments:                                #
#    muscle -in file.fasta -tree1 tree.phy -maxiters 1 -cluster             #
#    after this command we recieve tree.phy file                            #
#                                                                           #
# 3. Split file.fasta to many files and genereating graph.grf file.         #
#    we call 'tree_to_program' with arguments:                              #
#    tree_to_program file.fasta tree.phy 0 muscle                           #
#                                                                           #
# 4. Then we generating a programm by graph.grf file                        #
#         we call graph2c++ for this purpose.                               #
#                                                                           #
# 5. Then we run a programm through MPI by calling mpirun programm          #
#    Number of arguments read from command line arguments.                  #
#     mpirun -np $num_procs ./prog                                          #
#                                                                           #
# 6. Move results to up catalogue and delete working catalogue.             #
#                                                                           #
#############################################################################

MAKE=make
MPI_HOME=/usr/local/OpenMPI
PARUS_HOME=${HOME}/parus-1.2.9_OpenMPI
MUSCLE_PATH=`which muscle`

function err()
{
    echo ERROR > $align_name.err 
    exit -1
}

if (($# < 3 ))
then 
	echo ""
	echo ""
	echo " Makes an alignment by .fasta file that contains"
	echo " nucleotide or amino asid sequences."
	echo ""
	echo " The arguments are:"
	echo "         make_align.sh align_name num_processors data.fasta"
	echo " for example:"
	echo "         ./make_align.sh ltr5 4 seq.fasta"
	echo ""
	echo ""
	echo "Your command line:"
	echo "'$0 $@'"
	echo ""
	echo ""
	
	exit 1
	
fi

align_name=$1;
num_procs=$2;
fasta_file=$3;

echo ""
echo "Align name: '$align_name'"
echo "Number of processors: $num_procs"
echo "Fasta file name: '$fasta_file'"
echo ""

mkdir ./$align_name
cd ./$align_name

if ! $MUSCLE_PATH -in ../$fasta_file -tree1 ../$align_name.tre -maxiters 1 -cluster
then
    echo "Tree building failure!"
    err
fi

if ! ../tree_to_program ../$fasta_file ../$align_name.tre  0 $MUSCLE_PATH
then
    echo "Graph generating failure!"
    err
fi

if ! ${PARUS_HOME}/bin/graph2c++
then
    echo "C++ program generating failure!"
    err
fi

if ! $MAKE
then
    echo "Make failed"
    err 
fi

rm ./*.cpp

#lamboot
if ! ${MPI_HOME}/bin/mpirun -np $num_procs ./prog
then
    echo "Parallel program failure!"
    err
fi
#lamhalt

cp data_node_1.fasta ../$align_name.fasta
cp times.txt ../${align_name}.times.txt

cd .. 
rm -rf ./$align_name

exit 0
