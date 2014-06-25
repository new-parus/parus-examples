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


#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <string>
using namespace std;

#include "fasta.h"

#define BUF_SIZE 1024

char str_buf[BUF_SIZE+1];


/*
******************************************************************
*                                                                *
* The Sequence_record class part of source.                      *
*                                                                *
******************************************************************
*/



/********************************************************************/
Sequence_record::Sequence_record()
{
	name=NULL;
	comment=NULL;
	align=NULL;
	return;
}
/********************************************************************/
Sequence_record::~Sequence_record()
{
	if(name!=NULL)      free(name);
	if(comment!=NULL)   free(comment);
	if(align!=NULL)     free(align);
	return;
}
/*******************************************************************/
int Sequence_record::read(FILE *f)
{
	
	string str="";
	int i;

	int c;
		
	c=fgetc(f);
	if(c==EOF) return -1;
	
	/*
	 *
	 * Reading name of sequence in FASTA format.
	 *
	 */
	i=0;
	while(!isspace(c)&&(c!='|')&&(c!='/'))
	{
		switch(c)
		{
			case ':':
			case ',':
			case ';':
				c='_';
			break;
			case '(':
				c='{';
			break;
			case ')':
				c='}';
			break;
		}

		str_buf[i]=c;
		i++;
		if(i==BUF_SIZE)
		{
			str_buf[BUF_SIZE]='\0';
			str+=str_buf;
			i=0;
		}
		c=fgetc(f);
		if(c==EOF) return -1;
	}
	
	if(i)
	{
		str_buf[i]='\0';
		str+=str_buf;
	}

	name=(char *)malloc((str.size()+1)*sizeof(char));
	strcpy(name,str.c_str());

	/*
	 *
	 * Reading comments to sequence in FASTA format 
	 *
	 */
	if(c!='\n')
	{
		c=fgetc(f);
		if(c==EOF) return -1;
	}
	
	str="";
	i=0;

	while(c!='\n')
	{
		str_buf[i]=c;
		i++;
		if(i==BUF_SIZE)
		{
			str_buf[BUF_SIZE]='\0';
			str+=str_buf;
			i=0;
		}
		c=fgetc(f);
		if(c==EOF) return -1;
	}

	if(i)
	{
		str_buf[i]='\0';
		str+=str_buf;
	}

	comment=(char *)malloc((str.size()+1)*sizeof(char));
	strcpy(comment,str.c_str());

	/*
	 *
	 * Reading sequence in FASTA format
	 *
	 */
	
	c=fgetc(f);
	i=0;
	str="";
	
	while((c!='>')&&(c!=EOF))
	{
		if(
			/*
			 * Nucleotides
			 */
			(c!='a')&&
			(c!='t')&&
			(c!='g')&&
			(c!='c')&&
			(c!='s')&&
			(c!='w')&&
			(c!='r')&&
			(c!='y')&&
			(c!='k')&&
			(c!='m')&&
			(c!='b')&&
			(c!='v')&&
			(c!='h')&&
			(c!='d')&&
			(c!='n')&&
			(c!='u')&&
			(c!='A')&&
			(c!='T')&&
			(c!='G')&&
			(c!='C')&&
			(c!='S')&&
			(c!='W')&&
			(c!='R')&&
			(c!='Y')&&
			(c!='K')&&
			(c!='M')&&
			(c!='B')&&
			(c!='V')&&
			(c!='H')&&
			(c!='D')&&
			(c!='N')&&
			(c!='U')&&
			/*
			 * acids
			 * a,b,c,d,g,h,k,m,n,r,s,t,v,w are using in 
			 * nucleotides
			 */
			(c!='e')&&
			(c!='f')&&
			(c!='i')&&
			(c!='l')&&			
			(c!='p')&&
			(c!='q')&&
			(c!='x')&&
			(c!='y')&&
			(c!='z')&&
			(c!='E')&&
			(c!='F')&&
			(c!='I')&&
			(c!='L')&&			
			(c!='P')&&
			(c!='Q')&&
			(c!='X')&&
			(c!='Y')&&
			(c!='Z')&&
			/*
			 * delimiters
			 */
			(c!='-')&&
			(c!='.')&&
			/*
			 * white space
			 */
			(c!=' ')&&
			(c!='\n')&&
			(c!='\r')&&
			(c!='\t')&&
			/*
			 * digits
			 */
			!((c>=0)&&(c<=9))
		)/* End if condition */
		{
			return -2;
		}

		if((c==' ')||(c=='\n')||(c=='\r')||(c=='\t')||(c>=0)&&(c<=9))
		{
			c=fgetc(f);
			continue;
		}
		
		str_buf[i]=c;
		i++;
		if(i==BUF_SIZE)
		{
			str_buf[BUF_SIZE]='\0';
			str+=str_buf;
			i=0;
		}
		c=fgetc(f);
			
	} /* End while */

	if(i)
	{
		str_buf[i]='\0';
		str+=str_buf;
	}
	
	align=(char *)malloc((str.size()+1)*sizeof(char));
	strcpy(align,str.c_str());

	if(c==EOF) return 1;

	return 0;
}
/******************************************************************/
int Sequence_record::write(FILE *f)
{
	if(name==NULL)
	{
		return -1;
	}

	if(align==NULL)
	{
		return -2;
	}

	if(comment!=NULL)
	{
		fprintf(f,">%s %s\n%s\n",name,comment,align);
	}
	else
	{
		fprintf(f,">%s\n%s\n",name,align);
	}
	return 0;
}
/*****************************************************************/
int Sequence_record::fill_record(char *new_name,char *new_comment,char *new_align)
{
	if(name!=NULL)
	{
		free(name);
		name=NULL;
	}

	if(comment!=NULL)
	{
		free(comment);
		comment=NULL;
	}

	if(align!=NULL)
	{
		free(align);
		align=NULL;
	}
	
	if(new_name!=NULL)
	{
		name=(char *)malloc((strlen(new_name)+1)*sizeof(char));
		if(name==NULL) return -1;
		strcpy(name,new_name);
	}

	if(new_comment!=NULL)
	{
		comment=(char *)malloc((strlen(new_comment)+1)*sizeof(char));
		if(comment==NULL) return -1;
		strcpy(comment,new_comment);
	}

	if(new_align!=NULL)
	{
		align=(char *)malloc((strlen(new_align)+1)*sizeof(char));
		if(align==NULL) return -1;
		strcpy(align,new_align);
	}

	return 0;
}
/*****************************************************************/

