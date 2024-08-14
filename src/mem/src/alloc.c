#include "alloc.h"
#if defined(HAVE_JEMALLOC)
#include "jemalloc.h"
#else
#if defined(_MSC_VER)
#include <crtdbg.h>
#endif
#include <stdlib.h>

#define je_malloc(x)		malloc(x)
#define je_realloc(x, s)		realloc(x, s)
#define je_free(x)			free(x)

#endif

#define REALLOC_SIZE(s) (((s)+16)/8*13)

void* grow( void* mem, size_t len, size_t * alloc ) {
  if( len <= *alloc )
    return mem;
  
  len = REALLOC_SIZE(len);
  if( mem == NULL || *alloc == 0 )
    mem = je_malloc( len );
  else
    mem = je_realloc( mem, len );

  if( mem )
    *alloc = je_malloc_usable_size(mem);
  else
    *alloc = 0;
  
  return mem;
}

void* allocator_alloc( allocator_t* alloc, size_t len ) {
  return je_malloc( len );
}

void* allocator_realloc( allocator_t* alloc, void* ptr, size_t len ) {
  return je_realloc( ptr, len );
}

void  allocator_free( allocator_t* alloc, void* ptr ) {
  je_free( ptr );
}

allocator_t* allocator_replace( allocator_t* alloc ) {
  return NULL;
}
