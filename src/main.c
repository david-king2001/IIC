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
#include <stdio.h>
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

double pastData[4][30]; //!<Store history of input data

#define input_initialize {.ang_dig = true, .digital_on = false, .is_set = false, .max = 0.0, .min = 0.0, .raw_data=0, .scaled_data=0.0}
#define input_initialize_digital {.ang_dig = false, .digital_on = false, .is_set = false, .max = 0.0, .min = 0.0, .raw_data=0, .scaled_data=0.0}
#define output_initialize_analog {.data = 0, .high_low= false, .input_chnl=-1, .rel_dac = false, .relay = false, .reset = 0.0, .trigger = 0.0}
#define output_initialize {.data = 0, .high_low= false, .input_chnl=-1, .rel_dac = true, .relay = false, .reset = 0.0, .trigger = 0.0}

//!4 Analog inputs from ADC (inputs[0-3]), 4 Digital inputs (inputs[4-7])
INPUT inputs[8] = {input_initialize, input_initialize, input_initialize, input_initialize, input_initialize_digital, input_initialize_digital, input_initialize_digital, input_initialize_digital}; 
OUTPUT outputs[10] = {output_initialize_analog, output_initialize_analog, output_initialize, output_initialize, output_initialize, output_initialize, output_initialize, output_initialize}; //!<2 Analog outputs to DAC (outputs[0-1]), 8 Relays (outputs[2-9])
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
    //writeStatus = true;
    
}

