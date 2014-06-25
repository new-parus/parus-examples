/*
 *  This file is a part of the PARUS project.
 *  Copyright (C) 2006  Alexey N. Salnikov, Alexander S. Ivanov
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
 * Alexander S. Ivanov (alex@angel.cs.msu.su)
 *
 */


#include <stdio.h>
#include <stdlib.h>

/**
 *
 * number of processors
 * 
 */
int num_proc;
/**
 *
 *  Number of input neurons in the percepton
 *
 */ 
int num_ent;
/**
 * 
 * This is a parameter 'num_nodes' in 'graph.grf' file. 
 * 
 */
int num_nodes;
/**
 *
 *  Number of layers in 'graph.grf' file  
 */
int num_layers;
/**
 *
 * Number of output neurons in the perceptron
 *
 */
int num_res;

/**
 *
 * (computed) a number of edges to connect a common layer with the previous 
 * layer
 *
 */
int edges_per_layer;

/**
 *
 * Define the number of the starting node
 * @return always 1
 * 
 */
int starting_node(void) {return 1;}
/**
 *
 * Return the number of the internal vertices of the perceptron.
 * @param l is a layer number and @param k is the number
 * of the node in its perceptron layer
 *
 */ 
int common_node(int l, int k) {return 2+(1+num_proc)*(l-1)+k;}
/**
 *
 * This function returns the number of the node in the 'graph.grf' file that 
 * stores the output of the percepton to the file 
 * 
 */
int final_node(void) {return 2+(num_proc+1)*num_layers;}

/**
 *
 * Return the number of the edge that conducts input data
 * from the starting node to node @param k of the first layer
 *
 */
int starting_edge(int k) {return k;}

/**
 *
 * Return the number of the edge connecting node @param i in layer @param l
 * and node @param j in the (l+1)-th layer.
 * 
 */
int common_edge(int l, int i, int j)
	{return num_proc+edges_per_layer*(l-1)+num_proc+1+(i-1)*num_proc+j-1;}

/**
 *
 * Return the number of the edge that passes the result from
 * node @param k in the last layer to the final node.
 *
 */
int final_edge(int k) {return 2*num_proc+edges_per_layer*(num_layers-1)+k;}
	
