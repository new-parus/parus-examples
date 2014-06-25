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


#ifndef __PHIL_TREE_H__
#define __PHIL_TREE_H__

namespace parus
{

/**
 * A tree node for the Philogenetic tree is described
 * lower.
 */
struct Tree
{
	/**
	 * Leaves names
	 */
	char **names;
	/**
	 * Number of leaves names
	 */
	int num_names;
	/**
	 * Node number. It is used for building 
	 * a parallel program with  this tree. 
	 */
	int number;
	/**
	 * Length  branch from parent to this node 
	 * 
	 */
	double length;
	/**
	 * Layer number in reverse layer numeration
	 * It is used during the execution process
	 * of the parallel program built with this tree.
	 *
	 */
	int antilayer;
	/**
	 * left child of this node
	 */
	Tree *left;
	/**
	 * right child of this node
	 */
	Tree *right;	
};



/**
 *
 * This class describes Philogenetic trees.
 * It is used for parsing .phy files, getting 
 * leaf names from sub trees and so on.
 *
 */
class Phil_tree
{
protected:
	Tree *root;
	int height;
public:
	
	/**
	 * This constructor sets 
	 * values to 0.	
	 */
	Phil_tree();
	
	/**
	 * Delete tree and all subtrees.
	 */
	~Phil_tree();
	
	/**
	 * Read the tree from a file. 
	 */
	int read(const char *file_name);
	
	/**
	 * This function forms a new merged tree by other trees.
	 * Nodes after node shown as 'layer_number' parameter are merged
	 * to one leaf. All names of the leaves of the merged nodes are
	 * attached to the new leaf.
	 * 
	 */
	int merge_after_layer(int layer_number,Phil_tree &merged_tree);
	
	/**
	 *
	 * This function sets inverse layer enumeration.
	 * It sets layer enumeration from leaves to root vertexes.
	 * 
	 */
	int set_antilayer(void);
	
	/**
	 * Get the height of the tree.
	 */
	inline int get_height(void) { return height; };
	/**
	 * It returns a link to the root node in the  tree. 
	 */
	inline Tree* get_root(void) { return root;   };
	int get_num_nodes(void);
	/**
	 * Sets height of the tree
	 */
	inline void set_height(int new_height) {height=new_height;};
};

		
} /* END namespace parus */

#endif /* __PHIL_TREE_H__ */

