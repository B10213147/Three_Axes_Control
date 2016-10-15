/*
 * temperature_sensor.c
 *
 *  Created on: Oct 15, 2016
 *      Author: Harvard Tseng
 */

#include "temperature_sensor.h"
#include "rtos.h"
#include "TM4C123GH6PM.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

int num2str(int x, char* str);

struct rtos_pipe *tempsensor_Fifo;

void tempsensor_driver(void){
	uint32_t pui32ADC0Value[1];

    //
    // Trigger the ADC conversion.
    //
    ADCProcessorTrigger(ADC0_BASE, 3);

    //
    // Wait for conversion to be completed.
    //
    while(!ADCIntStatus(ADC0_BASE, 3, false))
    {
    }

    //
    // Clear the ADC interrupt flag.
    //
    ADCIntClear(ADC0_BASE, 3);

    //
    // Read ADC Value.
    //
    ADCSequenceDataGet(ADC0_BASE, 3, pui32ADC0Value);

    //
    // Use non-calibrated conversion provided in the data sheet.  Make
    // sure you divide last to avoid dropout.
    //
    uint32_t ui32TempValueC = ((1475 * 4096) - (2475 * pui32ADC0Value[0])) / 410;

    int digit;
    char temp[5];
    digit = num2str(ui32TempValueC, temp);

    char line1[6];
    for(int i=0; digit-i > 0; i++){
        line1[i] = temp[digit-i];
    }
    line1[4] = line1[3];
    line1[3] = line1[2];
    line1[2] = '.';
    rtos_pipe_write(tempsensor_Fifo, line1, 5);
}

void tempsensor_init(void){
    //
    // The ADC0 peripheral must be enabled for use.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    //
    // Enable sample sequence 3 with a processor signal trigger.
    //
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_TS | ADC_CTL_IE |
                             ADC_CTL_END);

    //
    // Since sample sequence 3 is now configured, it must be enabled.
    //
    ADCSequenceEnable(ADC0_BASE, 3);

    //
    // Clear the interrupt status flag.  This is done to make sure the
    // interrupt flag is cleared before we sample.
    //
    ADCIntClear(ADC0_BASE, 3);

    tempsensor_Fifo = rtos_pipe_create(10);
}

int num2str(int x, char* str){
    int digit, remainder;

    for(digit=1; x>0 && digit<16; digit++){
        remainder = x%10;
        str[digit] = remainder + 48;
        x /= 10;
    }

    return digit-1;
}
