#include "definitions.h" 
#include "terminal_input_output.h"
#include "input_output.h"
#include "adc.h"
#include <string.h>
#include <stdio.h>

bool enter = false;
char rec_char = ' ';
char buffer[OUT_BUFFER] = {};
char receive_buffer[128] = {};
int receive_buff_size = 0;
int nbytes = 0;
uint8_t reg_data = 0;
uint8_t cmd = 0;
char input_instructions[6][256] = {"To edit an alarm first character must be \'A\'.\r\n\033[0K",
                                "First character must be \'I\' input conf, \'A\' alarm conf,\'O\' alarm conf \'D\' display\r\n\033[0K",
                                "Ex A in_type:A.D input:0-4 out_type:A or R output:0-7 alarm:0-4 high/low:low trigger:5432.1 reset:1234.5\r\n\033[0K",
                                "Ex I type:A input:0 max:5432.1 min:1234.5\r\n\033[0K",
                                "Ex O input:0-4 output:0-1 max:5432.1 min:1234.5\r\n\033[0K",
                                "D A for alarms, D H for input log\r\n\033[0K"};

    
void Terminal_Initialize(){
    //Clear the terminal
    UART4_Write(&"\033[2J", sizeof("\033[2J"));
    while (UART4_WriteIsBusy()); 
    UART4_Write(&"\033[0;0H", sizeof("\033[0;0H"));
    while (UART4_WriteIsBusy());
}

void PrintInstructions(){
    for (int i=0; i<6; i++){
        nbytes = sprintf(buffer, input_instructions[i]);
        while (UART4_WriteIsBusy()); 
        UART4_Write(&buffer, nbytes);
        while (UART4_WriteIsBusy());        
    }
}


void PrintRegister(uint8_t reg){
    reg_data = ADC_Read_Registor(reg);
    switch(reg){
        case MODE:
            nbytes = sprintf(buffer, "MODE: ");
        break;
        case ID:
            nbytes = sprintf(buffer, "ID: ");
        break;
        case STATUS:
            nbytes = sprintf(buffer, "STATUS: ");
        break;
        case CONTROL:
            nbytes = sprintf(buffer, "CONTROL: ");
        break;
        case FILTER:
            nbytes = sprintf(buffer, "FILTER: ");
        break;
    }
    nbytes += sprintf(buffer+nbytes, "%x  %u%u%u%u%u%u%u%u     \r\n\033[0K", reg_data, 
            (reg_data >> 7) & 1,
            (reg_data >> 6) & 1,
            (reg_data >> 5) & 1,
            (reg_data >> 4) & 1,
            (reg_data >> 3) & 1,
            (reg_data >> 2) & 1,
            (reg_data >> 1) & 1,
            reg_data & 1);
    while (UART4_WriteIsBusy()); 
    UART4_Write(&buffer, nbytes);
    while (UART4_WriteIsBusy());
}

void PrintAnalogInputs(){
    //Print the values of each input
    for (int i=0; i<4; i++){
        nbytes = sprintf(buffer, "%s Analog Input #%d %lf          \r\n\033[0K",inputs[i].is_set ? "Set" : "Not Set", i, inputs[i].scaled_data);
        while (UART4_WriteIsBusy()); 
        UART4_Write(&buffer, nbytes);
        while (UART4_WriteIsBusy());
        
        nbytes = sprintf(buffer, "%s Analog Input RAW #%d %u          \r\n\033[0K",inputs[i].is_set ? "Set" : "Not Set", i, inputs[i].raw_data);
        while (UART4_WriteIsBusy()); 
        UART4_Write(&buffer, nbytes);
        while (UART4_WriteIsBusy());
        
        nbytes = sprintf(buffer, "Max:%lf  Min:%lf\r\n\r\n\033[0K",inputs[i].max, inputs[i].min);
        while (UART4_WriteIsBusy()); 
        UART4_Write(&buffer, nbytes);
        while (UART4_WriteIsBusy());
    }
}

void PrintDigitalInputs(){
    for (int i=4; i<8; i++){
        nbytes = sprintf(buffer, "%s Digital Input #%d %s\r\n\033[0K",inputs[i].is_set ? "Set" :"Not Set" , i-4, inputs[i].digital_on ? "on " : "off");
        while (UART4_WriteIsBusy()); 
        UART4_Write(&buffer, nbytes);
        while (UART4_WriteIsBusy());
    }
}

