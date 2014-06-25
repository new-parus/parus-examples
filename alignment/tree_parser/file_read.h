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
* This file contents the description of class that       *
* relises the access to the text file and counts the     *
* numbers of strings and columns                         *
*                                                        *
* Written by Alexey Salnikov in september 2003           *
*                                                        *
**********************************************************  
*/

#ifndef __FILE_READ_H__
#define __FILE_READ_H__

#include <stdio.h>

namespace parus
{
 /**
  Class that relises the access to the text file and 
  counts the numbers of strings and columns.
 */
 class TXT_File
 {
  protected:
   FILE *f;
   long int strings;
   long int columns;
  public:
   TXT_File();
   ~TXT_File();
   /**
         
    Open file by it's name in operation system. 
   
    */
   int open(const char *file_name);
   /** Read one symbol from file */
   int getc(void);
   /** 
     Return number of the read strings from file.
     The numeration begins from 1.  
    */
   inline long int current_string(void) { return strings+1; };
   /**
     Return number of the read columns from file.
     It begins from 1.
    */
   inline long int current_column(void) { return columns; }; 
   
 };
}


#endif /* __FILE_READ_H__ */
