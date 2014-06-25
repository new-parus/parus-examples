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

#include "phil_tree.h"
#include "tree_read.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace parus;

void delete_phil_tree(struct Tree *root);
int merge_tree_after_layer(Tree *old_tree,Tree *new_tree,int layer_number);
int collect_tree_leaves(Tree *root,int *num_leaves,char ***names);
int count_leaves(Tree *root);
int set_anti_layer(Tree *root);
int tree_num_nodes(Tree *root);

/*******************************************************************/
void delete_phil_tree(struct Tree *root)
{
	int i=0;
	
	if(root->names)
	{
		for(i=0;i<root->num_names;i++)
		{
			free(root->names[i]);
			root->names[i]=NULL;
		}

		free(root->names);
		root->names=NULL;
	}

	root->num_names=0;
	
	if(root->left)
	{
		delete_phil_tree(root->left);
		free(root->left);
		root->left=NULL;
	}
	
	if(root->right)
	{
		delete_phil_tree(root->right);
		free(root->right);
		root->right=NULL;
	}

	return;			
}

/*******************************************************************/
Phil_tree::Phil_tree()
{
	height=0;
	root=NULL;
	return;
}
/*******************************************************************/
Phil_tree::~Phil_tree()
{
	if(root)
	{
		delete_phil_tree(root);
		free(root);
		root=NULL;
	}
	
	height=0;

	return;
}
/*******************************************************************/
int Phil_tree::read(const char *file_name)
{
	int flag;
	
	root=(struct Tree *)malloc(sizeof(struct Tree));
	if(root==NULL)
	{
		printf("Phil_tree::read(\"%s\"): memory allocation error \n",file_name);
		return -1;
	}

	root->names=NULL;
	root->num_names=0;
	root->length=0;
	root->left=NULL;
	root->right=NULL;
	root->number=1;
	root->antilayer=-1;
	
	flag=tree_read(file_name,root,&height);

	return 0;
}
/******************************************************************************/
int Phil_tree::set_antilayer(void)
{
	return set_anti_layer(root);
}
/******************************************************************************/
int Phil_tree::merge_after_layer(int layer_number,Phil_tree &merged_tree)
{
	int flag=0;

	Tree *temp=NULL;

	if(height==0)
	{
		printf("merge_after_layer: You try to form the new merged tree using unexisting.\n");
		return -1;
	}
	
	if(merged_tree.height!=0)
	{
		printf("merge_after_layer: You try to form the tree over an existing tree.\n");
		return -1;
	}

	temp=(Tree *)malloc(sizeof(Tree));
	if(temp==NULL)
	{
		printf("merge_after_layer: Memory allocation error.\n");
		return -1;
	}

	temp->left=NULL;
	temp->right=NULL;
	temp->names=NULL;
	temp->num_names=0;
	temp->length=0;
	temp->number=0;
	temp->antilayer=0;
	
	flag=merge_tree_after_layer(root,temp,layer_number);
	if(flag)
	{
		printf
		(
			"Phil_tree::merge_after_layer(%d,Phil_tree &merged_tree): merging failure!\n",
			layer_number
		);
		
		return -1;
	}

	merged_tree.root=temp;

	merged_tree.set_height(layer_number);
	
	return 0;
}	
/********************************************************************************/
int merge_tree_after_layer(Tree *old_tree,Tree *new_tree,int layer_number)
{
	int i;
	int flag=0;
	
	int num_left_leaves=0;
	int num_right_leaves=0;
	int num_names=0;
	
	char **left_leaf_names=NULL;
	char **right_leaf_names=NULL;
	char **names=NULL;

	Tree *left=NULL;
	Tree *right=NULL;
	
	/*
	 * Weather we reach 
	 * leaf in new merged 
	 * tree.
	 *
	 */
	if(layer_number==0)
	{
		if(old_tree->names!=NULL)
		{
			names=(char **)malloc(sizeof(char *)*old_tree->num_names);
			if(names==NULL)
			{
				printf("merge_tree_after_layer: memory allocation error\n");
				return -1;
			}

			for(i=0;i<old_tree->num_names;i++)
			{
				names[i]=(char *)malloc((strlen(old_tree->names[i])+1)*sizeof(char));
				if(names[i]==NULL)
				{
					printf("merge_tree_after_layer: memory allocation error\n");
					for(i=i-1;i>=0;i++) free(names[i]);
					free(names);
					return -1;
				}

				strcpy(names[i],old_tree->names[i]);
			}

			num_names=old_tree->num_names;
		}

		if(old_tree->left!=NULL)
		{
			flag=collect_tree_leaves(old_tree->left,&num_left_leaves,&left_leaf_names);
			
		}
		
		if(flag)
		{
			printf("merge_tree_after_layer: can't merge the leaves in the left subtree. \n");
			return -1;
		}
		
		if(old_tree->right!=NULL)
		{
			flag=collect_tree_leaves(old_tree->right,&num_right_leaves,&right_leaf_names);
		}	
		
		if(flag)
		{
			printf("merge_tree_after_layer: can't merge the leaves in the right subtree. \n");
			return -1;
		}
		
		new_tree->names=(char **)malloc(sizeof(char *)*(num_left_leaves+num_right_leaves+num_names));
		if(new_tree->names==NULL)
		{
			printf("merge_tree_after_layer: memory allocation error.\n");
			return -1;
		}
		new_tree->num_names=num_left_leaves+num_right_leaves+num_names;

		for(i=0;i<num_left_leaves;i++)
		{
			new_tree->names[i]=left_leaf_names[i];
		}

		for(i=0;i<num_right_leaves;i++)
		{
			new_tree->names[i+num_left_leaves]=right_leaf_names[i];
		}

		for(i=0;i<num_names;i++)
		{
			new_tree->names[i+num_left_leaves+num_right_leaves]=names[i];
		}

		new_tree->length=old_tree->length;
		new_tree->number=old_tree->number;
		new_tree->antilayer=old_tree->antilayer;

		free(left_leaf_names);
		free(right_leaf_names);
		free(names);

		return 0;
		


	} /* End if(layer_number==0) */
	
	if(old_tree->left!=NULL)
	{
		left=(Tree *)malloc(sizeof(Tree));
		if(left==NULL)
		{
			printf("merge_tree_after_layer: memory allocation error.\n");
			return -1;
		}

		left->left=NULL;
		left->right=NULL;
		left->names=NULL;
		left->num_names=0;
		left->length=0;
		left->number=0;
		left->antilayer=-1;
		
		flag=merge_tree_after_layer(old_tree->left,left,layer_number-1);

		if(flag)
		{
			return -1;
		}
	}/* end left */

	if(old_tree->right!=NULL)
	{
		right=(Tree *)malloc(sizeof(Tree));
		if(right==NULL)
		{
			printf("merge_tree_after_layer: memory allocation error.\n");
			return -1;
		}

		right->left=NULL;
		right->right=NULL;
		right->names=NULL;
		right->num_names=0;
		right->length=0;
		right->number=0;
		right->antilayer=-1;
		
		flag=merge_tree_after_layer(old_tree->right,right,layer_number-1);

		if(flag)
		{
			return -1;
		}
	} /* end right */
	
	if(old_tree->names)
	{
		names=(char **)malloc(sizeof(char *)*old_tree->num_names);
		if(names==NULL)
		{
			printf("merge_tree_after_layer: memory allocation error.\n");
			return -1;
		}
		
		for(i=0;i<old_tree->num_names;i++)
		{
			names[i]=(char *)malloc((strlen(old_tree->names[i])+1)*sizeof(char));
			if(names[i]==NULL)
			{
				printf("merge_tree_after_layer: memory allocation error.\n");
				for(;i>=0;i--)
				{
					free(names[i]);
				}
				
				free(names);
				return -1;
			}

			strcpy(names[i],old_tree->names[i]);
		}
		
		new_tree->names=names;
		new_tree->num_names=old_tree->num_names;
				
	} /* End names if */
		
	new_tree->length=old_tree->length;
	new_tree->number=old_tree->number;
	new_tree->antilayer=old_tree->antilayer;
	new_tree->left=left;
	new_tree->right=right;


	return 0;
}
/******************************************************************/
int count_leaves(Tree *root)
{
	if(root=NULL) return 0;
	if((root->left==NULL)&&(root->right==NULL))
	{
		return root->num_names;
	}
	
	return count_leaves(root->left)+count_leaves(root->right);
	
}
/******************************************************************/
int collect_tree_leaves(Tree *root,int *num_leaves,char ***param_names)
{
	int num_left_leaves=0;
	int num_right_leaves=0;
	
	char **left_names=NULL;
	char **right_names=NULL;
	char **names=NULL;

	int i;
	int flag=0;

	if((root->left==NULL)&&(root->right==NULL))
	{
		names=(char **)malloc(sizeof(char *)*root->num_names);
		if(names==NULL)
		{
			printf("collect_tree_leaves: memory allocation error\n");
			return -1;
		}

		for(i=0;i<root->num_names;i++)
		{
			names[i]=(char *)malloc((strlen(root->names[i])+1)*sizeof(char));
			if(names[i]==NULL)
			{
				for(;i>=0;i++)
				{
					free(names[i]);
				}
				printf("collect_tree_leaves: memory allocation error\n");
				return -1;
			}

			strcpy(names[i],root->names[i]);
		}

		*num_leaves=root->num_names;
		*param_names=names;		

		return 0;
			
	}

	if(root->left!=NULL)
	{
		flag=collect_tree_leaves(root->left,&num_left_leaves,&left_names);
		if(flag)
		{
			printf("collect_tree_leaves: error while collecting the left leaves\n");
			return -1;
		}
	}

	if(root->right!=NULL)
	{
		flag=collect_tree_leaves(root->right,&num_right_leaves,&right_names);
		if(flag)
		{
			printf("collect_tree_leaves: error while collecting the right leaves\n");
			return -1;
		}
	}

	names=(char **)malloc(sizeof(char *)*(num_left_leaves+num_right_leaves));
	if(names==NULL)
	{
		printf("collect_tree_leaves: memory allocation error.\n");
		return -1;
	}
	
	for(i=0;i<num_left_leaves;i++)
	{
		names[i]=left_names[i];
	}
	
	for(i=0;i<num_right_leaves;i++)
	{
		names[i+num_left_leaves]=right_names[i];
	}
	
	free(left_names);
	free(right_names);

	*num_leaves=num_left_leaves+num_right_leaves;
	*param_names=names;
	
	return 0;
	
}
/******************************************************************/
int set_anti_layer(Tree *root)
{
	int left_antilayer=0;
	int right_antilayer=0;
	
	if(root==NULL) return 0;

	if(root->left!=NULL)
	{
		set_anti_layer(root->left);
		left_antilayer=root->left->antilayer;
	}
	
	if(root->right!=NULL)
	{
		set_anti_layer(root->right);
		right_antilayer=root->right->antilayer;
	}

	if((right_antilayer>0)&&(right_antilayer>left_antilayer))
	{
		root->antilayer=right_antilayer+1;
		return 0;
	}

	if((left_antilayer>0)&&(left_antilayer>=right_antilayer))
	{
		root->antilayer=left_antilayer+1;
		return 0;
	}

	root->antilayer=1;
	return 0;
}

/******************************************************************/
int Phil_tree::get_num_nodes(void)
{
	return tree_num_nodes(root);
}
/******************************************************************/
int tree_num_nodes(Tree *root)
{
	if(root==NULL) return 0;

	return tree_num_nodes(root->left)+tree_num_nodes(root->right)+1;
}
/******************************************************************/