void PrintAnalogOutputs(){
    for (int i=0; i<2; i++){
        nbytes = sprintf(buffer, "Analog Outputs #%d %u\r\n\033[0K",i, outputs[i].data);
        while (UART4_WriteIsBusy()); 
        UART4_Write(&buffer, nbytes);
        while (UART4_WriteIsBusy());
        nbytes = sprintf(buffer, "Source I#%d max:%lf min:%lf\r\n\033[0K",outputs[i].input_chnl, outputs[i].trigger, outputs[i].reset);
        while (UART4_WriteIsBusy()); 
        UART4_Write(&buffer, nbytes);
        while (UART4_WriteIsBusy());
    }
    nbytes = sprintf(buffer,"\r\n\033[0K");
    while (UART4_WriteIsBusy());
    UART4_Write(&buffer, nbytes);
    while (UART4_WriteIsBusy());
}

void PrintRelays(){
    for (int i=2; i<10; i++){
        nbytes = sprintf(buffer, "Relay States #%d %u\r\n\033[0K",i-2, outputs[i].relay);
        while (UART4_WriteIsBusy()); 
        UART4_Write(&buffer, nbytes);
        while (UART4_WriteIsBusy());
    }
    nbytes = sprintf(buffer,"\r\n\033[0K");
    while (UART4_WriteIsBusy());
    UART4_Write(&buffer, nbytes);
    while (UART4_WriteIsBusy());
}

void PrintAlarmSettings(){
                        //Print the alarms currently set for each input
                    for (int i=0; i<8; i++){
                        nbytes = sprintf(buffer, "Input #%d alarms:\r\n\033[0K", i);
                        while (UART4_WriteIsBusy());
                        UART4_Write(&buffer, nbytes);
                        while (UART4_WriteIsBusy());
                        for (int j=0; j<4; j++){
                            if (inputs[i].alrms[j] != NULL && inputs[i].alrms[j]->input_chnl != -1) {
                                nbytes = sprintf(buffer, "Alarm #%d on/off: %s, high/low:%s, trigger:%f, reset:%f\r\n\033[0K",
                                                                j, inputs[i].alrms[j]->relay ? "On" : "Off",
                                                                inputs[i].alrms[j]->high_low ? "High-Low" : "Low-High",
                                                                inputs[i].alrms[j]->trigger, inputs[i].alrms[j]->reset);
                                while (UART4_WriteIsBusy());
                                UART4_Write(&buffer, nbytes);
                                while (UART4_WriteIsBusy());
                            }else{
                                nbytes = sprintf(buffer,"Alarm #%d not used\r\n\033[0K", j);
                                while (UART4_WriteIsBusy());
                                UART4_Write(&buffer, nbytes);
                                while (UART4_WriteIsBusy());
                            }
                        }
                        nbytes = sprintf(buffer,"\r\n\033[0K");
                        while (UART4_WriteIsBusy());
                        UART4_Write(&buffer, nbytes);
                        while (UART4_WriteIsBusy());
                    }
                    
                    

}

void PrintHistory(){
    for(int i=0; i<4; i++){
        nbytes = sprintf(buffer, "Input %u",i);
        
        for (int j=0; j<10; j++){
            nbytes+=sprintf(buffer+nbytes, ",%lf", pastData[i][j]);
        }
        nbytes+=sprintf(buffer+nbytes, "\r\n\033[0K");
        UART4_Write(&buffer, nbytes);
        while (UART4_WriteIsBusy());
    }
}


