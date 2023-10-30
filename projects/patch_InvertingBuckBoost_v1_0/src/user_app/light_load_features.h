#ifndef SRC_USER_APP_LIGHT_LOAD_FEATURES_H_
#define SRC_USER_APP_LIGHT_LOAD_FEATURES_H_


void light_load_features_threshold_irq_handle_enable(void);
void light_load_features_threshold_irq_handle_disable(void);
void light_load_features_threshold_irq_handle(PMBUS_PAGE_t loop);

void light_load_features_enable(mode_t mode);

void PMBUS_HANDLE_MFR_LIGHT_LOAD_THRESH(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);
void PMBUS_HANDLE_MFR_LIGHT_LOAD_MODE(PMBUS_PAGE_t page, PMBUS_DIRECTION_e direction);

#endif /* SRC_USER_APP_LIGHT_LOAD_FEATURES_H_ */
