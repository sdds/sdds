#include <os-ssal/SSW.h>

#include <sdds/sdds_types.h>

#include <stdlib.h>

ssw_rc_t
Memory_init(void) {
    return SDDS_RT_OK;
}

void*
Memory_alloc(size_t size) {
    return malloc(size);
}

void
Memory_free(void* ptr) {
    free(ptr);
}