int main (int argc, char **argv)
{
    int i, j, l, q, r, s_start, s_end, r_start, r_end;
	// s_start and s_end are the first and the last-plus-one indices in the node's output data array
	// for the data send through the edge which is currently processed.
	// r_start and r_end are the same indices for the data which will be received.
	// q and r are quotient and remainder for num_ent/num_proc
	
    FILE *graph_grf, *header_c;
	// just file handles
	
	// Read the command line arguments
	if (argc!=5)
	{
		printf("\nParameters expected(#proc, #input, #layers, #output)\n");
		printf("The technical problems are causes for equivalens in inputs and outputs.\n\n");
		exit(1);
    }
	num_proc=atoi(argv[1]);
    num_ent=atoi(argv[2]);
    num_layers=atoi(argv[3]);
    num_res=atoi(argv[4]);
	edges_per_layer=num_proc*(num_proc+1);
    if (num_proc<=0) {
	  printf("The proposed system has not enough resources\n");
	  printf("to run the application\n");
	  exit(0);
    }
    num_nodes=num_layers*num_proc+2;
	
	// Write down the header of the graph
    graph_grf=fopen("graph.grf","w+");
    fprintf(graph_grf,"<GRAPH_BEGIN>\n"
                      "\theader \"header.c\"\n"
                      "\troot \"\"\n"
                      "\ttail \"\"\n"
                      "\tnum_nodes %d\n"
                      "\t<NODES_BEGIN>\n", num_nodes);
					  
    // add the starting node
    fprintf(graph_grf,"\t\t<NODE_BEGIN>\n"
                      "\t\t\tnumber 1\n"
                      "\t\t\ttype 0\n"
                      "\t\t\tweight 1\n"
                      "\t\t\tlayer 1\n"
                      "\t\t\tnum_input_edges 0\n"
                      "\t\t\tedges ( )\n"
                      "\t\t\tnum_output_edges %d\n",num_proc);
    fprintf(graph_grf,"\t\t\tedges ( ");
    for (i=1; i<=num_proc; i++)
	    fprintf(graph_grf,"%d ",starting_edge(i));
    fprintf(graph_grf," )\n"
                      "\t\t\thead \"\"\n"
                      "\t\t\tbody \"start_body.c\"\n"
                      "\t\t\ttail \"start_tail.c\"\n"
                      "\t\t<NODE_END>\n");
    // add the 1st layer
	// add the common nodes of the 1st layer
	for (i=1; i<=num_proc; i++) {
		fprintf(graph_grf,"\t\t<NODE_BEGIN>\n"
		                  "\t\t\tnumber %d\n",2+i);
		fprintf(graph_grf,"\t\t\ttype 0\n"
		                  "\t\t\tweight 1\n"
		                  "\t\t\tlayer 2\n"
	  	    	          "\t\t\tnum_input_edges 1\n"
	    	              "\t\t\tedges ( %d )\n",starting_edge(i));
		fprintf(graph_grf,"\t\t\tnum_output_edges %d\n",num_proc);
		fprintf(graph_grf,"\t\t\tedges ( ");
		for (j=1; j<=num_proc; j++)
		   fprintf(graph_grf,"%d ",common_edge(1,i,j));
		fprintf(graph_grf,")\n"
						  "\t\t\thead \"lk_head.c\"\n"
						  "\t\t\tbody \"lk_body.c\"\n"
						  "\t\t\ttail \"lk_tail.c\"\n"
						  "\t\t<NODE_END>\n"); }
	// add the common layers
	for (l=2; l<num_layers; l++) {
		// add the common nodes
		for (i=1; i<=num_proc; i++) {
			fprintf(graph_grf,"\t\t<NODE_BEGIN>\n"
			                  "\t\t\tnumber %d\n",common_node(l,i));
			fprintf(graph_grf,"\t\t\ttype 0\n"
			                  "\t\t\tweight 1\n"
			                  "\t\t\tlayer %d\n"
		  	    	          "\t\t\tnum_input_edges %d\n",l+1,num_proc);
			fprintf(graph_grf,"\t\t\tedges ( ");
			for (j=1; j<=num_proc; j++)
			   fprintf(graph_grf,"%d ",common_edge(l-1,j,i));
			fprintf(graph_grf,")\n\t\t\tnum_output_edges %d\n",num_proc);
			fprintf(graph_grf,"\t\t\tedges ( ");
			for (j=1; j<=num_proc; j++)
			   fprintf(graph_grf,"%d ",common_edge(l,i,j));
			fprintf(graph_grf,")\n"
							  "\t\t\thead \"lk_head.c\"\n"
							  "\t\t\tbody \"lk_body.c\"\n"
							  "\t\t\ttail \"lk_tail.c\"\n"
							  "\t\t<NODE_END>\n"); }
	}
	for (i=1; i<=num_proc; i++) {
		fprintf(graph_grf,"\t\t<NODE_BEGIN>\n"
		                  "\t\t\tnumber %d\n",common_node(l,i));
		fprintf(graph_grf,"\t\t\ttype 0\n"
		                  "\t\t\tweight 1\n"
		                  "\t\t\tlayer %d\n"
	  	    	          "\t\t\tnum_input_edges %d\n",l+1,num_proc);
		fprintf(graph_grf,"\t\t\tedges ( ");
		for (j=1; j<=num_proc; j++)
		   fprintf(graph_grf,"%d ",common_edge(l-1,j,i));
		fprintf(graph_grf,")\n\t\t\tnum_output_edges 1\n");
		fprintf(graph_grf,"\t\t\tedges ( %d )\n", final_edge(i));
		fprintf(graph_grf,"\t\t\thead \"lk_head.c\"\n"
						  "\t\t\tbody \"lk_body.c\"\n"
						  "\t\t\ttail \"lk_tail.c\"\n"
						  "\t\t<NODE_END>\n"); }
	//add the finishing node
	fprintf(graph_grf,"\t\t<NODE_BEGIN>\n"
	              	  "\t\t\tnumber %d\n"
					  "\t\t\ttype 0\n"
	            	  "\t\t\tweight 1\n"
	          		  "\t\t\tlayer %d\n"
	          		  "\t\t\tnum_input_edges %d\n",
			final_node(),l+2,num_proc);
  	fprintf(graph_grf,"\t\t\tedges ( ");
	for (i=1; i<=num_proc; i++)
	    fprintf(graph_grf,"%d ",final_edge(i));
	fprintf(graph_grf,")\n\t\t\tnum_output_edges 0\n");
    fprintf(graph_grf,"\t\t\tedges ( )\n"
	                  "\t\t\thead \"fin_head.c\"\n"
    	              "\t\t\tbody \"fin_body.c\"\n"
      		          "\t\t\ttail \"\"\n"
          		      "\t\t<NODE_END>\n");
	fprintf(graph_grf,"\t<NODES_END>\n"
					  "\tnum_edges %d\n"
					  "\t<EDGES_BEGIN>\n",
					  num_proc*2+(num_layers-1)*num_proc*num_proc);
	//place the edges conducting raw data
	q=num_ent/num_proc;
	r=num_ent%num_proc;
	for (i=1; i<=num_proc; i++)
	{
		r_start=0;
		r_end=num_ent;
		s_start=0;
		s_end=num_ent;
//		s_start=r_start=0; s_end=r_end=0;
		fprintf(graph_grf,"\t\t<EDGE_BEGIN>\n"
						  "\t\t\tnumber %d\n"
						  "\t\t\tweight 1\n"
						  "\t\t\ttype GRAPH_NONE\n"
						  "\t\t\tnum_var 1\n"
						  "\t\t\tnum_send_nodes 1\n"
						  "\t\t\tsend_nodes ( 1 )\n"
						  "\t\t\tnum_recv_nodes 1\n"
						  "\t\t\trecv_nodes ( %d )\n"
						  "\t\t\t<SEND_BEGIN>\n"
						  "\t\t\t\t<CHUNK_BEGIN>\n"
						  "\t\t\t\t\tname \"*net_input\"\n"
						  "\t\t\t\t\ttype GRAPH_DOUBLE\n"
						  "\t\t\t\t\tleft_offset \"%d\"\n"
						  "\t\t\t\t\tright_offset \"%d\"\n"
						  "\t\t\t\t<CHUNK_END>\n"
						  "\t\t\t<SEND_END>\n"
						  "\t\t\t<RECIEVE_BEGIN>\n"
						  "\t\t\t\t<CHUNK_BEGIN>\n"
						  "\t\t\t\t\tname \"*input\"\n"
						  "\t\t\t\t\ttype GRAPH_DOUBLE\n"
						  "\t\t\t\t\tleft_offset \"%d\"\n"
						  "\t\t\t\t\tright_offset \"%d\"\n"
						  "\t\t\t\t<CHUNK_END>\n"
						  "\t\t\t<RECIEVE_END>\n"
						  "\t\t<EDGE_END>\n",
						  starting_edge(i),common_node(1,i),
						  s_start,s_end,r_start,r_end);
	}
	//for each layer:
	for (l=1; l<=num_layers; l++)
	{
		//place the edges conducting intermediate data down - for all layers but the last
		if (l!=num_layers)
		for (j=1; j<=num_proc; j++)	// through sources
		{
			for (i=1; i<=num_proc; i++) // through destinations
			{
		  	  s_start=0;
			  if (j<=r)	r_start=(q+1)*(j-1),
						r_end=(q+1)*j,
						s_end=q+1;
				else	r_start=(q+1)*r+q*(j-r-1),
						r_end=(q+1)*r+q*(j-r),
						s_end=q;
//		s_start=r_start=0; s_end=r_end=0;
			  fprintf(graph_grf,"\t\t<EDGE_BEGIN>\n"
								"\t\t\tnumber %d\n"
							    "\t\t\tweight 1\n"
							    "\t\t\ttype GRAPH_NONE\n"
							    "\t\t\tnum_var 1\n"
							    "\t\t\tnum_send_nodes 1\n"
							    "\t\t\tsend_nodes ( %d )\n"
							    "\t\t\tnum_recv_nodes 1\n"
								"\t\t\trecv_nodes ( %d )\n"
							    "\t\t\t<SEND_BEGIN>\n"
							    "\t\t\t\t<CHUNK_BEGIN>\n"
							    "\t\t\t\t\tname \"*output\"\n"
							    "\t\t\t\t\ttype GRAPH_DOUBLE\n"
							    "\t\t\t\t\tleft_offset \"%d\"\n"
							    "\t\t\t\t\tright_offset \"%d\"\n"
							    "\t\t\t\t<CHUNK_END>\n"
							    "\t\t\t<SEND_END>\n"
							    "\t\t\t<RECIEVE_BEGIN>\n"
							    "\t\t\t\t<CHUNK_BEGIN>\n"
							    "\t\t\t\t\tname \"*input\"\n"
							    "\t\t\t\t\ttype GRAPH_DOUBLE\n"
							    "\t\t\t\t\tleft_offset \"%d\"\n"
							    "\t\t\t\t\tright_offset \"%d\"\n"
							    "\t\t\t\t<CHUNK_END>\n"
							    "\t\t\t<RECIEVE_END>\n"
							    "\t\t<EDGE_END>\n",
							    common_edge(l,j,i),
							    common_node(l,j),
								common_node(l+1,i),
							    s_start,s_end,r_start,r_end);
			}
		}
	}
	//place the edges conducting the results
	q=num_res/num_proc; r=num_res%num_proc;
	for (i=1; i<=num_proc; i++)
	{
		s_start=0;
		if (i<=r)	r_start=(q+1)*(i-1),
					r_end=(q+1)*i,
					s_end=q+1;
			else	r_start=(q+1)*r+q*(i-r-1),
					r_end=(q+1)*r+q*(i-r),
					s_end=q;
//		s_start=r_start=0; s_end=r_end=0;
		fprintf(graph_grf,"\t\t<EDGE_BEGIN>\n"
						  "\t\t\tnumber %d\n"
						  "\t\t\tweight 1\n"
						  "\t\t\ttype GRAPH_NONE\n"
						  "\t\t\tnum_var 1\n"
						  "\t\t\tnum_send_nodes 1\n"
						  "\t\t\tsend_nodes ( %d )\n"
						  "\t\t\tnum_recv_nodes 1\n"
						  "\t\t\trecv_nodes ( %d )\n"
						  "\t\t\t<SEND_BEGIN>\n"
						  "\t\t\t\t<CHUNK_BEGIN>\n"
						  "\t\t\t\t\tname \"*output\"\n"
						  "\t\t\t\t\ttype GRAPH_DOUBLE\n"
						  "\t\t\t\t\tleft_offset \"%d\"\n"
						  "\t\t\t\t\tright_offset \"%d\"\n"
						  "\t\t\t\t<CHUNK_END>\n"
						  "\t\t\t<SEND_END>\n"
						  "\t\t\t<RECIEVE_BEGIN>\n"
						  "\t\t\t\t<CHUNK_BEGIN>\n"
						  "\t\t\t\t\tname \"*result\"\n"
						  "\t\t\t\t\ttype GRAPH_DOUBLE\n"
						  "\t\t\t\t\tleft_offset \"%d\"\n"
						  "\t\t\t\t\tright_offset \"%d\"\n"
						  "\t\t\t\t<CHUNK_END>\n"
						  "\t\t\t<RECIEVE_END>\n"
						  "\t\t<EDGE_END>\n",
						  final_edge(i),
						  common_node(num_layers,i),
						  final_node(),
						  s_start,s_end,r_start,r_end);
	}
	fprintf(graph_grf,"\t<EDGES_END>\n"
					  "<GRAPH_END>\n");
    fclose(graph_grf);
	
	// Write the header file for the multiprocessor application
	header_c=fopen("header.c","w+");
	fprintf(header_c,"#include <stdio.h>\n"
					 "#include <fcntl.h>\n"
					 "#include <unistd.h>\n"
					 "#include <sys/stat.h>\n"
					 "#include <sys/types.h>\n"
					 "#include <string.h>\n\n"
					 "#define num_ent %d\n"
					 "#define num_proc %d\n"
					 "#define num_layers %d\n"
					 "#define num_res %d\n",
				 num_ent, num_proc, num_layers, num_res);
		fprintf(header_c, "#define get_l ((px_node+1)/(num_proc+1))\n");
		fprintf(header_c, "#define get_i ((px_node-3)%%(num_proc+1)+1)\n");
		fprintf(header_c, "#define get_q (num_ent/num_proc)\n");
		fprintf(header_c, "#define get_r (num_ent%%num_proc)\n");
		fprintf(header_c, "#define get_wt_start ((get_l-1)*num_ent*num_ent+((get_i-1<get_r)?(get_q+1)*(get_i-1)*num_proc:");
		fprintf(header_c, "((get_q+1)*get_r+get_q*(get_i-get_r-1))*num_ent))\n");
		fprintf(header_c, "#define get_width ((get_i-1<get_r)?get_q+1:get_q)\n");
	fclose(header_c);
	return 0;
}

// vim:set ts=4 sw=4 nowrap:
