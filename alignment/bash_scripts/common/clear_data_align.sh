#!/bin/sh

MAKE=gmake

if (($# < 1))
then 
	echo ""
	echo " The arguments are:"
	echo "         clear_data_align.sh align_name"
	echo " for example:"
	echo "         ./clear_data_align.sh ltr5"
	echo ""
	echo ""
	echo "Your command line:"
	echo "'$0 $@'"
	echo ""
	echo ""
	
	exit 1
	
fi

align_name=$1

rm -f $align_name.fasta
   
if [ -d $align_name ]
then
	cd $align_name
else
	exit 0
fi

if [ -f ../$align_name.err ]
then 
	mv ../$align_name.err .
fi

if [ -f ../$align_name.tre ]
then
	mv ../$align_name.tre .
fi

$MAKE clean 

exit 0

