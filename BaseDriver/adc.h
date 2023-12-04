/*
 * adc.h
 *
 *  Created on: Oct 11, 2023
 *      Author: PK6300
 */

#ifndef ADC_H_
#define ADC_H_

void ADC_init(int pcc_adc_index, ADC_Type *adc);

#define NUM_ADC 9
extern uint16_t ADC_results[NUM_ADC];
extern uint16_t ADC_filtered[NUM_ADC];
extern int32_t ADC_filter_state[NUM_ADC];

#define convertAdcChan(adc, adcChan) { adc->SC1[0] &= ~ADC_SC1_ADCH_MASK; adc->SC1[0] = ADC_SC1_ADCH(adcChan); }

#define adc_complete(adc) ((adc->SC1[0] & ADC_SC1_COCO_MASK)>>ADC_SC1_COCO_SHIFT)

#define read_adc_ch(adc, chan) (adc->R[chan])


#endif /* ADC_H_ */
