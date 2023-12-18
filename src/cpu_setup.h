/*
 * cpu_setup.h
 *
 *  Created on: Sep 29, 2023
 *      Author: PK6300
 */

#ifndef CPU_SETUP_H_
#define CPU_SETUP_H_

void CPU_setup(void);
void WDOG_disable (void);
void SOSC_init_8MHz(void);
void SPLL_init_160MHz(void);
void NormalRUNmode_80MHz (void);


#endif /* CPU_SETUP_H_ */
