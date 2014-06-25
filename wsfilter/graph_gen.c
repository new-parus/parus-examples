/*
 *  This file is a part of the PARUS project.
 *  Copyright (C) 2006  Alexey N. Salnikov
 *  
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * Alexey N. Salnikov (salnikov@cmc.msu.ru)
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <parus_config.h>

#define F_LEN 2048

#define PATH "/examples/wsfilter"


int main(int argc,char **argv)
{

	int status;
	int num_procs;
	int node_weight;
	int edge_weight;

	FILE *f=NULL;

	int i;
	int node_number=0;
	
	if(argc<4)
	{
		printf("\n\nParameters are: %s <num parallel processes> <weight per node> <weigth per edge>\n\n",argv[0]);
		return 0;
	}

	num_procs=atoi(argv[1]);
	node_weight=atoi(argv[2]);
	edge_weight=atoi(argv[3]);

	f=fopen("graph.grf","w");
	if(f==NULL)
	{
		printf("Can't create file \"graph.grf\"\n");
		return -1;
	}

	fprintf(f,"<GRAPH_BEGIN>\n");
	fprintf(f,"  header \"%s/examples/wsfilter/templates/header.cpp\"\n", PARUS_INSTALL_DIR);
	fprintf(f,"  root   \"%s/examples/wsfilter/templates/root.cpp\"\n",   PARUS_INSTALL_DIR);
	fprintf(f,"  tail   \"%s/examples/wsfilter/templates/tail.cpp\"\n",   PARUS_INSTALL_DIR);
	fprintf(f,"  num_nodes %d\n",num_procs*2+2);
	
	fprintf(f,"\n<NODES_BEGIN>\n");
	
	/*
	 * Now we write first layer of graph.
	 * This layer reads input of netcdf file
	 * process it (call wsfilter::process2 function)
	 * and prepare results for send them to nodes on second layer.
	 * 
	 */
	for(i=0;i<num_procs-1;i++)
	{
		fprintf(f,"\n <NODE_BEGIN>\n");
		fprintf(f,"   number %d\n",i);
		fprintf(f,"   type 0\n");
		fprintf(f,"   weight %d\n",node_weight);
		fprintf(f,"   layer 1\n");
		fprintf(f,"   num_input_edges 0\n");
		fprintf(f,"   edges ( )\n");
		fprintf(f,"   num_output_edges 2\n");
		fprintf(f,"   edges ( %d  %d )\n",i,i+num_procs);
		fprintf(f,"   head \"%s/examples/wsfilter/templates/node_header.cpp\"\n",PARUS_INSTALL_DIR);
		fprintf(f,"   body \"%s/examples/wsfilter/templates/node_body.cpp\"\n",  PARUS_INSTALL_DIR);
		fprintf(f,"   tail \"%s/examples/wsfilter/templates/node_tail.cpp\"\n",  PARUS_INSTALL_DIR);
		fprintf(f," <NODE_END>\n");
	}

	fprintf(f,"\n <NODE_BEGIN>\n");
	fprintf(f,"   number %d\n",i);
	fprintf(f,"   type 0\n");
	fprintf(f,"   weight %d\n",node_weight);
	fprintf(f,"   layer 1\n");
	fprintf(f,"   num_input_edges 0\n");
	fprintf(f,"   edges ( )\n");
	fprintf(f,"   num_output_edges 1\n");
	fprintf(f,"   edges ( %d )\n",i);
	fprintf(f,"   head \"%s/examples/wsfilter/templates/node_header_last.cpp\"\n",PARUS_INSTALL_DIR);
	fprintf(f,"   body \"%s/examples/wsfilter/templates/node_body.cpp\"\n",  PARUS_INSTALL_DIR);
	fprintf(f,"   tail \"%s/examples/wsfilter/templates/node_tail.cpp\"\n",  PARUS_INSTALL_DIR);
	fprintf(f," <NODE_END>\n");

	/*
	 * Now we write second layer of graph.
	 * This layer reads data from first layer and 
	 * data tail from first layer.
	 *
	 * Then node adds tail to data begin and converts it to
	 * short. Then node writes it to NetCDF file. 
	 * 
	 */
	fprintf(f,"\n <NODE_BEGIN>\n");
	fprintf(f,"   number %d\n",num_procs);
	fprintf(f,"   type 1\n");
	fprintf(f,"   weight %d\n",node_weight);
	fprintf(f,"   layer 2\n");
	fprintf(f,"   num_input_edges 2\n");
	fprintf(f,"   edges ( %d %d )\n",0,num_procs*2);
	fprintf(f,"   num_output_edges 0\n");
	fprintf(f,"   edges ( )\n");
	fprintf(f,"   head \"%s/examples/wsfilter/templates/out_node_header.cpp\"\n",PARUS_INSTALL_DIR);
	fprintf(f,"   body \"%s/examples/wsfilter/templates/out_node_body.cpp\"\n",  PARUS_INSTALL_DIR);
	fprintf(f,"   tail \"%s/examples/wsfilter/templates/out_node_tail.cpp\"\n",  PARUS_INSTALL_DIR);
	fprintf(f," <NODE_END>\n");

	
	for(i=1;i<num_procs-1;i++)
	{
		fprintf(f,"\n <NODE_BEGIN>\n");
		fprintf(f,"   number %d\n",num_procs+i);
		fprintf(f,"   type 1\n");
		fprintf(f,"   weight %d\n",node_weight);
		fprintf(f,"   layer 2\n");
		fprintf(f,"   num_input_edges 3\n");
		fprintf(f,"   edges ( %d %d %d )\n",i,num_procs+i-1,num_procs*2+i);
		fprintf(f,"   num_output_edges 0\n");
		fprintf(f,"   edges ( )\n");
		fprintf(f,"   head \"%s/examples/wsfilter/templates/out_node_header.cpp\"\n",PARUS_INSTALL_DIR);
		fprintf(f,"   body \"%s/examples/wsfilter/templates/out_node_body.cpp\"\n",  PARUS_INSTALL_DIR);
		fprintf(f,"   tail \"%s/examples/wsfilter/templates/out_node_tail.cpp\"\n",  PARUS_INSTALL_DIR);
		fprintf(f," <NODE_END>\n");
	}

	fprintf(f,"\n <NODE_BEGIN>\n");
	fprintf(f,"   number %d\n",num_procs+i);
	fprintf(f,"   type 1\n");
	fprintf(f,"   weight %d\n",node_weight);
	fprintf(f,"   layer 2\n");
	fprintf(f,"   num_input_edges 3\n");
	fprintf(f,"   edges ( %d %d %d )\n",i,num_procs+i-1,num_procs*2+i);
	fprintf(f,"   num_output_edges 0\n");
	fprintf(f,"   edges ( )\n");
	fprintf(f,"   head \"%s/examples/wsfilter/templates/out_node_header_last.cpp\"\n",PARUS_INSTALL_DIR);
	fprintf(f,"   body \"%s/examples/wsfilter/templates/out_node_body.cpp\"\n",       PARUS_INSTALL_DIR);
	fprintf(f,"   tail \"%s/examples/wsfilter/templates/out_node_tail.cpp\"\n",       PARUS_INSTALL_DIR);
	fprintf(f," <NODE_END>\n");

	/*
	 * Node for processing short files
	 */
	fprintf(f,"\n <NODE_BEGIN>\n");
	fprintf(f,"   number %d\n",2*num_procs);
	fprintf(f,"   type 1\n");
	fprintf(f,"   weight %d\n",node_weight);
	fprintf(f,"   layer 2\n");
	fprintf(f,"   num_input_edges 1\n");
	fprintf(f,"   edges ( %d )\n",num_procs*3);
	fprintf(f,"   num_output_edges 0\n");
	fprintf(f,"   edges ( )\n");
	fprintf(f,"   head \"%s%s/templates/short_data_netcdf_header.cpp\"\n",PARUS_INSTALL_DIR,PATH); 
	fprintf(f,"   body \"%s%s/templates/short_data_netcdf_node.cpp\"\n",  PARUS_INSTALL_DIR,PATH);
	fprintf(f,"   tail \"%s%s/templates/short_data_netcdf_tail.cpp\"\n",  PARUS_INSTALL_DIR,PATH);
	fprintf(f," <NODE_END>\n");
		
	/*
	 * Node creating NetCDF file	 
	 */
	fprintf(f,"\n <NODE_BEGIN>\n");
	fprintf(f,"   number %d\n",2*num_procs+1);
	fprintf(f,"   type 1\n");
	fprintf(f,"   weight %d\n",node_weight);
	fprintf(f,"   layer 1\n");
	fprintf(f,"   num_input_edges 0\n");
	fprintf(f,"   edges ( )\n");
	fprintf(f,"   num_output_edges %d\n",num_procs+1);
	fprintf(f,"   edges ( ");
	for(i=0;i<num_procs+1;i++)  fprintf(f," %d",i+num_procs*2);
	fprintf(f,"	)\n");
	fprintf(f,"   head \"\"\n");
	fprintf(f,"   body \"%s/examples/wsfilter/templates/create_netcdf_node.cpp\"\n",PARUS_INSTALL_DIR);
	fprintf(f,"   tail \"\"\n");
	fprintf(f," <NODE_END>\n");

	
	fprintf(f,"\n<NODES_END>\n");
	
	fprintf(f,"  num_edges %d\n",num_procs*3);

	fprintf(f,"\n<EDGES_BEGIN>\n");
	
	for(i=0;i<num_procs-1;i++)
	{
		fprintf(f,"\n <EDGE_BEGIN>\n");
		fprintf(f,"   number %d\n",i);
		fprintf(f,"   weight %d\n",edge_weight);
		fprintf(f,"   type   GRAPH_NONE\n");
		fprintf(f,"   num_var 1\n");
		fprintf(f,"   num_send_nodes 1\n");
		fprintf(f,"   send_nodes ( %d )\n",i);
		fprintf(f,"   num_recv_nodes 1\n");
		fprintf(f,"   recv_nodes ( %d )\n",i+num_procs);
		fprintf(f,"   <SEND_BEGIN>\n");
		fprintf(f,"\n    <CHUNK_BEGIN>\n");
		fprintf(f,"       name \"*data_out\"\n");
		fprintf(f,"       type GRAPH_DOUBLE\n");
		fprintf(f,"       left_offset \"0\"\n");
		fprintf(f,"       right_offset \"window_size\"\n");
		fprintf(f,"    <CHUNK_END>\n");
		fprintf(f,"   <SEND_END>\n");
		fprintf(f,"   <RECIEVE_BEGIN>\n");
		fprintf(f,"\n    <CHUNK_BEGIN>\n");
		fprintf(f,"       name \"*data_out\"\n");
		fprintf(f,"       type GRAPH_DOUBLE\n");
		fprintf(f,"       left_offset \"0\"\n");
		fprintf(f,"       right_offset \"window_size\"\n");
		fprintf(f,"    <CHUNK_END>\n");
		fprintf(f,"   <RECIEVE_END>\n");
		fprintf(f," <EDGE_END>\n");
	}

	fprintf(f,"\n <EDGE_BEGIN>\n");
	fprintf(f,"   number %d\n",i);
	fprintf(f,"   weight %d\n",edge_weight);
	fprintf(f,"   type   GRAPH_NONE\n");
	fprintf(f,"   num_var 1\n");
	fprintf(f,"   num_send_nodes 1\n");
	fprintf(f,"   send_nodes ( %d )\n",i);
	fprintf(f,"   num_recv_nodes 1\n");
	fprintf(f,"   recv_nodes ( %d )\n",i+num_procs);
	fprintf(f,"   <SEND_BEGIN>\n");
	fprintf(f,"\n    <CHUNK_BEGIN>\n");
	fprintf(f,"       name \"*data_out\"\n");
	fprintf(f,"       type GRAPH_DOUBLE\n");
	fprintf(f,"       left_offset \"0\"\n");
	fprintf(f,"       right_offset \"window_size+F_LEN\"\n");
	fprintf(f,"    <CHUNK_END>\n");
	fprintf(f,"   <SEND_END>\n");
	fprintf(f,"   <RECIEVE_BEGIN>\n");
	fprintf(f,"\n    <CHUNK_BEGIN>\n");
	fprintf(f,"       name \"*data_out\"\n");
	fprintf(f,"       type GRAPH_DOUBLE\n");
	fprintf(f,"       left_offset \"0\"\n");
	fprintf(f,"       right_offset \"window_size\"\n");
	fprintf(f,"    <CHUNK_END>\n");
	fprintf(f,"   <RECIEVE_END>\n");
	fprintf(f," <EDGE_END>\n");

	for(i=0;i<num_procs-1;i++)
	{
		fprintf(f,"\n <EDGE_BEGIN>\n");
		fprintf(f,"   number %d\n",i+num_procs);
		fprintf(f,"   weight %d\n",F_LEN*sizeof(double));
		fprintf(f,"   type   GRAPH_NONE\n");
		fprintf(f,"   num_var 1\n");
		fprintf(f,"   num_send_nodes 1\n");
		fprintf(f,"   send_nodes ( %d )\n",i);
		fprintf(f,"   num_recv_nodes 1\n");
		fprintf(f,"   recv_nodes ( %d )\n",i+1+num_procs);
		fprintf(f,"   <SEND_BEGIN>\n");
		fprintf(f,"\n    <CHUNK_BEGIN>\n");
		fprintf(f,"       name \"*data_out\"\n");
		fprintf(f,"       type GRAPH_DOUBLE\n");
		fprintf(f,"       left_offset \"window_size\"\n");
		fprintf(f,"       right_offset \"window_size+F_LEN\"\n");
		fprintf(f,"    <CHUNK_END>\n");
		fprintf(f,"   <SEND_END>\n");
		fprintf(f,"   <RECIEVE_BEGIN>\n");
		fprintf(f,"\n    <CHUNK_BEGIN>\n");
		fprintf(f,"       name \"*tail\"\n");
		fprintf(f,"       type GRAPH_DOUBLE\n");
		fprintf(f,"       left_offset \"0\"\n");
		fprintf(f,"       right_offset \"F_LEN\"\n");
		fprintf(f,"    <CHUNK_END>\n");
		fprintf(f,"   <RECIEVE_END>\n");
		fprintf(f," <EDGE_END>\n");
	}
	

	for(i=0;i<num_procs+1;i++)
	{
		fprintf(f,"\n <EDGE_BEGIN>\n");
		fprintf(f,"   number %d\n",i+2*num_procs);
		fprintf(f,"   weight %d\n",1);
		fprintf(f,"   type   GRAPH_NONE\n");
		fprintf(f,"   num_var 1\n");
		fprintf(f,"   num_send_nodes 1\n");
		fprintf(f,"   send_nodes ( %d )\n",2*num_procs+1);
		fprintf(f,"   num_recv_nodes 1\n");
		fprintf(f,"   recv_nodes ( %d )\n",i+num_procs);
		fprintf(f,"   <SEND_BEGIN>\n");
		fprintf(f,"\n    <CHUNK_BEGIN>\n");
		fprintf(f,"       name \"fictive_var\"\n");
		fprintf(f,"       type GRAPH_CHAR\n");
		fprintf(f,"       left_offset \"0\"\n");
		fprintf(f,"       right_offset \"1\"\n");
		fprintf(f,"    <CHUNK_END>\n");
		fprintf(f,"   <SEND_END>\n");
		fprintf(f,"   <RECIEVE_BEGIN>\n");
		fprintf(f,"\n    <CHUNK_BEGIN>\n");
		fprintf(f,"       name \"fictive_var\"\n");
		fprintf(f,"       type GRAPH_CHAR\n");
		fprintf(f,"       left_offset \"0\"\n");
		fprintf(f,"       right_offset \"1\"\n");
		fprintf(f,"    <CHUNK_END>\n");
		fprintf(f,"   <RECIEVE_END>\n");
		fprintf(f," <EDGE_END>\n");
	}
	
	fprintf(f,"\n<EDGES_END>\n");
	
	fprintf(f,"\n<GRAPH_END>\n\n");
	
	
	fclose(f);

	f=fopen("wsf_procs.h","w");
	if(f==NULL)
	{
		printf("Can't create file \"wsf_procs.h\"\n");
		return -1;
	}

	fprintf(f,"#define NUM_PROCS %d\n\n",num_procs);
	
	fclose(f);
	
	f=fopen("Makefile","w");
	if(f==NULL)
	{
		printf("Can't create file \"Makefile\"\n");
		return -1;
	}
	
	fprintf(f,"include %s/config\n\n",PARUS_INSTALL_DIR);
	fprintf(f,"EXECS= parallel_wsfilter\n\n");
	fprintf(f,"OBJS= %s%s/aflib/aflibFFT.o\\\n",PARUS_INSTALL_DIR,PATH);
	fprintf(f,"      %s%s/filter/lspwsfilter.o\\\n",PARUS_INSTALL_DIR,PATH);
	fprintf(f,"      %s%s/libsystem/module_header.o\n\n",PARUS_INSTALL_DIR,PATH);
	fprintf
	(
		f,
		"LOCAL_INCLUDE=\\\n     -I %s%s/aflib\\\n    -I %s%s/filter\\\n   -I %s%s/libsystem\n\n\n",
		PARUS_INSTALL_DIR,
		PATH,
		PARUS_INSTALL_DIR,
		PATH,
		PARUS_INSTALL_DIR,
		PATH
	);
	
	fprintf(f,"all: filter.cpp $(EXECS)\n\n");
	fprintf(f,"filter.cpp:\n");
	fprintf(f,"\t%s/bin/graph2c++ -p filter\n",PARUS_INSTALL_DIR);
	fprintf(f,"\trm -f ./Makefile.new\n\n");

	fprintf(f,"parallel_wsfilter: filter.o $(OBJS)\n");
	fprintf(f,"\t$(MPI_CCLINKER) $(MPI_CCLINKER_FLAGS) $(MPI_LIB_PATH) -L $(INSTALL_DIR)/lib filter.o $(OBJS) -o parallel_wsfilter $(MPI_LIBS) -lm -lparus -lnetcdf\n\n");
	fprintf(f,"clean:\n\t/bin/rm -f *.o $(EXECS) $(EXECS)_*\n\n");
	fprintf(f,".c.o:\n\t$(MPI_cc) $(MPI_cc_FLAGS) $(MPI_cc_INCLUDE) $(LOCAL_INCLUDE) -c $*.c -o $*.o\n\n");
	fprintf(f,".cpp.o:\n\t$(MPI_CC) $(MPI_CC_FLAGS) $(MPI_CC_INCLUDE) $(LOCAL_INCLUDE) -c $*.cpp -o $*.o\n\n");
	
	fclose(f);
	
	return 0;
}
