
#ifndef ADD_ON_FEATURES_H_
#define ADD_ON_FEATURES_H_

#include "regulation_drv.h"
//ADD-ON Features:
//#define pmbus_memory_error

void add_on_features_init (void);
#ifdef pmbus_memory_error
void patch_PMBUS_Copy_From_Memory(PMBUS_PAGE_t loop, const uint8_t *ram_location);
#endif

#endif
