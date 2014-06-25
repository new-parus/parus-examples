#!/bin/bash
rm task
rm graph.grf
rm prog.cpp
rm Makefile
rm prog
make gen_graph ||
{	echo "task.c contains errors";
	exit 1; }
if !(./gen_graph $1 $2 $3 $4); then
	exit 1
fi
if !(graph2c++); then
	echo "the graph contains errors"
	exit 1
fi
make

