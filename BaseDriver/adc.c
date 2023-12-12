/*
 * adc.c
 *
 *  Created on: Oct 11, 2023
 *      Author: PK6300
 */

#include "S32K142.h"
#include "ADC.h"

uint16_t ADC_results[NUM_ADC];
uint16_t ADC_filtered[NUM_ADC];
int32_t ADC_filter_state[NUM_ADC];

void ADC_init(int pcc_adc_index, ADC_Type *adc) {
 PCC->PCCn[pcc_adc_index] &= ~PCC_PCCn_CGC_MASK; /* Disable clock to change PCS */
 PCC->PCCn[pcc_adc_index] &= ~PCC_PCCn_PCS_MASK; // clear bits in the clock source selection part of register
// PCC->PCCn[pcc_adc_index] |= PCC_PCCn_PCS(1); /* PCS=1: Select SOSCDIV2 */      // <- 5uS
// PCC->PCCn[pcc_adc_index] |= PCC_PCCn_PCS(1); /* PCS=3: Select SOSCDIV2_CLK */  // <- 5uS
// PCC->PCCn[pcc_adc_index] |= PCC_PCCn_PCS(2); /* PCS=3: Select SIRCDIV2_CLK */  // <- 5uS
 PCC->PCCn[pcc_adc_index] |= PCC_PCCn_PCS(3); /* PCS=3: Select FIRCDIV2_CLK */  // <- 2uS
// PCC->PCCn[pcc_adc_index] |= PCC_PCCn_PCS(6); /* PCS=6: Select SPLLDIV2_CLK */  // <- 2uS
 PCC->PCCn[pcc_adc_index] |= PCC_PCCn_CGC_MASK; /* Enable bus clock in ADC */

 adc->SC1[0] =0x00001F;	 /* ADCH=1F: Module is disabled for conversions*/
 	 	 	 	 	 	 /* AIEN=0: Interrupts are disabled */

 adc->CFG1 = 0x000000004; /* ADICLK=0: Input clk=ALTCLK1 = See above PCC */
 	 	 	 	 	 	  /* ADIV=0: Prescaler=1 */
 	 	 	 	 	 	  /* MODE=1: 12-bit conversion */

// adc->CFG2 = 0x00000000C; /* SMPLTS=12(default): sample time is 13 ADC clks */
 adc->CFG2 = 3; /* SMPLTS=12(default): sample time is 3+1 = 4 ADC clks */

 adc->SC2 = 0x00000000;	 /* ADTRG=0: SW trigger */
 	 	 	 	 	 	 /* ACFE,ACFGT,ACREN=0: Compare func disabled */
 	 	 	 	 	 	 /* DMAEN=0: DMA disabled */
 	 	 	 	 	 	 /* REFSEL=0: Voltage reference pins= VREFH, VREEFL */

 adc->SC3 = 0x00000000;  /* CAL=0: Do not start calibration sequence */
 	 	 	 	 	 	 /* ADCO=0: One conversion performed */
 	 	 	 	 	 	 /* AVGE,AVGS=0: HW average function disabled */
}
/*
void convertAdcChan(ADC_Type *adc, uint16_t adcChan) { // For SW trigger mode, SC1[0] is used
 adc->SC1[0] &= ~ADC_SC1_ADCH_MASK; // Clear prior ADCH bits
 adc->SC1[0] = ADC_SC1_ADCH(adcChan); // Initiate Conversion
}
*/
/*
uint8_t adc_complete(ADC_Type *adc) {
 return ((adc->SC1[0] & ADC_SC1_COCO_MASK)>>ADC_SC1_COCO_SHIFT); // Wait for completion
}
*/
/*
uint32_t read_adc_ch(ADC_Type *adc, int chan) {
 uint16_t adc_result = adc->R[chan]; // For SW trigger mode, R[0] is used
 return (uint32_t) (adc_result);
}
*/
