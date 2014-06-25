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

#include <map>

#include <stdio.h>
#include <stdlib.h>

#include "programm_graph.h"

using namespace std;

class Edge
{
public:
	int from;
	int to;
};

int write_node
(
	FILE *f,
	parus::Tree *node,
	int parent_node_number,
	int number_output_edge,
	map<int,Edge> &edges_list,
	Fasta &fasta,
	const char *path_to_muscle
);

int edge_number=0;

/**************************************************************************/
int Programm_graph::write(const char *path_to_muscle)
{
	FILE *f=NULL;

	Edge edge;

	map<int,Edge> edges_set;

	int num_edges;
	int num_nodes;


	int flag;
	int i;
	
	num_nodes=tree.get_num_nodes();

	f=fopen("header_graph.cpp","w");
	if(f==NULL)
	{
		printf
		(
			"Programm_graph::write(const char *path_to_muscle=\"%s\") can't create file header_graph.cpp \n",
			path_to_muscle
		);
		return -1;
	}

	fprintf(f,"#include <stdlib.h>\n\nchar fictitious_var;\nchar command_string[2048];\n\n");
	fprintf(f,"class Pair_values\n{\n public:\n\tint pred_1;\n\tint pred_2;\n\tvoid set_values(int val1,int val2);\n };\n\n");
	fprintf(f,"Pair_values pairs[%d];\n\n",num_nodes);
	fprintf(f,"void Pair_values::set_values(int val1,int val2)\n{\n\tpred_1=val1;\n\tpred_2=val2;\n}\n\n");
	fclose(f);
	
	f=fopen("graph.grf","w");
	
	if(f==NULL)
	{
		printf
		(
			"Programm_graph::write(const char *path_to_muscle=\"%s\") can't create file graph.grf \n",
			path_to_muscle
		);
		return -1;
	}

	fprintf(f,"<GRAPH_BEGIN>\n");
	fprintf(f," header \"header_graph.cpp\"\n");
	fprintf(f," root \"root_graph_program.cpp\"\n");
	fprintf(f," tail \"\"\n");
	fprintf(f," num_nodes %d\n",num_nodes);
	fprintf(f,"\n <NODES_BEGIN>\n");
	flag=write_node(f,tree.get_root(),0,0,edges_set,fasta,path_to_muscle);
	if(flag)
	{
		printf("Nodes can't be written to file system\n");
		return -1;
	}
	fprintf(f," <NODES_END>\n\n");
	num_edges=edges_set.size();
	fprintf(f,"  num_edges %d\n\n",num_edges);
	fprintf(f," <EDGES_BEGIN>\n");
	for(i=1;i<=num_edges;i++)
	{
		edge=edges_set[i];
		fprintf(f,"  <EDGE_BEGIN>\n");
		fprintf(f,"   number %d\n",i);
		fprintf(f,"   weight 1\n");
		fprintf(f,"   type GRAPH_NONE\n");
		fprintf(f,"   num_var 1\n");
		fprintf(f,"   num_send_nodes 1\n");
		fprintf(f,"   send_nodes ( %d )\n",edge.from);
		fprintf(f,"   num_recv_nodes 1\n");
		fprintf(f,"   recv_nodes ( %d )\n",edge.to);
		fprintf(f,"   <SEND_BEGIN>\n\n");
		fprintf(f,"    <CHUNK_BEGIN>\n");
		fprintf(f,"      name \"fictitious_var\"\n");
		fprintf(f,"      type GRAPH_CHAR\n");
		fprintf(f,"      left_offset  \"0\"\n");
		fprintf(f,"      right_offset \"1\"\n");
		fprintf(f,"    <CHUNK_END>\n\n");
		fprintf(f,"   <SEND_END>\n");
		fprintf(f,"   <RECIEVE_BEGIN>\n\n");
		fprintf(f,"    <CHUNK_BEGIN>\n");
		fprintf(f,"      name \"fictitious_var\"\n");
		fprintf(f,"      type GRAPH_CHAR\n");
		fprintf(f,"      left_offset  \"0\"\n");
		fprintf(f,"      right_offset \"1\"\n");
		fprintf(f,"    <CHUNK_END>\n");
		fprintf(f,"   <RECIEVE_END>\n");
		fprintf(f,"  <EDGE_END>\n\n");
	}
	fprintf(f," <EDGES_END>\n");
	fprintf(f,"<GRAPH_END>\n");

	fclose(f);	

	f=fopen("process_pair.cpp","w");
	if(f==NULL)
	{
		printf
		(
			"Programm_graph::write(const char *path_to_muscle=\"%s\") can't create file process_pair.cpp \n",
			path_to_muscle
		);
		return -1;
	}
	
	fprintf
	(
		f,
		"sprintf(command_string,\"%s -profile -in1 data_node_%%d.fasta -in2 data_node_%%d.fasta -out data_node_%%d.fasta\",pairs[px_node-1].pred_1,pairs[px_node-1].pred_2,px_node);\n",
		path_to_muscle
	);
	fprintf(f,"int flag=system(command_string);\n");
	fprintf(f,"if(flag) return -1;");
	
	fprintf
	(
		f,
		"sprintf(command_string,\"rm -f data_node_%%d.fasta data_node_%%d.fasta\",pairs[px_node-1].pred_1,pairs[px_node-1].pred_2);\n"
	);

	fprintf(f,"flag=system(command_string);\n");
	fprintf(f,"if(flag) return -1;");
	fclose(f);
	
	f=fopen("generate_profile.cpp","w");
	if(f==NULL)
	{
		printf
		(
			"Programm_graph::write(const char *path_to_muscle=\"%s\") can't create file process_pair.cpp \n",
			path_to_muscle
		);
		return -1;
	}
	
	fprintf
	(
		f,
		"sprintf(command_string,\"%s -in fasta_node_%%d.fasta -out data_node_%%d.fasta\",px_node,px_node);\n",
		path_to_muscle
	);
	fprintf(f,"int flag=system(command_string);\n");
	fprintf(f,"if(flag) return -1;");
	fprintf
	(
		f,
		"sprintf(command_string,\"rm -f fasta_node_%%d.fasta\",px_node);\n"
	);
	fprintf(f,"flag=system(command_string);\n");
	fprintf(f,"if(flag) return -1;");
fclose(f);

	return 0;
}
/*************************************************************************/
int write_node
(
	FILE *f,
	parus::Tree *node,
	int parent_node_number,
	int number_output_edge,
	map<int,Edge> &edges_list,
	Fasta &fasta,
	const char *path_to_muscle
)
{
	Edge edge;
	int flag=0;
	int num_input_edges=0;

	int left_edge_number=0;
	int right_edge_number=0;

	char str[100];
	int i;

	FILE *file_node_body=NULL;

	Fasta node_fasta;
	Sequence_record record;


	if(node==NULL) return 0;

	fprintf(f,"  <NODE_BEGIN>\n");
	fprintf(f,"   number %d\n",node->number);
	fprintf(f,"   type 0\n");
	fprintf(f,"   weight 1000\n");
	fprintf(f,"   layer %d\n",node->antilayer);
	
	if(node->left!=NULL)
	{
		num_input_edges++;
	}

	if(node->right!=NULL)
	{
		num_input_edges++;
	}
	fprintf(f,"   num_input_edges %d\n",num_input_edges);
	
	fprintf(f,"   edges ( ");
	if(node->left!=NULL)
	{
		edge_number++;
		left_edge_number=edge_number;
		fprintf(f," %d ",edge_number);
	}
	
	if(node->right!=NULL)
	{
		edge_number++;
		right_edge_number=edge_number;
		fprintf(f," %d ",edge_number);
	}
	fprintf(f,")\n");


	if(number_output_edge==0)
	{
		fprintf(f,"   num_output_edges 0\n");
		fprintf(f,"   edges ( )\n");
	}
	else
	{
		fprintf(f,"   num_output_edges 1\n");
		fprintf(f,"   edges ( %d )\n",number_output_edge);
		
		edge.to=parent_node_number;
		edge.from=node->number;
		edges_list[number_output_edge]=edge;
		
	}
	
	fprintf(f,"   head \"\"\n");
	if((node->left!=NULL)&&(node->right!=NULL))
	{
			fprintf(f,"   body \"process_pair.cpp\"\n");
	}
	if(node->num_names!=0)
	{
			fprintf(f,"   body \"generate_profile.cpp\"\n");
	}
	fprintf(f,"   tail \"\"\n");
	fprintf(f,"  <NODE_END>\n\n");
	
	flag=write_node
	(
		f,
		node->left,
		node->number,
		left_edge_number,
		edges_list,
		fasta,
		path_to_muscle
	);
	
	if(flag) return -1;

	flag=write_node
	(
		f,
		node->right,
		node->number,
		right_edge_number,
		edges_list,
		fasta,
		path_to_muscle
	);

	if(flag) return -1;

	if((node->left!=NULL)&&(node->right!=NULL))
	{
		
		file_node_body=fopen("root_graph_program.cpp","a");
		
		if(file_node_body==NULL)
		{
			printf("Can't open file 'root_graph_program.cpp'\n");
			return -1;
		}

		fprintf
		(
			file_node_body,
			"pairs[%d].set_values( %d, %d );\n",
			node->number-1,
			node->left->number,
			node->right->number
		);

		fclose(file_node_body);

		/*
		fprintf
		(
			file_node_body,
			"system(\"%s -profile -in1 data_node_%d.fasta -in2 data_node_%d.fasta -out data_node_%d.fasta \");\n",
			path_to_muscle,
			node->left->number,
			node->right->number,
			node->number
		);
		*/
		
		/*
		 * This code commented because this problem
		 * solved in make_align.sh script
		 */
		
		/*
		fprintf
		(
			file_node_body,
			"system(\"rm data_node_%d.fasta data_node_%d.fasta\");\n",
			node->left->number,
			node->right->number
		);
		*/
	}
    
	if(node->num_names!=0)
	{
	 /*
		fprintf
		(
			file_node_body,
			"system(\"%s -in fasta_node_%d.fasta -out data_node_%d.fasta \");\n",
			path_to_muscle,
			node->number,
			node->number			
		);
	*/	
		/* 
		 * This code commented because this problem
		 * solved in make_align.sh script
		 */
		
		/*
		fprintf
		(
			file_node_body,
			"system(\"rm fasta_node_%d.fasta \");\n",
			node->number			
		);
		*/
		
		for(i=0;i<node->num_names;i++)
		{
			fasta.get(node->names[i],record);
			node_fasta.add(record);			
		}

		sprintf(str,"fasta_node_%d.fasta",node->number);
		flag=node_fasta.write(str);
		if(flag)
		{
			printf("The node with number %d can't be written to file '%s'\n",node->number,str);
			return -1;
		}
			
	}

	//fclose(file_node_body);
	
	return 0;
	
}
/*************************************************************************/
int Programm_graph::read(const char *fasta_file,const char *tree_file,int layer)
{
	int flag;
	
	parus::Phil_tree temporary_tree;
	
	flag=temporary_tree.read(tree_file);
	if(flag) return -1;
	
	if(layer==0)
	{
		temporary_tree.merge_after_layer(temporary_tree.get_height(),tree);
	}
	else
	{
		temporary_tree.merge_after_layer(layer,tree);
	}
	
	tree.set_antilayer();
	if(flag) return -1;
	flag=fasta.read(fasta_file);
	if(flag) return -1;

	return 0;
}
/************************************************************************/

