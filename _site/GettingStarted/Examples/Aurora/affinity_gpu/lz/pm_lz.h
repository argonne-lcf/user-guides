
#ifndef PM_OPENMP_H
#define PM_OPENMP_H

//#include <sycl/sycl.hpp>
#include <level_zero/ze_api.h>
//#include <sycl/ext/oneapi/backend/level_zero.hpp>

#define _SIZE_GRID 32
#define _SIZE_BLOCK 64

#define _SYCL_CHECK_ERRORS()               \
{                                          \
                                           \
}

extern int dev_num_devices();
extern void dev_properties(int);
extern int dev_check_peer(int, int);

/* extern void dev_set_device(int); */
/* extern int dev_get_device(); */

/* extern void* dev_malloc(int); */
/* extern void* dev_malloc_host(int); */

/* extern void dev_free(void*); */
/* extern void dev_free_host(void*); */

/* extern void dev_push(void*, void*, int); */
/* extern void dev_pull(void*, void*, int); */
/* extern void dev_copy(void*, void*, int); */

#endif

