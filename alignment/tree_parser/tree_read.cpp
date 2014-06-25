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
#include <string.h>
#include <ctype.h>

#include "file_read.h"
#include "phil_tree.h"

using namespace parus;

#define BUFFER_SIZE 1024

int current_symbol;
int node_number=2;

int tree_read(const char *file_name,struct Tree *root,int *height);
int sub_tree_read(TXT_File &f,struct Tree *root,int *height);
int read_white_space(TXT_File &f);
int read_length(TXT_File &f,double *value);
int read_leaf_name(TXT_File &f,char *str);


int tree_read(const char *file_name,struct Tree *root,int *height)
{
	
	TXT_File f;
	int flag=0;
	
	flag=f.open(file_name);
	if(flag)
	{
		printf("tree_read: Can't open file '%s'\n",file_name);
		return -1;
	}
	
	/*
	 * This code is required because
	 * we need to initialize current_symbol
	 * variable before run read_white_space
	 * function. This function tests value 
	 * current_symbol and then read next symbol.
	 */
	current_symbol=' ';	
	
	flag=read_white_space(f);
	if(current_symbol==EOF)
	{
		printf
		(
			"tree_read: Unexpected end of file - string %d column %d\n",
			f.current_string(),
			f.current_column()
		);
		
		return -1;
	}

	flag=sub_tree_read(f,root,height);

	if(flag)
	{
		printf("tree_read: Tree reading failure!\n");
		return -1;
	}

	flag=read_white_space(f);
	if(current_symbol!=';')
	{
		printf
		(
			"tree_read: ';' expected, but is absent: string %d column %d \n",
			f.current_string(),
			f.current_column()
		);
		
		return -1;
	}

	return 0;
}
/******************************************************************/
int sub_tree_read(TXT_File &f,struct Tree *root,int *height)
{
	int flag=0;

	double left_length;
	double right_length;
	char str[BUFFER_SIZE];
	
	struct Tree *left=NULL;
	struct Tree *right=NULL;
	char **names=NULL;

	int left_height=0;
	int right_height=0;

	int i;
	
	if(current_symbol!='(')
	{
		printf
		(
			"sub_tree_read: '(' expected, but '%c' is read: string %d column %d\n",
			 current_symbol,
			 f.current_string(),
			 f.current_column()
		);
		
		return -1;
	}
	
	left=(struct Tree *)malloc(sizeof(struct Tree));

	if(left==NULL)
	{
			printf("sub_tree_read: memory allocation error\n");
			return -1;
	}
		
	left->names=NULL;
	left->num_names=0;
	left->left=NULL;
	left->right=NULL;
	left->length=0;
	left->number=node_number;
	node_number++;
		
	right=(struct Tree *)malloc(sizeof(struct Tree));
	
	if(right==NULL)
	{
		printf("sub_tree_read: memory allocation error\n");
		free(left);
		return -1;
	}
		
	right->names=NULL;
	right->num_names=0;
	right->left=NULL;
	right->right=NULL;
	right->length=0;
	right->number=node_number;
	node_number++;
	
	current_symbol=f.getc();
	
	flag=read_white_space(f);

	/*
	 *
	 * Reading left subtree
	 *
	 */
	
	if(current_symbol=='(')
	{
				
		flag=sub_tree_read(f,left,&left_height);
		if(flag)
		{
			printf("sub_tree_read: Reading left subtree failed!\n");
			free(left);
			free(right);
			return -1;
		}

	}
	else
	{
		flag=read_leaf_name(f,str);
		if(flag)
		{
			printf("sub_tree_read: Error while reading leaf.\n");
			free(left);
			free(right);
			return -1;
		}
		
		left->names=(char **)malloc(sizeof(char *));
		if(left->names==NULL)
		{
			printf("sub_tree_read: memory allocation error\n");
			free(left);
			free(right);
			return -1;
		}
		
		left->names[0]=(char *)malloc((strlen(str)+1)*sizeof(char));
		if(left->names[0]==NULL)
		{
			printf("sub_tree_read: memory allocation error\n");
			free(left->names);
			free(left);
			free(right);
			return -1;

		}

		strcpy(left->names[0],str);
		left->num_names=1;
		//left_height=1;
	}
	
	flag=read_white_space(f);
	
	
	if(current_symbol!=':')
	{
		printf
		(
			"sub_tree_read: ':' expected, but '%c' read: string %d column %d\n",
			 current_symbol,
			 f.current_string(),
			 f.current_column()
		);
		
		if(left->names)
		{
			if(left->names[0]) free(left->names[0]);
			free(left->names);
		}
		
		free(left);
		free(right);
		
		return -1;
	}

	current_symbol=f.getc();
	flag=read_white_space(f);
	
	flag=read_length(f,&left_length);
	if(flag)
	{
		printf("sub_tree_read: Can't read left length\n");
		
		if(left->names)
		{
			if(left->names[0]) free(left->names[0]);
			free(left->names);
		}
		
		free(left);
		free(right);

		return -1;
	}

	left->length=left_length;
	
	flag=read_white_space(f);
	
	/*
	 *
	 * Read delimiter ','
	 * 
	 */

	if(current_symbol!=',')
	{
		printf
		(
			"sub_tree_read: ',' expected, but '%c' read: string %d column %d\n",
			current_symbol,
			f.current_string(),
			f.current_column()
		);

		if(left->names)
		{
			if(left->names[0]) free(left->names[0]);
			free(left->names);			
		}
		
		free(left);
		free(right);
		
		return -1;
	}

	current_symbol=f.getc();
	flag=read_white_space(f);
	
	/*
	 *
	 * Reading right subtree
	 *
	 */
	
	if(current_symbol=='(')
	{
	
		
		flag=sub_tree_read(f,right,&right_height);
		if(flag)
		{
			printf("sub_tree_read: Reading right subtree failed!\n");
			
			if(left->names)
			{
				if(left->names[0]) free(left->names[0]);
				free(left->names);
			}

			free(left);
			free(right);
			
			return -1;
		}

	
	}
	else
	{
		flag=read_leaf_name(f,str);
		if(flag)
		{
			printf("sub_tree_read: Error while reading leaf.\n");
			return -1;
		}
		
		right->names=(char **)malloc(sizeof(char *));
		if(right->names==NULL)
		{
			printf("sub_tree_read: memory allocation error\n");
			
			if(left->names)
			{
				if(left->names[0]) free(left->names[0]);
				free(left->names);
			}

			free(left);
			free(right);
			return -1;

		}
		
		right->names[0]=(char *)malloc((strlen(str)+1)*sizeof(char));
		if(right->names[0]==NULL)
		{
			printf("sub_tree_read: memory allocation error\n");
			
			if(left->names)
			{
				if(left->names[0]) free(left->names[0]);
				free(left->names);
			}

			free(right->names);
			
			free(left);
			free(right);
			return -1;

		}

		strcpy(right->names[0],str);
		right->num_names=1;
		//right_height=1;
	}
	
	flag=read_white_space(f);
	

	if(current_symbol!=':')
	{
		printf
		(
			"sub_tree_read: ':' expected, but '%c' read: string %d column %d\n",
			 current_symbol,
			 f.current_string(),
			 f.current_column()
		);
		
		if(left->names)
		{
			if(left->names[0]) free(left->names[0]);
			free(left->names);
		}
			
		free(left);

		if(right->names)
		{
			if(right->names[0]) free(right->names[0]);
			free(right->names);
		}
		
		free(right);
		
		return -1;
	}
	
	current_symbol=f.getc();
	flag=read_white_space(f);
	
	flag=read_length(f,&right_length);
	if(flag)
	{
		printf("sub_tree_read: Can't read right length\n");

		if(left->names)
		{
			if(left->names[0]) free(left->names[0]);
			free(left->names);
		}
		
		free(left);
		
		if(right->names)
		{
			if(right->names[0]) free(right->names[0]);
			free(right->names);
		}
		
		free(right);
		
		return -1;
	}

	right->length=right_length;

	flag=read_white_space(f);

	if(current_symbol!=')')
	{
		printf
		(
			"sub_tree_read: ')' expected, but '%c' read: string %d column %d\n",
			 current_symbol,
			 f.current_string(),
			 f.current_column()
		);

		if(left->names!=NULL)
		{
			for(i=0;i<left->num_names;i++)
				free(left->names[i]);
			free(left->names);
		}
		free(left);
		if(right->names!=NULL)
		{	
			for(i=0;i<right->num_names;i++)
				free(right->names[i]);
			free(right->names);
		}
		free(right);
		
		return -1;
	}

	current_symbol=f.getc();

	root->left=left;
	root->right=right;

	if(left_height>right_height)
		*height=left_height+1;
	else
		*height=right_height+1;

	return 0;
}
/******************************************************************************/
int read_white_space(TXT_File &f)
{
	//current_symbol=f.getc();
	
	while
	(
		(current_symbol==' ')||
		(current_symbol=='\t')||
		(current_symbol=='\n')||
		(current_symbol=='\r')
	)
		current_symbol=f.getc();

	if(current_symbol==EOF)
		return -1;

	return 0;
}
/******************************************************************************/
int read_length(TXT_File &f,double *value)
{
	char str[256]="";
	int i;

	for
	(
		i=0;
		(((current_symbol>='0')&&(current_symbol<='9'))||
		(current_symbol=='-')||
		(current_symbol=='+')||
		(current_symbol=='e')||
		(current_symbol=='E')||
		(current_symbol=='.'))&&
		(i<256-1);
		i++
	)
	{
		str[i]=current_symbol;
		current_symbol=f.getc();
	}

	str[i]='\0';

	if(current_symbol==EOF)
		return -1;

	*value=strtod(str,NULL);


	return 0;
			
}
/******************************************************************************/
int read_leaf_name(TXT_File &f,char *str)
{
	int i,j;
	for
	(
		i=0;
		(!isspace(current_symbol))&&
		(current_symbol!=':')&&
		(current_symbol!='|')&&
		(current_symbol!='/')&&
		(current_symbol!=EOF)&&
		(i<(BUFFER_SIZE-1));
		i++
	)
	{
		str[i]=current_symbol;
		current_symbol=f.getc();
	}

	if(i==(BUFFER_SIZE-1))
	{
		printf("Very long record name (more %d symbols)\n",BUFFER_SIZE-1);
		return -2;
	}

	str[i]='\0';

	if
	(
		isspace(current_symbol)&&(current_symbol!='\n')||
		(current_symbol=='|')||
		(current_symbol=='/')
	)
	{
		/* XXX 
		 * There is a question. 
		 * Is it necesossary code '(current_symbol!='\n')' for correct working of program
		 * 
		 */
		for(j=i;(current_symbol!='\n')&&(current_symbol!=':')&&(current_symbol!=EOF);j++)
		{
			current_symbol=f.getc();
		}
	}

	for(i--;isspace(str[i]);i--)
	{
		str[i]='\0';
	}

	if(current_symbol==EOF)
			return -1;
	
	return 0;
}
/******************************************************************************/

