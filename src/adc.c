#include "definitions.h" 

uint8_t adc_cmd;

void ADC_Wait(){
    while(ADC_RDY_Get());
}

void ADC_Initialize(){
    adc_cmd = MODE;
    
    SS_ADC_Clear(); 
    SPI1_Write(&adc_cmd, 1);
    ADC_Wait();
    
    adc_cmd = MODE_Continuous;
    
    SPI1_Write(&adc_cmd, 1);
    ADC_Wait();
    SS_ADC_Set();
}

void ADC_Select_Chnl(uint8_t chnl){
    adc_cmd = CONTROL;
    
    SS_ADC_Clear();
    SPI1_Write(&adc_cmd, 1);
    ADC_Wait();
    
    adc_cmd = (chnl<<4) + 0b1000 + Input_Range;
    
    SPI1_Write(&adc_cmd, 1);
    ADC_Wait();
    SS_ADC_Set();
}

void ADC_Read_Data(uint8_t* RxData){
    adc_cmd = DATA;
    
    SS_ADC_Clear();
    SPI1_Write(&adc_cmd, 1);
    ADC_Wait();
    SS_ADC_Set();

    SS_ADC_Clear();
    SPI1_Read(RxData, 3);
    ADC_Wait();
    SS_ADC_Set();
}





