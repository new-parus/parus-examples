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


#ifndef __FASTA_H__
#define __FASTA_H__

#include <stdio.h>

/**
 *
 * This class implements one 
 * sequence record in FASTA file.
 *
 */
class Sequence_record
{
 protected:
	char *name;
	char *comment;
	char *align;
 public:
	Sequence_record();
	~Sequence_record();
	int read(FILE *f);
	int write(FILE *f);
	/**
	 * Get name of sequence record from Sequence_record
	 * object.
	 */
	inline char *get_name(void)
	{
		return name;
	};
	inline char *get_comment(void)
	{
		return comment;
	};
	inline char *get_align(void)
	{
		return align;
	};
	
	int fill_record(char *new_name,char *new_comment,char *new_align); 
	
			
};

class Fasta
{
 protected:
	int num_records;
	Sequence_record **sequence_records;
 public:
	 Fasta();
	 ~Fasta();
	 int read(const char *fasta_file_name);
	 int write(const char *fasta_file_name);
	 int get(const char *record_name,Sequence_record &record);
	 int add(Sequence_record &record);
	 inline int get_num_records(void) { return num_records; }; 
	
};

#endif /* __FASTA_H__ */