void ParseInputForAlarm(char* input_string){
    int alarm_num;
    int input_num;
    int output_num;
    char in_type;
    char out_type;
    char hl_str[5];
    double trigger, reset;

    sscanf(input_string, "in_type:%c input:%d out_type:%c output:%d alarm:%d high/low:%s trigger:%lf reset:%lf", 
        &in_type, &input_num, &out_type, &output_num, &alarm_num, hl_str, &trigger, &reset);
        

    // Check the input number
    if (input_num < 0 || input_num >= 4) {
        nbytes = sprintf(buffer, "Error: Invalid input number\r\n");
        while (UART4_WriteIsBusy()); 
        UART4_Write(&buffer, nbytes);
        while (UART4_WriteIsBusy());
        return;
    }
    
    // Check the alarm number    
    if (alarm_num < 0 || alarm_num >= 4) {
        nbytes = sprintf(buffer, "Error: Invalid alarm number\r\n");
        while (UART4_WriteIsBusy()); 
        UART4_Write(&buffer, nbytes);
        while (UART4_WriteIsBusy());
        return;
    }
    
    // Check the type string
    if (in_type != 'A' && in_type != 'D'){
        nbytes = sprintf(buffer, "Error: Invalid input type\r\n");
        while (UART4_WriteIsBusy()); 
        UART4_Write(&buffer, nbytes);
        while (UART4_WriteIsBusy());
        return;
    }
    
        // Check the type string
    if (out_type == 'A' && in_type != 'R'){
        nbytes = sprintf(buffer, "Error: Invalid input type\r\n");
        while (UART4_WriteIsBusy()); 
        UART4_Write(&buffer, nbytes);
        while (UART4_WriteIsBusy());
        return;
    }

    

    // Check the high/low string
    if (strcmp(hl_str, "high") != 0 && strcmp(hl_str, "low") != 0){
        nbytes = sprintf(buffer, "Error: Invalid high/low type\r\n");
        while (UART4_WriteIsBusy()); 
        UART4_Write(&buffer, nbytes);
        while (UART4_WriteIsBusy());
        return;
    }
            // Check the type string
    if (strcmp(hl_str, "high")== 0){
        if (trigger<reset){
            nbytes = sprintf(buffer, "Error: Invalid alarm range\r\n");
            while (UART4_WriteIsBusy()); 
            UART4_Write(&buffer, nbytes);
            while (UART4_WriteIsBusy());
            return;            
        }
    }else{
        if (trigger>reset){
            nbytes = sprintf(buffer, "Error: Invalid alarm range\r\n");
            while (UART4_WriteIsBusy()); 
            UART4_Write(&buffer, nbytes);
            while (UART4_WriteIsBusy());
            return;            
        }
    }
    
    
    EditAlarm(&outputs[out_type == 'A' ? output_num : output_num+2], &inputs[in_type == 'A' ? input_num : input_num+4], trigger, reset, in_type == 'A' ? input_num : input_num+4, alarm_num, strcmp(hl_str, "high") == 0);

}

void ParseInputForInput(char* input_string){
    int input_num;
    double high, low = 0.0;
    char type_str[2] = {};
    
    sscanf(input_string, "type:%s input:%d max:%lf min:%lf", 
        type_str, &input_num, &high, &low);
        
    // Check the input number
    if (input_num < 0 || input_num >= 4) {
        nbytes = sprintf(buffer, "Error: Invalid input number\r\n\033[0K");
        while (UART4_WriteIsBusy()); 
        UART4_Write(&buffer, nbytes);
        while (UART4_WriteIsBusy());
        return;
    }
    
        // Check the type string
    if (strcmp(type_str, "A") != 0 && strcmp(type_str, "D") != 0){
        nbytes = sprintf(buffer, "Error: Invalid input type string\r\n\033[0K");
        while (UART4_WriteIsBusy()); 
        UART4_Write(&buffer, nbytes);
        while (UART4_WriteIsBusy());
        return;
    }
    
    if(high<low){
        nbytes = sprintf(buffer, "Error: Invalid input high low\r\n\033[0K");
        while (UART4_WriteIsBusy()); 
        UART4_Write(&buffer, nbytes);
        while (UART4_WriteIsBusy());
        return;
    }
    
    ConfigureInput(&inputs[strcmp(type_str, "A") == 0 ? input_num : input_num+4], type_str[0]=='A', high, low, strcmp(type_str, "A") == 0 ? input_num : input_num+4);
}

void ParseInputForOutput(char* input_string){
    int input_num;
    int output_num;
    double max, min;

    sscanf(input_string, "input:%d output:%d max:%lf min:%lf", 
        &input_num, &output_num, &max, &min);
    
        // Check the input number
    if (input_num < 0 || input_num >= 4) {
        nbytes = sprintf(buffer, "Error: Invalid input number\r\n\033[0K");
        while (UART4_WriteIsBusy()); 
        UART4_Write(&buffer, nbytes);
        while (UART4_WriteIsBusy());
        return;
    }
    
    if (input_num < 0 || input_num >= 4) {
        nbytes = sprintf(buffer, "Error: Invalid input number\r\n\033[0K");
        while (UART4_WriteIsBusy()); 
        UART4_Write(&buffer, nbytes);
        while (UART4_WriteIsBusy());
        return;
    }
    
    if (!ConfigureAnalogOutput(&outputs[output_num], &inputs[input_num], input_num, max, min )){
        nbytes = sprintf(buffer, "Error: \r\n\033[0K");
        while (UART4_WriteIsBusy()); 
        UART4_Write(&buffer, nbytes);
        while (UART4_WriteIsBusy());
        return;
    }
    
}