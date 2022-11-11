#include "definitions.h" 

//!Local varaible to store what commend to send to DAC
uint8_t adc_cmd;

//!Blocking until ADC busy signal is low
void ADC_Wait() {
    while (ADC_RDY_Get());
    for (int i=0; i<1000; i++);
}

//See adc.h
void ADC_Initialize() {
    adc_cmd = MODE;

    SS_ADC_Clear();
    SPI1_Write(&adc_cmd, 1);
    while (SPI1_IsBusy());
    ADC_Wait();

    adc_cmd = MODE_Continuous;

    SPI1_Write(&adc_cmd, 1);
    while (SPI1_IsBusy());
    ADC_Wait();
    SS_ADC_Set();
}

//See adc.h
void ADC_Select_Chnl(uint8_t chnl) {
    adc_cmd = CONTROL;

    SS_ADC_Clear();
    
    SPI1_Write(&adc_cmd, 1);
    while (SPI1_IsBusy());
    ADC_Wait();

    adc_cmd = (chnl << 4) + 0b1000 + Input_Range;

    SPI1_Write(&adc_cmd, 1);
    while (SPI1_IsBusy());
    ADC_Wait();
    SS_ADC_Set();
}

//See adc.h
void ADC_Read_Data(uint32_t* RxData) {
    adc_cmd = DATA;

    SS_ADC_Clear();
    SPI1_Write(&adc_cmd, 1);
    while (SPI1_IsBusy());
    ADC_Wait();
    SPI1_Read(RxData, 3);
    while (SPI1_IsBusy());
    ADC_Wait();
    SS_ADC_Set();
}





