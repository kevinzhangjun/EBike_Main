/******************************************************************
**	  E_Bike
**	  ADC
**
**    Versiomn: V1.0
**	  Author  : Kevin Zhang
********************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "Adc.h"

Idt_ADC_T ADC_ST;

/* Flag used to store if an ADC PAL conversion group has finished executing */
volatile bool ADC0ConvDone = false;


/* Variable to store value from ADC conversion */
volatile uint16_t adcRawValue;

void Init_ADC(void)
{
    /* Write your local variable definition here */
    status_t status;


    adc_resolution_t resolution;

    /* Get ADC max value from the resolution configured in extension field */
    resolution = ((extension_adc_s32k1xx_t *)(adc_pal_1_config.extension))->resolution;

    if (resolution == ADC_RESOLUTION_8BIT)
        ADC_ST.ADC_MAX = (uint16_t) (1 << 8);
    else if (resolution == ADC_RESOLUTION_10BIT)
    	ADC_ST.ADC_MAX = (uint16_t) (1 << 10);
    else
    	ADC_ST.ADC_MAX = (uint16_t) (1 << 12);

    DEV_ASSERT(adc_pal_1_instance.instIdx == ADC0_INSTANCE);
    status = ADC_Init(&adc_pal_1_instance, &adc_pal_1_config);
    DEV_ASSERT(status == STATUS_SUCCESS);

    /***************************************************************
     * Part 1 of the example: software triggered conversion group
     ***************************************************************/
    ADC_Start_Group(0);
    ADC_ST.ADC_4ms_Flg = false;
}

void ADC_Start_Group(uint8_t selectedGroupIndex)
{
    status_t status;

    /* Start the selected SW triggered group of conversions */
    status = ADC_StartGroupConversion(&adc_pal_1_instance, selectedGroupIndex);
    DEV_ASSERT(status == STATUS_SUCCESS);
}



