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
#      make_align.sh align_name num_processors data.fasta minutes_to_work   #
# for example:                                                              #
#      ./make_align.sh ltr5 4 seq.fasta 3600                                #
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
# 5. Then we submit a programm on cluster                                   #
#    Number of arguments read from command line arguments.                  #
#     mpisubmit -np $num_procs -w $time_label ./prog                        #
#                                                                           #
# 6. Move results to up catalogue and delete working catalogue.             #
#                                                                           #
#############################################################################

MAKE=make
PARUS_HOME=${HOME}/parus
MUSCLE_PATH=${HOME}/bin/muscle
SUBMIT_COMMAND=mpisubmit
CLUSTALW_PATH=`which clustalw-mpi`


function err()
{
    echo ERROR > $align_name.err 
    exit -1
}

if (($# < 5))
then 
	echo ""
	echo ""
	echo " Makes an alignment by .fasta file that contains"
	echo " nucleotide or amino asid sequences."
	echo ""
	echo " The arguments are:"
	echo "         make_align.sh align_name num_processors data.fasta minutes_to_work algorithm [ some_unique_number ]"
	echo " for example:"
	echo "         ./schedule_make_align.sh ltr5 4 seq.fasta 3600 muscle 1"
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
minutes_to_work=$4;
algorithm=$5;
number=0;

if (( $# > 5 ))
then 
	number=$6;
fi

echo ""
echo "Align name: $align_name"
echo "Number of processors: $num_procs"
echo "Fasta file name: '$fasta_file'"
echo "Algorithm: $algorithm"
echo "Result files modifier: $number"
echo ""

if !  mkdir $align_name
then
	err
fi

cd ./$align_name

sed -e 's/[:,]/_/g' < ../$fasta_file  | sed -e 's/(/\{/g' | sed -e 's/)/\}/g' > $fasta_file 

rm -f sbm.txt

if [ "$algorithm" = "clustalw" ]
then
	if ! $SUBMIT_COMMAND -n $num_procs -w 00:$minutes_to_work:00 $CLUSTALW_PATH -- -infile=$fasta_file -outfile=result$number.fasta > sbm.txt
	then
		echo "Submit to queue failed!"
		err
	fi
fi

if [ "$algorithm" = "muscle" ]
then 


	if (($num_procs == 1))
	then
	
		if ! $SUBMIT_COMMAND -n 1 -w 00:$minutes_to_work:00 $MUSCLE_PATH -- -in $fasta_file -out result$number.fasta -maxhours $((minutes_to_work/60)) > sbm.txt
		then
			echo "Submit to queue failed!"
			err
		fi


	else
	
		if ! $MUSCLE_PATH -in $fasta_file -tree1 $align_name.tre -maxiters 1 -cluster1 upgmb > /dev/null
		then
			echo "Tree building failure!"
			err
		fi

		if ! ../tree_to_program $fasta_file $align_name.tre  0 $MUSCLE_PATH
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

		if ! $SUBMIT_COMMAND -n $num_procs -w 00:$minutes_to_work:00 ./prog > sbm.txt
		then
			echo "Submit to queue failed!"
			err
		fi
	fi # end multiple procs

fi # end muscle

#
# skif 
#

#job_id=`grep "ID" sbm.txt | cut -f 6  -d " " | cut -f 2 -d "=" | cut -f 1 -d ")" `

job_id=`grep "llsubmit" sbm.txt | awk '{print $4 }' | tr -d '"' `

echo $job_id

rm -f job_id.txt

if [ "$job_id" = "" ]
then
	echo "The job has not been submitted"
	err
fi

echo $job_id  > job_id.txt


exit 0

