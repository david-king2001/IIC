#include "definitions.h" 
#include "terminal_input_output.h"

void Terminal_Initialize(){
    errorStatus = false;
    writeStatus = false;
    readStatus = false;
    rec_char = ' ';
    receive_buff_size = 0;
    nbytes = 0;
    reg_data = 0;
    cmd = 0;
    
    //Clear the terminal
    UART1_Write(&"\033[2J", sizeof("\033[2J"));
    while (UART1_WriteIsBusy()); 
    UART1_Write(&"\033[0;0H", sizeof("\033[0;0H"));
    while (UART1_WriteIsBusy());
}


void PrintRegister(uint8_t reg){
    SS_ADC_Clear();
    SPI1_Write(&cmd, 1);
    while(SPI1_IsBusy());

    SPI1_Read(&reg_data, 1);
    while(SPI1_IsBusy());
    SS_ADC_Set();    

    nbytes = sprintf(buffer, "Status: %d%d%d%d%d%d%d%d\r\n", (reg_data>>7) & 0x01,(reg_data>>6) & 0x01,(reg_data>>5) & 0x01,(reg_data>>4) & 0x01,(reg_data>>3) & 0x01,(reg_data>>2) & 0x01,(reg_data>>1) & 0x01, reg_data & 0x01 );
    while (UART1_WriteIsBusy()); 
    UART1_Write(&buffer, nbytes);
    while (UART1_WriteIsBusy());
}

void 
