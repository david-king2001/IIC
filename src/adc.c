#include "definitions.h" 

//!Local varaible to store what commend to send to DAC
uint8_t adc_cmd;

//!Blocking until ADC busy signal is low
void ADC_Wait() {
    while (ADC_RDY_Get());
}

//See adc.h
void ADC_Initialize() {
    adc_cmd = WRITE + MODE;
    uint32_t rst = -1;
    
    SS_ADC_Clear();
    SPI1_Write(&rst, 4);
    while (SPI1_IsBusy());
    SS_ADC_Set();
    
    SS_ADC_Clear();
    SPI1_Write(&adc_cmd, 1);
    while (SPI1_IsBusy());
    
    adc_cmd = MODE_Continuous;
 
    SPI1_Write(&adc_cmd, 1);
    while (SPI1_IsBusy());
    SS_ADC_Set();
    
}

//See adc.h
void ADC_Select_Chnl(uint8_t chnl) {
        
    adc_cmd = WRITE+CONTROL;

    SS_ADC_Clear();
    SPI1_Write(&adc_cmd, 1);
    while (SPI1_IsBusy());

    adc_cmd = 0 + 0b1000 + Input_Range;

    SPI1_Write(&adc_cmd, 1);
    while (SPI1_IsBusy());
    SS_ADC_Set();
}

//See adc.h
void ADC_Read_Data(uint8_t* RxData) {    
    
    adc_cmd = READ+DATA;
    
    SS_ADC_Clear();
    SPI1_Write(&adc_cmd, 1);
    while (SPI1_IsBusy());

    SPI1_Read(RxData, 3);
    while (SPI1_IsBusy());
    SS_ADC_Set();
}





