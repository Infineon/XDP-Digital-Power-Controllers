/*
 * patch_pmbus_fifo.h
 *
 *  Created on: Jul 18, 2023
 *      Author: dlewis4
 */

#ifndef SRC_PATCH_PMBUS_FIFO_H_
#define SRC_PATCH_PMBUS_FIFO_H_

#define JIRA_972_I2C_FIFO
#define JIRA_973_I2C_FIFO

#ifdef JIRA_973_I2C_FIFO
	void patch_pmbus_fifo(void);
#endif

#endif /* SRC_PATCH_PMBUS_FIFO_H_ */
