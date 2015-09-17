/* Replace: malloc() and free().
   Description: simplified memory management over static buffer without
		releasing of allocated memory. MALLOC_BUF_SIZE defines
		size of static buffer (default value is 8k).
*/

#include <malloc.h>

#ifndef MALLOC_BUF_SIZE
# define MALLOC_BUF_SIZE (8u*1024u)
#endif

void *malloc (size_t sz) {
	static char malloc_buffer [MALLOC_BUF_SIZE];
	static size_t malloc_next = 0;

	size_t old = malloc_next;
	if (sizeof malloc_buffer - old < sz)
		return NULL;

	malloc_next += sz;
	return malloc_buffer + old;
}

#ifdef __WATCOMC__
# if defined __TINY__ || defined __SMALL__ || defined __MEDIUM__
void _WCNEAR *_nmalloc (size_t sz) { return malloc (sz); }
# else /* defined __COMPACT__ || defined __LARGE__ || defined __HUGE__ */
void _WCNEAR *_nmalloc (size_t sz) { (void)sz; return NULL; }
# endif
void *realloc (void *p, size_t sz) { (void)p; (void)sz; return NULL; }
#endif

#ifdef __cplusplus
void free (void*) {}
#else
void free () {}
#endif
