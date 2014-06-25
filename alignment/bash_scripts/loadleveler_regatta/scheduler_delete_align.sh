#!/bin/sh

if (($# != 1))
then 
	echo ""
	echo " The arguments are:"
	echo "         scheduler_check_align.sh align_name"
	echo " for example:"
	echo "         ./scheduler_delete_align.sh ltr5"
	echo ""
	echo ""
	echo "Your command line:"
	echo "'$0 $@'"
	echo ""
	echo ""
	
	exit 1
	
fi

align_name=$1

rm -f $align_name.err $align_name.fasta $align_name.tre

cd $align_name

if [ -f job_id.txt ]
then
	job_id=`cat job_id.txt`
	finished=`llq | grep $job_id`
fi

if [ "$finished" != "" ]
then
	tasks -d $job_id
fi

cd ..
rm -rf $align_name