/*
******************************************************************
*                                                                *
* The Fasta class part of source.                                *
*                                                                *
******************************************************************
*/


/*****************************************************************/
Fasta::Fasta()
{
	num_records=0;
	sequence_records=NULL;

	return;
}
/*****************************************************************/
Fasta::~Fasta()
{
	int i;
	for(i=0;i<num_records;i++)
	{
		delete sequence_records[i];
	}
	
	if(sequence_records!=NULL) free(sequence_records);
	
	return;	
}
/****************************************************************/
int Fasta::read(const char *file_name)
{
	FILE *f;
	int c;
	
	int flag=0;

	int i;

	Sequence_record **new_recs=NULL;

	Sequence_record *current_seq_rec=NULL;
	
	
	f=fopen(file_name,"r");
	if(f==NULL)
	{
		printf("Fasta::read(\"%s\") -- can't open file\n",file_name);
		return -1;
	}

	c=fgetc(f);

	while(isspace(c))
	{
		c=fgetc(f);
	}
	
	if(c!='>')
	{
		printf("Error: the file is not in FASTA format!\n");
		fclose(f);
		return -1;
	}

	while(!flag)
	{
		new_recs=(Sequence_record **)malloc(sizeof(Sequence_record *)*(num_records+1));
		if(new_recs==NULL)
		{
			printf("Memory allocation error!\n");
			fclose(f);
			return -1;
		}
		
		for(i=0;(i<num_records)&&(sequence_records!=NULL);i++)
		{
			new_recs[i]=sequence_records[i];
		}
		
		current_seq_rec= new Sequence_record;
		
		flag=current_seq_rec->read(f);		
		
				
		if((flag==0)||(flag==1))
		{
			new_recs[num_records]=current_seq_rec;
			num_records++;
			if(sequence_records!=NULL) 
				free(sequence_records);
			sequence_records=new_recs;
		}

		if(flag==-2)
		{
			printf
			(
				"Bad sequence format for the record named %s\n",
				current_seq_rec->get_name()
			);
			
			delete current_seq_rec;
			free(new_recs);
			fclose(f);
			
			return -1;
		}
	} /* end while */
	
	
	fclose(f);
	return 0;
}
/****************************************************************/
int Fasta::write(const char *file_name)
{
	FILE *f;
	int i;
	int flag;
	
	f=fopen(file_name,"w");
	if(f==NULL)
	{
		printf("Fasta::write(\"%s\") -- can't open file\n",file_name);
		return -1;
	}

	for(i=0;i<num_records;i++)
	{
		flag=sequence_records[i]->write(f);
		if(flag)
		{
			printf("Error: the record can't be written to the file '%s'\n",file_name);
			if(flag==-1)
			{
				printf("\tThe record has empty name\n");
			}
			if(flag==-2)
			{
				printf("\tThe record has empty sequence\n");
			}
			return -1;
		}
	}

	fclose(f);
	
	return 0;
}
/****************************************************************/
int Fasta::get(const char *record_name,Sequence_record &record)
{
	int i;
	int flag;

	Sequence_record *tmp_rec;
	
	for(i=0;i<num_records;i++)
	{
		tmp_rec=sequence_records[i];
		if(!strcmp(tmp_rec->get_name(),record_name))
		{
						
			flag=record.fill_record
			(
				tmp_rec->get_name(),
				tmp_rec->get_comment(),
				tmp_rec->get_align()
			);
			
			if(flag)
			{
				printf("Fasta::get(const char *record_name=\"%s\",Sequence_record &record): - memmory allocation problems\n",record_name);
				return -2;
			}
			return 0;
		}
	}
	
	/*
	 * Record not found
	 */
	return -1;
}
/*****************************************************************/
int Fasta::add(Sequence_record &record)
{
	int i;
	int flag;
	Sequence_record *new_record=NULL;
	Sequence_record **new_records=NULL;

	new_record=new Sequence_record;
	if(new_record==NULL) return -1;

	flag=new_record->fill_record
	                 (
					 	record.get_name(),
	                 	record.get_comment(),
	                 	record.get_align()
	                 );

	if(flag)
	{
		delete new_record;
		return -1;
	}

	new_records=(Sequence_record **)malloc(sizeof(Sequence_record *)*(num_records+1));
	if(new_records==NULL)
	{
		delete new_record;
		return -1;
	}

	for(i=0;i<num_records;i++)
	{
		new_records[i]=sequence_records[i];
	}
	new_records[i]=new_record;

	num_records++;

	free(sequence_records);

	sequence_records=new_records;

	return 0;
	
}
/*****************************************************************/

