
#ifndef ADD_ON_FEATURES_H_
#define ADD_ON_FEATURES_H_

#include "regulation_drv.h"

/**
 * Enable or disable various add on features here.  Implementation in add_on_features.c
 */
#define en_ishare

void add_on_features_init (void);

//venkat addition
#ifdef en_ishare
void added_droop_disable(PMBUS_PAGE_t loop);
void added_droop_enable(PMBUS_PAGE_t loop);
void remove_added_droop_irq_callback(PMBUS_PAGE_t loop);

void enable_ishare(PMBUS_PAGE_t loop);
void disable_ishare(PMBUS_PAGE_t loop);
void calculate_droop(PMBUS_PAGE_t loop);																		//Punit Edit
void patch_Regulation_Shutdown_Sequence (PMBUS_PAGE_t loop, REGULATION_STATE state, REGULATION_CONTROLS control);
#endif


#endif
