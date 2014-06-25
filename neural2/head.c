/*
 *  This file is a part of the PARUS project.
 *  Copyright (C) 2006  Alexey N. Salnikov, Alexander S. Ivanov
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
 * Alexander S. Ivanov (alex@angel.cs.msu.su)
 *
 */

#include <stdio.h>
#include <string.h>

#define num_ent 10
#define num_proc 10
#define num_layers 3
#define num_res 1

int get_l(void)
{
  return (px_node_number-1)/num_proc+1;
}

int get_i(void)
{
  return (px_node_number-2)%num_proc+1;
}

int get_q(void)
{
  return num_ent/num_proc;
}

int get_r(void)
{
  return num_ent%num_proc;
}

int get_wt_start(void)
{
  if (get_i()-1<get_r())
	return (get_q()+1)*(get_i()-1)*num_proc;
  else 
	return ((get_q()+1)*get_r()+get_q()*(get_i()-get_r()-1))*num_proc;
}

int get_wt_end(void)
{
  if (get_i()-1<get_r())
	return (get_q()+1)*(get_i()-1)*num_proc;
  else 
	return ((get_q()+1)*get_r()+get_q()*(get_i()-get_r()))*num_proc;
}
