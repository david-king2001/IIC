#include "definitions.h" 
#include "terminal_input_output.h"
#include "input_output.h"
#include "adc.h"
#include <string.h>
#include <stdio.h>

bool enter = false;
char rec_char = ' ';
char buffer[512] = {};
char receive_buffer[128] = {};
int receive_buff_size = 0;
int nbytes = 0;
uint8_t reg_data = 0;
uint8_t cmd = 0;

    
void Terminal_Initialize(){
    //Clear the terminal
    UART1_Write(&"\033[2J", sizeof("\033[2J"));
    while (UART1_WriteIsBusy()); 
    UART1_Write(&"\033[0;0H", sizeof("\033[0;0H"));
    while (UART1_WriteIsBusy());
}


void PrintRegister(uint8_t reg){
    cmd = READ + reg;
    SS_ADC_Clear();
    SPI1_Write(&cmd, 1);
    while(SPI1_IsBusy());

    SPI1_Read(&reg_data, 1);
    while(SPI1_IsBusy());
    SS_ADC_Set();    

    nbytes = sprintf(buffer, "%d%d%d%d%d%d%d%d\r\n", (reg_data>>7) & 0x01,(reg_data>>6) & 0x01,(reg_data>>5) & 0x01,(reg_data>>4) & 0x01,(reg_data>>3) & 0x01,(reg_data>>2) & 0x01,(reg_data>>1) & 0x01, reg_data & 0x01 );
    while (UART1_WriteIsBusy()); 
    UART1_Write(&buffer, nbytes);
    while (UART1_WriteIsBusy());
}

void PrintAnalogInputs(){
    //Print the values of each input
    for (int i=0; i<4; i++){
        nbytes = sprintf(buffer, "Analog Input #%d %u\r\n",i, inputs[i].raw_data);
        while (UART1_WriteIsBusy()); 
        UART1_Write(&buffer, nbytes);
        while (UART1_WriteIsBusy());
    }
}

void PrintDigitalInputs(){
    for (int i=4; i<8; i++){
        nbytes = sprintf(buffer, "Digital Input #%d %s\r\n",i-4, inputs[i].digital_on ? "on" : "off");
        while (UART1_WriteIsBusy()); 
        UART1_Write(&buffer, nbytes);
        while (UART1_WriteIsBusy());
    }
}

void PrintAnalogOutputs(){
    for (int i=0; i<2; i++){
        nbytes = sprintf(buffer, "Analog Outputs #%d %u\r\n",i, outputs[i].data);
        while (UART1_WriteIsBusy()); 
        UART1_Write(&buffer, nbytes);
        while (UART1_WriteIsBusy());
    }
}

void PrintRelays(){
    for (int i=2; i<10; i++){
        nbytes = sprintf(buffer, "Analog Outputs #%d %u\r\n",i-2, outputs[i].data);
        while (UART1_WriteIsBusy()); 
        UART1_Write(&buffer, nbytes);
        while (UART1_WriteIsBusy());
    }
}

void ParseInputForAlarm(char* input_string){
    int alarm_num;
    int input_num;
    int output_num;
    char type_str[2];
    char hl_str[5];
    double trigger, reset;

    sscanf(input_string, "type:%s input:%d output:%d alarm:%d high/low:%s trigger:%lf reset:%lf", 
        type_str, &input_num, &output_num, &alarm_num, hl_str, &trigger, &reset);
        
    nbytes = sprintf(buffer, "Function Rec %s\r\n", input_string);
        while (UART1_WriteIsBusy()); 
        UART1_Write(&buffer, nbytes);
        while (UART1_WriteIsBusy());
    
        nbytes = sprintf(buffer, "type:%s input:%d output:%d alarm#:%d high/low:%s trigger:%lf reset:%lf", type_str, input_num, output_num, alarm_num, hl_str, trigger, reset);
        while (UART1_WriteIsBusy()); 
        UART1_Write(&buffer, nbytes);
        while (UART1_WriteIsBusy());
    // Check the input number
    if (input_num < 0 || input_num >= 4) {
        nbytes = sprintf(buffer, "Error: Invalid input number\r\n");
        while (UART1_WriteIsBusy()); 
        UART1_Write(&buffer, nbytes);
        while (UART1_WriteIsBusy());
        return;
    }
    
    // Check the alarm number    
    if (alarm_num < 0 || alarm_num >= 4) {
        nbytes = sprintf(buffer, "Error: Invalid alarm number\r\n");
        while (UART1_WriteIsBusy()); 
        UART1_Write(&buffer, nbytes);
        while (UART1_WriteIsBusy());
        return;
    }
    
    // Check the type string
    if (strcmp(type_str, "A") != 0 && strcmp(type_str, "D") != 0){
        nbytes = sprintf(buffer, "Error: Invalid input type\r\n");
        while (UART1_WriteIsBusy()); 
        UART1_Write(&buffer, nbytes);
        while (UART1_WriteIsBusy());
        return;
    }


    // Check the high/low string
    if (strcmp(hl_str, "high") != 0 && strcmp(hl_str, "low") != 0){
        nbytes = sprintf(buffer, "Error: Invalid high/low type\r\n");
        while (UART1_WriteIsBusy()); 
        UART1_Write(&buffer, nbytes);
        while (UART1_WriteIsBusy());
        return;
    }
    EditAlarm(&outputs[output_num], &inputs[input_num], trigger, reset, input_num, alarm_num, strcmp(hl_str, "high") == 0);

}

void ParseInputForInput(char* input_string){
    int input_num;
    double high, low;
    char type_str[2] = {};
    
    sscanf(input_string, "type:%s input:%d max:%lf min:%lf", 
        type_str, &input_num, &high, &low);
        
    // Check the input number
    if (input_num < 0 || input_num >= 4) {
        printf("Error: Invalid input number\n");
        return;
    }
    
        // Check the type string
    if (strcmp(type_str, "A") != 0 && strcmp(type_str, "D") != 0){
        printf("Error: Invalid type string\n");
        return;
    }
    
    ConfigureInput(&inputs[input_num], type_str[0]=='A', high, low);
}