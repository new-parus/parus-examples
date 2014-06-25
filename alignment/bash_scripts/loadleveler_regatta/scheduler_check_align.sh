#!/bin/sh

if (($# != 1))
then 
	echo ""
	echo " The arguments are:"
	echo "         scheduler_check_align.sh align_name"
	echo " for example:"
	echo "         ./make_align.sh ltr5"
	echo ""
	echo ""
	echo "Your command line:"
	echo "'$0 $@'"
	echo ""
	echo ""
	
	exit 1
	
fi

align_name=$1

cd $align_name

if ! [ -f job_id.txt ]
then
	echo "aborted"
	exit 3
fi

job_id=`cat job_id.txt`

finished=`llq | grep $job_id`

if [ "$finished" != "" ]
then
	echo "not finished"
	exit 2
fi

err_file=`ls | grep ".${job_id}.err"`
if [ "$err_file" == "" ]
then
	echo "finished"
	exit 0
fi

#if [ -f data_node_1.fasta  ]
#then 
#        mv data_node_1.fasta result.fasta
#	echo "finished"
#	exit 0
#fi

#if [ -f result.fasta ]
#then
#    echo "finished"
#    exit 0
#fi


#echo "aborted"
#exit 3

