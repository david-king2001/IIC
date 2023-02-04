/*******************************************************************************
  Main Source File

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include <string.h>
#include "adc.h"
#include "input_output.h"
#include "terminal_input_output.h"
// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

/*!
    \dot
    digraph state_machine{
        rankdir=LR
        node [shape=record, fontname=Helvetica, fontsize=10];
        STATE_INITIALIZE
        ADC_SEND_CMD;
        ADC_READ;
        CHECK_ALARMS;
        TRIGGER_ALARMS;
        DIGITAL_INPUTS_READ;
        DISPLAY;
        USER_INPUT_PERFORM_LOGIC;
        DAC_SEND_DATA;
        INCREMENT_CHANNEL;
            STATE_INITIALIZE -> ADC_SEND_CMD
            ADC_SEND_CMD -> ADC_READ;       
            ADC_READ -> CHECK_ALARMS;
            CHECK_ALARMS -> TRIGGER_ALARMS;
            TRIGGER_ALARMS ->DIGITAL_INPUTS_READ;
            DIGITAL_INPUTS_READ -> DISPLAY;
            DISPLAY -> USER_INPUT_PERFORM_LOGIC;
            USER_INPUT_PERFORM_LOGIC -> DAC_SEND_DATA;
            DAC_SEND_DATA -> INCREMENT_CHANNEL;
            INCREMENT_CHANNEL -> ADC_SEND_CMD;
    }
   \enddot
*/
typedef enum {
    STATE_INITIALIZE, /*!<Initialization state*/
    ADC_SEND_CMD, /*!<Selects current input channel on ADC*/
    ADC_READ, /*!<Reads ADC into input buffer*/
    CHECK_ALARMS, /*!<Checks and triggers alarms*/
    TRIGGER_ALARMS, /*!<Triggers or Untiggers alarms*/
    DIGITAL_INPUTS_READ, /*!<Reads Digital inputs and saves their state*/
    DAC_SEND_DATA, /*!<Sends data to DAC1 from output buffer*/
    DISPLAY, /*!<Update display - TODO*/
    USER_INPUT_PERFORM_LOGIC, /*!<Perform actions dependent on user input - TODO*/
    INCREMENT_CHANNEL /*!<Increment channel counters*/
} STATES;

STATES state = STATE_INITIALIZE;
STATES nextState = STATE_INITIALIZE;

// ***********************
// DATA STORAGE
// ***********************

INPUT inputs[8]; //!<4 Analog inputs from ADC (inputs[0-3]), 4 Digital inputs (inputs[4-7])
ANALOG analogs[4];
double pastData[4][30]; //!<Store history of input data
OUTPUT outputs[10]; //!<2 Analog outputs to DAC (outputs[0-1]), 8 Relays (outputs[2-9])

//************************


//!Flag that used to determine if to start a new task
volatile bool task_FLAG = false;
//!Flag that used to determine is 30 seconds have passed
//!Used for storing input data every 30 seconds
bool thirty_sec_passed = false;


uint8_t input_channel = 0; //!< 0-7, 0-3 is Analog inputs, 4-7 is Digital Inputs
uint8_t DAC_channel = 0; //!< 0-1, Either DAC1 or DAC2
uint16_t ms_counter = 0; //!< Counter of ms that passed resets at 30000ms


#define SCALE(input, old_max, old_min, new_max, new_min) ((((input - old_min) * (new_max - new_min)) / (old_max - old_min)) + new_min)

//!Update the past data storage
void pastDataUpdate(double* data) {
    for (int i = 0; i < 29; i++) {
        pastData[input_channel][i] = pastData[input_channel][i + 1];
    }
    pastData[input_channel][29] = *data;
}




//!This is callback routine is called by TMR interrupt
//!Triggers every 1ms, sets task_FLAG to true, increments ms counter
void TIMER2_InterruptSvcRoutine(uint32_t status, uintptr_t context) {
    //Set flag to true to switch to new task
    task_FLAG = true;
    //Check if 30seconds have past
    if (ms_counter == 30000) {
        ms_counter = 0;
    }else{
        ms_counter++;
    }
    
}

void APP_WriteCallback(uintptr_t context)
{
    writeStatus = true;
    
}

void APP_ReadCallback(uintptr_t context)
{
    
    if (rec_char == '\r' || receive_buff_size > 127){
        receive_buff_size = 0;
        
    }else{
        receive_buffer[receive_buff_size] = rec_char;
    }
    receive_buff_size++;
}


