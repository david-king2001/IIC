#include "definitions.h" 

//!Local varaible to store what commend to send to DAC
uint8_t adc_cmd;

//!Blocking until ADC busy signal is low
void ADC_Wait() {
    while (ADC_RDY_Get());
}

//!Initialize ADC at power on
/*!
 * Send command to ADC Communication Register to select MODE register\n
 * Send command to ADC MODE Register to select Continuous Conversion mode
 */
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

//!Selects Channel
/*!
 * Send command to ADC Communication Register to select Control register\n
 * Send command to ADC Control Register to select channel
 * \param chnl a integer with the selected input channel
 */
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

//!Reads data from ADC
/*!
 * Send command to ADC Communication Register to select data register\n
 * Read from ADC data Register into input buffer
 * \param RxData a pointer to start of buffer that stores that channels data
 */
void ADC_Read_Data(uint8_t* RxData) {
    adc_cmd = DATA;

    SS_ADC_Clear();
    SPI1_Write(&adc_cmd, 1);
    while (SPI1_IsBusy());
    ADC_Wait();
    SS_ADC_Set();

    SS_ADC_Clear();
    SPI1_Read(RxData, 3);
    while (SPI1_IsBusy());
    ADC_Wait();
    SS_ADC_Set();
}





