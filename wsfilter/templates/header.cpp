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

#include <string.h>

#include <netcdf.h>

#include "module_header.h"
#include <lspwsfilter.h>


/*
 * Allow to determine position
 * Set num NUM_PROCS macro variable.
 */
#include "wsf_procs.h"

#define F_LEN 2048
#define CUT_OFF 15000
#define NORMALIZE_COEFFICIENT 32767.0+32767.0/32768.0
#define MAX_VALUE 32768

int netcdf_fd;
int FLen;
int Cutoff;
size_t length;

float dt;
float t0;
char *units=NULL;
char *data_type=NULL;
char *data_title=NULL;

/*
 *
 * This flag will 1 if data in netcdf file 
 * uncapable to divide by pieces > 2*F_LEN
 *
 */
char short_data_flag=0;


WSFILTER_TYPE FType=WSFILTER_LOWPASS;

char *input_file_name=NULL;
char *output_file_name=NULL;


Module_parameters parms={0,NULL,0,NULL};

