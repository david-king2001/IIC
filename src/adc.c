#include "definitions.h" 
#include "adc.h"
#include "input_output.h"

//!Local varaible to store what commend to send to ADC
uint8_t adc_cmd;
uint8_t trash = 0;
bool converting = false;
//!Blocking until ADC busy signal is low
//void ADC_Wait() {
//    while (ADC_RDY_Get());
//}

//See adc.h
void ADC_Initialize() {
    adc_cmd = WRITE + MODE;
    uint32_t rst = -1;
    
    SS_ADC_Clear();
    SPI1_Write((uint8_t*)&rst, 4);
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
    
    if (converting){
        return;
    }
    converting = true;
    adc_cmd = WRITE+CONTROL;

    SS_ADC_Clear();
    SPI1_Write(&adc_cmd, 1);
    while (SPI1_IsBusy());

    adc_cmd = (chnl<<4) + 0b1000 + Input_Range;

    SPI1_Write(&adc_cmd, 1);
    while (SPI1_IsBusy());
    SS_ADC_Set();
    
}

//See adc.h
void ADC_Read_Data(uint8_t* RxData) {   
    
    if (ADC_RDY_Get())return;
    
    converting = false;
    adc_cmd = READ+DATA;
    
    
    SS_ADC_Clear();
    SPI1_Write(&adc_cmd, 1);
    while (SPI1_IsBusy());
    
    SPI1_Read(RxData+2, 1);
    while (SPI1_IsBusy());
    SPI1_Read(RxData+1, 1);
    while (SPI1_IsBusy());
    SPI1_Read(RxData, 1);
    while (SPI1_IsBusy());
    SS_ADC_Set();
}

uint8_t ADC_Read_Registor(uint8_t reg){
    uint8_t out = 0;
    adc_cmd = READ + reg;
    
    SS_ADC_Clear();
    SPI1_Write(&adc_cmd, 1);
    while(SPI1_IsBusy());
    
    SPI1_Read(&out, 1);
    while(SPI1_IsBusy());
    SPI1_Read(&trash, 1);
    while(SPI1_IsBusy());
    
    SS_ADC_Set(); 
    
    return out;
}





