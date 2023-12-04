/*
 * peripheral_init.h
 *
 *  Created on: Sep 29, 2023
 *      Author: PK6300
 */

#ifndef PERIPHERAL_INIT_H_
#define PERIPHERAL_INIT_H_

void peripheral_init(void);
void Motor_Timer_Init(int pcc_ftm_index, FTM_Type *ftm);

#endif /* PERIPHERAL_INIT_H_ */
