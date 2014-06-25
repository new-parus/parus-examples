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

/*
*******************************************************************************
*             This is an implementation of litle library allows parse         *
*             parametersb in header NetCDF file that kernel send nmodule.     *
*                                                                             *
* Written by Alexey Salnikov in 2004 year.                                    *
*                                                                             *
*******************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netcdf.h>

#define __MODULE_HEADER_C__
  #include "module_header.h"
#undef __MODULE_HEADER_C__

int read_module_header(const char *file_name,Module_parameters *parms);
void free_parameters_struct(Module_parameters *parms);
int print_module_parameters(Module_parameters *parms);

int get_module_attribute_value(Module_parameters *parms, const char *name, void *val);
int get_module_attribute_size(Module_parameters *parms, const char *name,size_t *size);
int get_module_attribute_type(Module_parameters *parms, const char *name,char *type);

int get_file_type_name(char type, char *name);
int get_module_attribute_type_name(char type, char *name);


/******************************************************************************************/
int read_module_header(const char *file_name,Module_parameters *parms)
{
	int netcdf_fd;
	int status;
	int parameter_file_id;
	size_t parameter_file_name_length;
	size_t len;
	
	int num_files;
	int num_attributes;
	Module_attribute *attributes;
	Module_parameter_file *files;
	char *name;
	char type;
	char type_size;
	nc_type att_type;
	size_t size;
	void *val;
	
	char str[512]="";
	
	int i;
	
	
	status=nc_open(file_name,NC_NOWRITE,&netcdf_fd);
        if(status!=NC_NOERR)
        {
		fprintf(stderr,"Can't open file '%s' in read mode.\n",file_name);
		fprintf(stderr,"%s\n\n",nc_strerror(status));
		return -1;
	}
	
	/*
	 *
	 * Next code read information about all files those are shown in
	 * module parametrs.
	 *
	 */
	nc_inq_nvars(netcdf_fd,&num_files);
	
	parms->num_files=num_files;
	
	files=(Module_parameter_file *)calloc(num_files,sizeof(Module_parameter_file));
	if(files==NULL)
	{
		perror("Can't allocate memory for files in module parameters\n");
		nc_close(netcdf_fd);
		return -1;
	}
	
	for(i=0;i<num_files;i++)
	{
		
		/*
		 *
		 * Read file name and file type for one copy of specified
		 * in parametrs file.
		 * 
		 */
		sprintf(str,"file%d",i+1);
		
		nc_inq_varid(netcdf_fd,str,&parameter_file_id);
		nc_inq_attlen(netcdf_fd,parameter_file_id,"name",&parameter_file_name_length);
		
		name=(char *)malloc((parameter_file_name_length+1)*sizeof(char));
		if(name==NULL)
		{
			fprintf(stderr,"Allocation memory error: file name %d \n",i);
			nc_close(netcdf_fd);
			return -1;
		}
		
		/*
		 *
		 * Now we read module parameter file name, not attribute name.
		 * 
		 */
		status=nc_get_att_text(netcdf_fd,parameter_file_id,"name",name);
		if(status!=NC_NOERR)
		{
			fprintf(stderr,"Can't get type of file%d variable in header\n",i);
			fprintf(stderr,"%s\n\n",nc_strerror(status));
			return -1;
		}
		name[parameter_file_name_length]='\0';
		nc_inq_attlen(netcdf_fd,parameter_file_id,"name",&len);
		files[i].name=name;

		
		nc_inq_attlen(netcdf_fd,parameter_file_id,"type",&len);
		status=nc_get_att_text(netcdf_fd,parameter_file_id,"type",str);
		if(status!=NC_NOERR)
		{
			fprintf(stderr,"Can't get type of file%d variable in header\n",i);
			fprintf(stderr,"%s\n\n",nc_strerror(status));
			return -1;
		}
		str[len]='\0';
		
		type=HEADER_ERROR_TYPE;
		if(!strcmp(str,"in")) type=HEADER_INPUT_TYPE;
		if(!strcmp(str,"out")) type=HEADER_OUTPUT_TYPE;
		if(!strcmp(str,"inout")) type=HEADER_INOUT_TYPE;
		if(!strcmp(str,"doneflag")) type=HEADER_DONEFLAG_TYPE;
		
		if(type==HEADER_ERROR_TYPE)
		{
			fprintf(stderr,"unknown type of file in module header\n");
			nc_close(netcdf_fd);
			return -1;
		}
		
		files[i].type=type;
		
	}
	
	/*
	 *
	 * Moving link from local variable to structure
	 *
	 */
	parms->files=files;
	
	/*
	 *
	 * Next piece of code read all global attributes from NetCDF header of
	 * "System" module.
	 * 
	 *
	 */
	
	/*
	 *
	 * Read number of global attributes
	 *
	 */
	nc_inq_varnatts(netcdf_fd,NC_GLOBAL,&num_attributes);
	
	parms->num_attributes=num_attributes;
	
	attributes=(Module_attribute *)calloc(num_attributes,sizeof(Module_attribute));
	
	if(attributes==NULL)
	{
		perror("Can't allocate memory for attributes in module parameters\n");
		nc_close(netcdf_fd);
		return -1;
	}
	
	
	for(i=0;i<num_attributes;i++)
	{	
		/*
		 *
		 * Get name by order number
		 *
		 */
		nc_inq_attname(netcdf_fd,NC_GLOBAL,i,str);
		name=(char *)malloc((strlen(str)+1)*sizeof(char));
		if(name==NULL)
		{
			fprintf(stderr,"Allocation memory error: attribute name %s \n",str);
			nc_close(netcdf_fd);
			return -1;
		}
		strcpy(name,str);
		
		
		/*
		 *
		 * Get information about attribute
		 * 
		 */
		nc_inq_att(netcdf_fd,NC_GLOBAL,str,&att_type,&size);
		
		switch(att_type)
		{
			case NC_CHAR:
				type=HEADER_ATT_SCHAR;
				type_size=sizeof(signed char);
			break;
			case NC_BYTE:
				type=HEADER_ATT_UCHAR;
				type_size=sizeof(unsigned char);
			break;
			case NC_SHORT:
				type=HEADER_ATT_SHORT;
				type_size=sizeof(short);
			break;
			case NC_INT:
				type=HEADER_ATT_INT;
				type_size=sizeof(int);
			break;
			case NC_FLOAT:
				type=HEADER_ATT_FLOAT;
				type_size=sizeof(float);
			break;
			case NC_DOUBLE:
				type=HEADER_ATT_DOUBLE;
				type_size=sizeof(double);
			break;
			default:
				fprintf(stderr,"NetCDF attribute with name %s type %d is not registered\n",name,att_type);
				nc_close(netcdf_fd);
				return -1;
			break;
		}
		
		/*
		 *
		 * allocating memory for global attribute
		 *
		 */
		val=malloc(size*type_size);
		if(val==NULL)
		{
			fprintf(stderr,"There are no enough memory for attribute with name %s.",name);
			nc_close(netcdf_fd);
			return -1;
		}
		
		/*
		 *
		 * Get value of attribute from NetCDF file.
		 *
		 */
		switch(type)
		{
			case HEADER_ATT_UCHAR:
				nc_get_att_uchar(netcdf_fd,NC_GLOBAL,name,(unsigned char *)val);
			break;
			case HEADER_ATT_SCHAR:
				nc_get_att_schar(netcdf_fd,NC_GLOBAL,name,(signed char *)val);
			break;
			case HEADER_ATT_SHORT:
				nc_get_att_short(netcdf_fd,NC_GLOBAL,name,(short *)val);
			break;
			case HEADER_ATT_INT:
				nc_get_att_int(netcdf_fd,NC_GLOBAL,name,(int *)val);
			break;
			case HEADER_ATT_FLOAT:
				nc_get_att_float(netcdf_fd,NC_GLOBAL,name,(float *)val);
			break;
			case HEADER_ATT_DOUBLE:
				nc_get_att_double(netcdf_fd,NC_GLOBAL,name,(double *)val);
			break;
		}
		
		/*
		 *
		 * Filling attribute in stricture.
		 * 
		 */
		attributes[i].type=type;
		attributes[i].name=name;
		attributes[i].value=val;
		attributes[i].size=size;
		
	} /* End for(;;) by global attributes in NetCDF file */
	
	/*
	 *
	 * assign attributes to parameters structure
	 * 
	 */
	parms->attributes=attributes;
	
	
	nc_close(netcdf_fd);
	
	return 0;
}
/******************************************************************************************/
void free_parameters_struct(Module_parameters *parms)
{
	int i;
	
	for(i=0;i<parms->num_files;i++)
	{
		if(parms->files[i].name!=NULL)
		{
			free(parms->files[i].name);
		}
	}
	
	if(parms->files!=NULL)
		free(parms->files);
	
	parms->files=NULL;
	parms->num_files=0;
	
	for(i=0;i<parms->num_attributes;i++)
	{
		if(parms->attributes[i].name!=NULL)
			free(parms->attributes[i].name);
		if(parms->attributes[i].value!=NULL)
			free(parms->attributes[i].value);
	}
	
	if(parms->attributes!=NULL)
		free(parms->attributes);
	parms->attributes=NULL;
	parms->num_attributes=0;
	
	return;
}
/******************************************************************************/
int print_module_parameters(Module_parameters *parms)
{
	int i;
	char str[51]="";
	
	printf("Parameters for module are:\n\n");
	printf("\tFiles:\n\n");
	
	for(i=0;i<parms->num_files;i++)
	{
		get_file_type_name(parms->files[i].type,str);
		printf("\t\ttype=\"%s\"\n",str);
		printf("\t\tname=\"%s\"\n\n",parms->files[i].name);
	}

	printf("\tAttributes:\n\n");
	
	for(i=0;i<parms->num_attributes;i++)
	{
		get_module_attribute_type_name(parms->attributes[i].type,str);
		printf("\t\tname=\"%s\"\n",parms->attributes[i].name);
		printf("\t\ttype=\"%s\"\n",str);
		printf("\t\tsize= %d\n\n",parms->attributes[i].size);
	}
	
	return 0;
}
/******************************************************************************/
int get_module_attribute_value
(
	Module_parameters *parms,
	const char *name,
	void *val
)
{
	int i;
	size_t type_size;
	
	Module_attribute *attributes;
	
	
	attributes=parms->attributes;
	
	

	for(i=0;i<parms->num_attributes;i++)
	{
		if(!strcmp(attributes[i].name,name))
		{
				switch(attributes[i].type)
				{
 	 				case HEADER_ATT_SCHAR:
						type_size=sizeof(signed char);
					break;
					case HEADER_ATT_UCHAR:
						type_size=sizeof(unsigned char);
					break;
					case HEADER_ATT_SHORT:
						type_size=sizeof(short);
					break;
					case HEADER_ATT_INT:
						type_size=sizeof(int);
					break;
					case HEADER_ATT_FLOAT:
						type_size=sizeof(float);
					break;
					case HEADER_ATT_DOUBLE:
						type_size=sizeof(double);
					break;
					default:
						fprintf(stderr,"Unknown attribute type  %d \n",attributes[i].type);
						return -1;
					break;
				}
			memcpy(val,attributes[i].value,type_size*attributes[i].size);
			return 0;
		}
	}
	
	return -1;
}
/******************************************************************************/
int get_module_attribute_size
(
	Module_parameters *parms,
	const char *name,
	size_t* size
)
{
	int i;
	
	Module_attribute *attributes;
	
	attributes=parms->attributes;
	
	for(i=0;i<parms->num_attributes;i++)
	{
		if(!strcmp(attributes[i].name,name))
		{
			*size=attributes[i].size;
			return 0;
		}
	}
	
	return -1;
}
/******************************************************************************/
int get_module_attribute_type
(
	Module_parameters *parms,
	const char *name,
	char *type
)
{
	int i;
	
	Module_attribute *attributes;
	
	attributes=parms->attributes;
	
	for(i=0;i<parms->num_attributes;i++)
	{
		if(!strcmp(attributes[i].name,name))
		{
			*type=attributes[i].type;
			return 0;
		}
	}
	
	return -1;
}
/******************************************************************************/
int get_file_type_name(char type, char *name)
{
	switch(type)
	{
		case HEADER_INPUT_TYPE:
			strcpy(name,"in");
		break;
		case HEADER_OUTPUT_TYPE:
			strcpy(name,"out");
		break;
		case HEADER_INOUT_TYPE:
			strcpy(name,"inout");
		break;
		case HEADER_DONEFLAG_TYPE:
			strcpy(name,"doneflag");
		break;
		default:
			strcpy(name,"(unknown type)");
			return -1;
		break;
	}
	return 0;
}
/*******************************************************************************/
int get_module_attribute_type_name(char type, char *name)
{
	switch(type)
	{
 		case HEADER_ATT_SCHAR:
			strcpy(name,"char");
		break;
		case HEADER_ATT_UCHAR:
			strcpy(name,"unsigned char");
		break;
		case HEADER_ATT_SHORT:
			strcpy(name,"short");
		break;
		case HEADER_ATT_INT:
			strcpy(name,"int");
		break;
		case HEADER_ATT_FLOAT:
			strcpy(name,"float");
		break;
		case HEADER_ATT_DOUBLE:
			strcpy(name,"double");
		break;
		default:
			strcpy(name,"(unknown type)");
			return -1;
		break;
	}
	return 0;
}
/******************************************************************************/

