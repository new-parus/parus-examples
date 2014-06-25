#include <stdlib.h>
/*
*************************************************************************
* This is redefined malloc because of some Operation Systems Like AIX	*
* does not allow allocate 0 bytes.					*
*									*
*************************************************************************
*/

char fake_memory; 

void *__my_malloc(size_t size)
{
 if (size==0) return &fake_memory;
 return malloc(size);
}

void __my_free(void *pointer)
{
 if(pointer==&fake_memory) return;
 free(pointer);
 return;
}