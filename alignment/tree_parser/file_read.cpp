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
**********************************************************
* This file contents the relise of class that            *
* relises the access to the text file and counts the     *
* numbers of strings and columns                         *
*                                                        *
* Written by Alexey Salnikov in september 2003           *
*                                                        *
**********************************************************  
*/

#include "file_read.h"

#include <stdio.h>
#include <stdlib.h>

using namespace parus;

/*****************************************************************************/
TXT_File::TXT_File()
{
 f=NULL;
 strings=-1;
 columns=-1;
 return;
}
/*****************************************************************************/
TXT_File::~TXT_File()
{
 if(f!=NULL)
 {
  fclose(f);
  f=NULL;
 }
 strings=-1;
 columns=-1;
 return;
}
/****************************************************************************/
int TXT_File::open(const char *file_name)
{
 if(f!=NULL)
 {
  return -2;
 }
 f=fopen(file_name,"r");
 if(f==NULL) return -1;
 strings=0;
 columns=0;
 return 0;
}
/***************************************************************************/
int TXT_File::getc(void)
{
 int car='\r';
 if(f==NULL) return -1;
 while(car=='\r')
 {
  car=fgetc(f);
  if(car==EOF) return -1;
 } 
 if(car=='\n')
 {
  strings++;
  columns=0;
  return car;
 }
 columns++;
 return car; 
}
/*****************************************************************************/