//!Main
int main(void) {
    SYS_Initialize(NULL);

    TMR2_CallbackRegister(TIMER2_InterruptSvcRoutine, (uintptr_t) NULL);
    TMR2_Start();
    
    UART1_WriteCallbackRegister(APP_WriteCallback, 0);
    UART1_ReadCallbackRegister(APP_ReadCallback, 0);
    
    //Set GPIO pins to low for not in use
    LED_RED_Clear();
    RELAY0_Clear();
    RELAY1_Clear();
    RELAY2_Clear();
    RELAY3_Clear();
    RELAY4_Clear();
    RELAY5_Clear();
    RELAY6_Clear();
    RELAY7_Clear();

    //Set SS pins to High for not in use
    SS_ADC_Set();
    SS_DAC1_Set();
    SS_DAC2_Set();

    while (BTN0_Get() == 1);
 

    while (1) {
        
        if (task_FLAG) {
            switch (state) {
                case STATE_INITIALIZE:;
                
                
                    Terminal_Initialize();
                    
                    
                    UART1_Write(&"\033[2J", sizeof("\033[2J"));
                    while (UART1_WriteIsBusy()); 
                    UART1_Write(&"\033[0;0H", sizeof("\033[0;0H"));
                    while (UART1_WriteIsBusy());
                    
                    ADC_Initialize();
                    
                        //Initialize Analog inputs
                    for (uint8_t i = 0; i < 4; i++) {
                        ANALOG analog_int = {0,0,0,0};
                        INPUT input_int = {true, false, NULL, {NULL}, false};
                        analogs[i] = analog_int;
                        inputs[i] = input_int;
                        inputs[i].analog_input = &analogs[i];
                    }
                    if (inputs[0].analog_input == NULL)
                    //
                    for (uint8_t i = 4; i < 8; i++) {
                        INPUT input_int = {false, false, NULL, {NULL}, false};
                        inputs[i] = input_int;
                    }
                    for (uint8_t i=0; i<10; i++){
                        OUTPUT output_int = {false, 0, false, 0,0,-1,false};                    
                        outputs[i] = output_int;
                    }
                    
                    
                    
                    
//                    ConfigureInput(&inputs[0], true, 16777215, 0);
//                    ConfigureInput(&inputs[1], true, 16777215, 0);
//                    ConfigureInput(&inputs[2], true, 16777215, 0);
//                    ConfigureInput(&inputs[3], true, 16777215, 0);
//                    
//                    ConfigureInput(&inputs[4], false, 0, 0);
//                    ConfigureInput(&inputs[5], false, 0, 0);
//                    ConfigureInput(&inputs[6], false, 0, 0);
//                    ConfigureInput(&inputs[7], false, 0, 0);
//                    CreateDigitalAlarm(&outputs[2], &inputs[4], 4);
//                    CreateDigitalAlarm(&outputs[3], &inputs[5], 5);
//                    CreateDigitalAlarm(&outputs[4], &inputs[6], 6);
//                    
//                    
//                    CreateAnalogAlarm(&outputs[6], &inputs[0], 500, 450, 0, true);
//                    CreateAnalogAlarm(&outputs[7], &inputs[0], 300, 350, 0, false);
//                    ConfigureAnalogOutput(&outputs[0], &inputs[0], 0, 800,200);
                    

                    state = ADC_SEND_CMD;
                    break;

                case ADC_SEND_CMD:
                    if (inputs[input_channel / 2].is_set){
                        ADC_Select_Chnl(input_channel / 2);
                    }
                    state = ADC_READ;
                    break;

                    //In this state the ADC is read for the current input channel
                case ADC_READ:
                    //Read the data
                    
                    if (inputs[input_channel / 2].is_set){
                        ADC_Read_Data((uint8_t*)&(inputs[input_channel / 2].analog_input->raw_data));

                        //Store the data in past storage if 30 seconds have past
                        if (thirty_sec_passed) {
                            pastDataUpdate(&inputs[input_channel / 2].analog_input->scaled_data);
                            thirty_sec_passed = false;
                        }


                        ANALOG* input = inputs[input_channel / 2].analog_input;
                        //Convert to user scale and save
                        input->scaled_data = SCALE((double) input->raw_data, 16777215, 0, input->max, input->min);
                    
                    }

                    state = DIGITAL_INPUTS_READ;
                    break;

                case DIGITAL_INPUTS_READ:;
                
                    uint8_t digital_channel = 4+(input_channel % 4);
                    switch(digital_channel){
                        case 4:
                            inputs[digital_channel].digital = DIGITAL0_Get()==1;
                            break;
                        case 5:
                            inputs[digital_channel].digital = DIGITAL1_Get()==1;
                            break;
                        case 6:
                            inputs[digital_channel].digital = DIGITAL2_Get()==1;
                            break;
                        case 7:
                            inputs[digital_channel].digital = DIGITAL3_Get()==1;
                            break;
                        default:
                            break;
                            
                    }
                    state = DISPLAY;
                    break;

                    //TODO
                case DISPLAY:;
                    
                    //First 10 lines for debugging
                    //Set cursor to 10th line
                    while (UART1_WriteIsBusy()); 
                    UART1_Write(&"\033[10;0H", sizeof("\033[10;0H"));

                    
                    //TESTING///////////////////
//                        test_cmd = READ + STATUS;
//                        SS_ADC_Clear();
//                        for (int i=0; i<100000; i++);
//                        SPI1_Write(&test_cmd, 1);
//                        while(SPI1_IsBusy());
//                        
//                        SPI1_Read(&reg_data, 1);
//                        while(SPI1_IsBusy());
//                        for (int i=0; i<100; i++);
//                        for (int i=0; i<100000; i++);
//                        SS_ADC_Set();    
//                        
//                        nbytes = sprintf(buffer, "Status: %d%d%d%d%d%d%d%d\r\n", (reg_data>>7) & 0x01,(reg_data>>6) & 0x01,(reg_data>>5) & 0x01,(reg_data>>4) & 0x01,(reg_data>>3) & 0x01,(reg_data>>2) & 0x01,(reg_data>>1) & 0x01, reg_data & 0x01 );
//                        while (UART1_WriteIsBusy()); 
//                        UART1_Write(&buffer, nbytes);
//                        while (UART1_WriteIsBusy());
//                        
//                        
//                        
//                        test_cmd = READ+MODE;
//                        reg_data = 0;
//                        
//                        SS_ADC_Clear();
//                        SPI1_Write(&test_cmd, 1);
//                        while(SPI1_IsBusy());
// 
//                        SPI1_Read(&reg_data, 1);
//                        while(SPI1_IsBusy());
//                        for (int i=0; i<100; i++);
//                        SS_ADC_Set(); 
//                        
//                        
//                        nbytes = sprintf(buffer, "Mode Reg: %d%d%d%d%d%d%d%d\r\n", (reg_data>>7) & 0x01,(reg_data>>6) & 0x01,(reg_data>>5) & 0x01,(reg_data>>4) & 0x01,(reg_data>>3) & 0x01,(reg_data>>2) & 0x01,(reg_data>>1) & 0x01, reg_data & 0x01 );
//                        while (UART1_WriteIsBusy()); 
//                        UART1_Write(&buffer, nbytes);
//                        while (UART1_WriteIsBusy());
//                        
//                        
//                        test_cmd = READ+ID;
//                        reg_data = 0;
//                        SS_ADC_Clear();
//                        SPI1_Write(&test_cmd, 1);
//                        while(SPI1_IsBusy());
//                        
//                        
//                        SPI1_Read(&reg_data, 1);
//                        while(SPI1_IsBusy());
//                        for (int i=0; i<100; i++);
//                        SS_ADC_Set(); 
//                        nbytes = sprintf(buffer, "ID Reg: %d%d%d%d%d%d%d%d\r\n", (reg_data>>7) & 0x01,(reg_data>>6) & 0x01,(reg_data>>5) & 0x01,(reg_data>>4) & 0x01,(reg_data>>3) & 0x01,(reg_data>>2) & 0x01,(reg_data>>1) & 0x01, reg_data & 0x01 );
//                        while (UART1_WriteIsBusy()); 
//                        UART1_Write(&buffer, nbytes);
//                        while (UART1_WriteIsBusy());
//                        
//                        
//                        test_cmd = READ+MODE;
//                        reg_data = 0;
//                        SS_ADC_Clear();
//                        SPI1_Write(&test_cmd, 1);
//                        while(SPI1_IsBusy());
//                        
//                        
//                        SPI1_Read(&reg_data, 1);
//                        while(SPI1_IsBusy());
//                        for (int i=0; i<100; i++);
//                        SS_ADC_Set(); 
//                        nbytes = sprintf(buffer, "ID Reg: %d%d%d%d%d%d%d%d\r\n", (reg_data>>7) & 0x01,(reg_data>>6) & 0x01,(reg_data>>5) & 0x01,(reg_data>>4) & 0x01,(reg_data>>3) & 0x01,(reg_data>>2) & 0x01,(reg_data>>1) & 0x01, reg_data & 0x01 );
//                        while (UART1_WriteIsBusy()); 
//                        UART1_Write(&buffer, nbytes);
//                        while (UART1_WriteIsBusy());
//                        
//                        
//                        test_cmd = READ+ID;
//                        reg_data = 0;
//                        SS_ADC_Clear();
//                        SPI1_Write(&test_cmd, 1);
//                        while(SPI1_IsBusy());
//                        
//                        
//                        SPI1_Read(&reg_data, 1);
//                        
//                        while(SPI1_IsBusy());
//                        
//                        SS_ADC_Set(); 
//                        nbytes = sprintf(buffer, "ID Reg: %d%d%d%d%d%d%d%d\r\n", (reg_data>>7) & 0x01,(reg_data>>6) & 0x01,(reg_data>>5) & 0x01,(reg_data>>4) & 0x01,(reg_data>>3) & 0x01,(reg_data>>2) & 0x01,(reg_data>>1) & 0x01, reg_data & 0x01 );
//                        while (UART1_WriteIsBusy()); 
//                        UART1_Write(&buffer, nbytes);
//                        while (UART1_WriteIsBusy());
                     
                    //////////////////
                    
                    
                    
                    //Print the values of each input
                    for (int i=0; i<4; i++){
                        nbytes = sprintf(buffer, "Analog Input #%d %u\r\n",i, analogs[i].raw_data);
                        while (UART1_WriteIsBusy()); 
                        UART1_Write(&buffer, nbytes);
                        while (UART1_WriteIsBusy());
                    }
                    for (int i=4; i<8; i++){
                        nbytes = sprintf(buffer, "Digital Input #%d %s\r\n",i-4, inputs[i].digital ? "on" : "off");
                        while (UART1_WriteIsBusy()); 
                        UART1_Write(&buffer, nbytes);
                        while (UART1_WriteIsBusy());
                    }
                    
                    //Print the alarms currently set for each input
                    for (int i=0; i<8; i++){
                        nbytes = sprintf(buffer, "\nInput #%d alarms:\r\n", i);
                        while (UART1_WriteIsBusy());
                        UART1_Write(&buffer, nbytes);
                        while (UART1_WriteIsBusy());
                        for (int j=0; j<4; j++){
                            if (inputs[i].alrms[j] != NULL && inputs[i].alrms[j]->input_chnl != -1) {
                                nbytes = sprintf(buffer, "Alarm #%d on/off: %s, type:%s, high/low:%s, trigger:%f, reset:%f\r\n",
                                                                j, inputs[i].alrms[j]->relay ? "On" : "Off",
                                                                inputs[i].alrms[j]->rel_dac ? "Relay" : "Analog",
                                                                inputs[i].alrms[j]->high_low ? "High-Low" : "Low-High",
                                                                inputs[i].alrms[j]->trigger, inputs[i].alrms[j]->reset);
                                while (UART1_WriteIsBusy());
                                UART1_Write(&buffer, nbytes);
                                while (UART1_WriteIsBusy());
                            }else{
                                nbytes = sprintf(buffer,"Alarm #%d not used\r\n", j);
                                while (UART1_WriteIsBusy());
                                UART1_Write(&buffer, nbytes);
                                while (UART1_WriteIsBusy());
                            }
                        }
                    }
                    
                    
                    while (UART1_WriteIsBusy());
                    if (rec_char == '\r'){
                        UART1_Write(&"\033[0K", sizeof("\033[0K"));
                    }else{
                        UART1_Write(&receive_buffer, receive_buff_size);
                    }
                    while (UART1_WriteIsBusy());
                    state = USER_INPUT_PERFORM_LOGIC;
                    
                    
                    break;

                    //TODO
                case USER_INPUT_PERFORM_LOGIC:
                   
                    UART1_Read(&rec_char, 1);
                    
                    
                    
                    
                    
                    state = CHECK_ALARMS;
                    break;

                    //In this state the alarms are checked and triggered
                case CHECK_ALARMS:
                    for (uint8_t i = 0; i < 4; i++) {
                        if (inputs[input_channel].alrms[i] != NULL && inputs[input_channel].alrms[i]->input_chnl != -1) {
                            INPUT* input = &inputs[input_channel];
                            OUTPUT* output = input->alrms[i];

                            //Input is analog check if inside max and min
                            if (input->ang_dig) {

                                //Check if trigger on high or low
                                if (output->high_low) {

                                    //If data is above trigger point set relay output
                                    if (input->analog_input->scaled_data > output->trigger) {
                                        output->relay = true;
                                        //if data is below reset point clear relay output    
                                    } else if (input->analog_input->scaled_data < output->reset) {
                                        output->relay = false;
                                    }
                                } else {
                                    //If data is below trigger point set relay output
                                    if (input->analog_input->scaled_data < output->trigger) {
                                        output->relay = true;
                                        //if data is above reset point clear relay output 
                                    } else if (input->analog_input->scaled_data > output->reset) {
                                        output->relay = false;
                                    }
                                }
                                //Input is digital, simply check on and off
                            } else {
                                
                                if (input->digital) {
                                    output->relay = true;
                                } else {
                                    output->relay = false;
                                }
                            }
                        }
                    }

                    state = TRIGGER_ALARMS;
                    break;
                    
                case TRIGGER_ALARMS:
                    for (uint8_t i=2; i<10; i++){
                        bool set_alarm = outputs[i].relay;
                            switch(i-2){
                                case 0:
                                    set_alarm ? RELAY0_Set() : RELAY0_Clear();
                                    break;
                                case 1:
                                    set_alarm ? RELAY1_Set() : RELAY1_Clear();
                                    break;
                                case 2:
                                    set_alarm ? RELAY2_Set() : RELAY2_Clear();
                                    break;
                                case 3:
                                    set_alarm ? RELAY3_Set() : RELAY3_Clear();
                                    break;
                                case 4:
                                    set_alarm ? RELAY4_Set() : RELAY4_Clear();
                                    break;
                                case 5:
                                    set_alarm ? RELAY5_Set() : RELAY5_Clear();
                                    break;
                                case 6:
                                    set_alarm ? RELAY6_Set() : RELAY6_Clear();
                                    break;
                                case 7:
                                    set_alarm ? RELAY7_Set() : RELAY7_Clear();
                                    break;
                                                                                                                                                                    
                            }
                    }
                    
                    state = DAC_SEND_DATA;
                    break;
                case DAC_SEND_DATA:;
                    
                    //Get the channel that is set to provide input to the data
                    short int input_chnl = outputs[DAC_channel].input_chnl;
   
                    if (input_chnl != -1 ){
                        if (DAC_channel == 0)
                            SS_DAC1_Clear();
                        else if (DAC_channel == 1)
                            SS_DAC2_Clear();                        
                        
                        INPUT* dac1_input = &inputs[input_chnl];
                        
                        double output_trig = outputs[DAC_channel].trigger;
                        double output_reset = outputs[DAC_channel].reset;
                        if (dac1_input->analog_input->scaled_data > output_trig)
                            outputs[DAC_channel].data = 65534;
                        else if (dac1_input->analog_input->scaled_data - output_reset > 0)
                            outputs[DAC_channel].data = SCALE(dac1_input->analog_input->scaled_data, output_trig, output_reset, 65535, 0);
                        else
                            outputs[DAC_channel].data = 0;

                        SPI1_Write((uint8_t*)&outputs[DAC_channel].data, 2);
                        while (SPI1_IsBusy());

                        if (DAC_channel == 0)
                            SS_DAC1_Set();
                        else if (DAC_channel == 1)
                            SS_DAC2_Set();    

                    }


                    state = INCREMENT_CHANNEL;
                    break;


                case INCREMENT_CHANNEL:

                    //Reset ADC Channel at max
                    if (input_channel == 7)
                        input_channel = 0;
                    else
                        input_channel++;

                    //Reset DAC channel 
                    DAC_channel = !DAC_channel;

                    //Goto first task
                    state = ADC_SEND_CMD;
                    break;


                default:
                    break;
            }

            //Reset timer flag
            task_FLAG = false;
        }


    }


    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE);
}


/*******************************************************************************
 End of File
 */

