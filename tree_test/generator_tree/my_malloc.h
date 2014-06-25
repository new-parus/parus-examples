#ifndef __MY_MALLOC_H__
#define __MY_MALLOC_H__

#include <stdlib.h>

extern void *__my_malloc(size_t size);
extern void __my_free(void *pointer);

#define malloc __my_malloc
#define free __my_free


#endif /* __MY_MALLOC_H__ */