void APP_ReadCallback(uintptr_t context)
{
    
    if (rec_char == '\r' || receive_buff_size > 127){
        receive_buff_size = 0;
        enter = true;
    }else if (rec_char == '\b' && receive_buff_size!=0){
        receive_buff_size--;
    }else{
        receive_buffer[receive_buff_size] = rec_char;
        receive_buff_size++;
    }
    
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
    SS_DAC1_Clear();
    SS_DAC2_Set();


    
    while (BTN0_Get() == 1);
    while (1) {
        
        if (task_FLAG) {
            switch (state) {
                case STATE_INITIALIZE:;

              
                    Terminal_Initialize();
//                    
//                    
//                    UART1_Write(&"\033[2J", sizeof("\033[2J"));
//                    while (UART1_WriteIsBusy()); 
//                    UART1_Write(&"\033[0;0H", sizeof("\033[0;0H"));
//                    while (UART1_WriteIsBusy());
                    
                    ADC_Initialize();
                    
//                    
                    ConfigureInput(&inputs[1], true, 16777215, 0);
                    ConfigureAnalogOutput(&outputs[0], &inputs[1], 1, inputs[1].max, inputs[1].min);
                    ConfigureInput(&inputs[4], false, 0, 0);

                    
                    
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
                        ADC_Read_Data((uint8_t*)&(inputs[input_channel / 2].raw_data));
                        //Store the data in past storage if 30 seconds have past
                        if (thirty_sec_passed) {
                            pastDataUpdate(&inputs[input_channel / 2].scaled_data);
                            thirty_sec_passed = false;
                        }
                       
                        INPUT* input = &inputs[input_channel / 2];
                        //Convert to user scale and save
                        input->scaled_data = SCALE((double) input->raw_data, 0, 16777215, input->max, input->min);
                    
                    }

                    state = DIGITAL_INPUTS_READ;
                    break;

                case DIGITAL_INPUTS_READ:;
                
                    uint8_t digital_channel = 4+(input_channel % 4);
                    switch(digital_channel){
                        case 4:
                            inputs[digital_channel].digital_on = DIGITAL0_Get()==1;
                            break;
                        case 5:
                            inputs[digital_channel].digital_on = DIGITAL1_Get()==1;
                            break;
                        case 6:
                            inputs[digital_channel].digital_on = DIGITAL2_Get()==1;
                            break;
                        case 7:
                            inputs[digital_channel].digital_on = DIGITAL3_Get()==1;
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
                    UART1_Write(&"\033[3;0H", sizeof("\033[3;0H"));
                    
                    PrintRegister(MODE);
                    PrintRegister(STATUS);
                    PrintAnalogInputs();
                    PrintDigitalInputs();
                    PrintAnalogOutputs();
                    PrintRelays();
                    PrintAlarmSettings();
                    
                    if (enter){
                        //Clear the line
                        UART1_Write(&"\033[0K", sizeof("\033[0K"));
                    }else{
                        //Print back what has been received
                        UART1_Write(&receive_buffer, receive_buff_size);
                    }
                    while (UART1_WriteIsBusy());
                    state = USER_INPUT_PERFORM_LOGIC;
                    
                    
                    break;

                    //TODO
                case USER_INPUT_PERFORM_LOGIC:
                   
                    //Read a single character
                    UART1_Read(&rec_char, 1);
                    
                    //If the enter key was pressed, check input buffer
                    if (enter){
                        enter = false;
                        receive_buffer[receive_buff_size+1] = '\0';
                        if (receive_buffer[0] == 'I'){
                            ParseInputForInput(&receive_buffer[2]);
                        }else if (receive_buffer[0] == 'A'){
                            ParseInputForAlarm(&receive_buffer[2]);
                        }else if (receive_buffer[0] == 'O'){
                            
                        }else if (receive_buffer[0] =='D'){
                            
                        }else{
                            while (UART1_WriteIsBusy()); 
                            UART1_Write(&"Invalid input to terminal\r\n", sizeof("Invalid input to terminal\r\n"));
                            while (UART1_WriteIsBusy());
                        }
                    }
                    
                    
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
                                    if (input->scaled_data > output->trigger) {
                                        output->relay = true;
                                        //if data is below reset point clear relay output    
                                    } else if (input->scaled_data < output->reset) {
                                        output->relay = false;
                                    }
                                } else {
                                    //If data is below trigger point set relay output
                                    if (input->scaled_data < output->trigger) {
                                        output->relay = true;
                                        //if data is above reset point clear relay output 
                                    } else if (input->scaled_data > output->reset) {
                                        output->relay = false;
                                    }
                                }
                                //Input is digital, simply check on and off
                            } else {
                                
                                if (input->digital_on) {
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
                    outputs[2].relay ? RELAY0_Set() : RELAY0_Clear();
                    outputs[3].relay ? RELAY1_Set() : RELAY1_Clear();
                    outputs[4].relay ? RELAY2_Set() : RELAY2_Clear();
                    outputs[5].relay ? RELAY3_Set() : RELAY3_Clear();
                    outputs[6].relay ? RELAY4_Set() : RELAY4_Clear();
                    outputs[7].relay ? RELAY5_Set() : RELAY5_Clear();
                    outputs[8].relay ? RELAY6_Set() : RELAY6_Clear();
                    outputs[9].relay ? RELAY7_Set() : RELAY7_Clear();
                    state = DAC_SEND_DATA;
                    break;
                case DAC_SEND_DATA:;
                    
                    //Get the channel that is set to provide input to the data
                    short int input_chnl = outputs[DAC_channel].input_chnl;
   
                    if (input_chnl != -1 ){
                        if (DAC_channel == 0){
                            SS_DAC1_Set();
                            for (int i=0; i<1000; i++);
                            SS_DAC1_Clear();
                        }else if (DAC_channel == 1){
                            SS_DAC2_Set();     
                            for (int i=0; i<1000; i++);
                            SS_DAC2_Clear();
                        }
                        for (int i=0; i<1000; i++);
                        INPUT* dac1_input = &inputs[input_chnl];
                        
                        double output_trig = outputs[DAC_channel].trigger;
                        double output_reset = outputs[DAC_channel].reset;
                        if (dac1_input->scaled_data > output_trig){
                            outputs[DAC_channel].data = 65534;
                            LED_RED_Set();
                        }else if (dac1_input->scaled_data - output_reset > 0)
                            outputs[DAC_channel].data = SCALE(dac1_input->scaled_data, output_trig, output_reset, 65535, 0);
                        else
                            outputs[DAC_channel].data = 0;
                        
                        uint16_t flipped_out = (outputs[DAC_channel].data << 8) + (outputs[DAC_channel].data >>8);
                        flipped_out=0xff6f;
                        
                        SPI1_Write(((uint8_t*)&flipped_out), 2);
                        while (SPI1_IsBusy());

                        for (int i=0; i<1000; i++);
                        if (DAC_channel == 0){
                            SS_DAC1_Set();
                            for (int i=0; i<1000; i++);
                            SS_DAC1_Clear();
                        }else if (DAC_channel == 1){
                            SS_DAC2_Set();    
                            for (int i=0; i<1000; i++);
                            SS_DAC2_Clear();
                        }
                        for (int i=0; i<1000000; i++);
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

