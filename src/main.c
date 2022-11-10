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
        ALARMS;
        DIGITAL_INPUTS_READ;
        DISPLAY;
        USER_INPUT_PERFORM_LOGIC;
        DAC1_SEND_CMD;
        DAC2_SEND_CMD;
        INCREMENT_CHANNEL;
            STATE_INITIALIZE -> ADC_SEND_CMD
            ADC_SEND_CMD -> ADC_READ;       
            ADC_READ -> ALARMS;
            ALARMS -> DIGITAL_INPUTS_READ;
            DIGITAL_INPUTS_READ -> DISPLAY
            DISPLAY -> USER_INPUT_PERFORM_LOGIC;
            USER_INPUT_PERFORM_LOGIC -> DAC1_SEND_CMD;
            DAC1_SEND_CMD -> DAC2_SEND_CMD ;
            DAC2_SEND_CMD -> INCREMENT_CHANNEL;
            INCREMENT_CHANNEL -> ADC_SEND_CMD;
    }
   \enddot
*/
typedef enum {
    STATE_INITIALIZE, /*!<Initialization state*/
    ADC_SEND_CMD, /*!<Selects current input channel on ADC*/
    ADC_READ, /*!<Reads ADC into input buffer*/
    ALARMS, /*!<Checks and triggers alarms*/
    DIGITAL_INPUTS_READ, /*!<Reads Digital inputs and saves their state*/
    DAC1_SEND_CMD, /*!<Sends data to DAC1 from output buffer*/
    DAC2_SEND_CMD, /*!<Sends data to DAC2 from output buffer*/
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
    }
    RELAY7_Toggle();
}


//!Main
int main(void) {
    SYS_Initialize(NULL);


    TMR2_CallbackRegister(TIMER2_InterruptSvcRoutine, (uintptr_t) NULL);
    TMR2_Start();

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
    SS_DISPLAY_Set();

    while (BTN0_Get() == 1);
    IT8951Display1bppExample();


    while (1) {
        if (task_FLAG) {
            switch (state) {
                case STATE_INITIALIZE:

                    //Send Commands to ADC to set mode to continuous 
                    ADC_Initialize();

                    //Initialize Analog inputs
                    for (uint8_t i = 0; i < 4; i++) {
                        inputs[i].analog_input = (ANALOG*) malloc(sizeof (ANALOG));
                    }


                    inputs[4].digital_get = (DIGITAL_INPUT_CALLBACK) DIGITAL0_Get();
                    inputs[5].digital_get = (DIGITAL_INPUT_CALLBACK) DIGITAL1_Get();
                    inputs[6].digital_get = (DIGITAL_INPUT_CALLBACK) DIGITAL2_Get();
                    inputs[7].digital_get = (DIGITAL_INPUT_CALLBACK) DIGITAL3_Get();



                    //Set functions for easier calling
                    outputs[2].relay_set = (RELAY_CALLBACK) RELAY0_Set();
                    outputs[2].relay_clear = (RELAY_CALLBACK) RELAY0_Clear();
                    outputs[3].relay_set = (RELAY_CALLBACK) RELAY1_Set();
                    outputs[3].relay_clear = (RELAY_CALLBACK) RELAY1_Clear();
                    outputs[4].relay_set = (RELAY_CALLBACK) RELAY2_Set();
                    outputs[4].relay_clear = (RELAY_CALLBACK) RELAY2_Clear();
                    outputs[5].relay_set = (RELAY_CALLBACK) RELAY3_Set();
                    outputs[5].relay_clear = (RELAY_CALLBACK) RELAY3_Clear();
                    outputs[6].relay_set = (RELAY_CALLBACK) RELAY4_Set();
                    outputs[6].relay_clear = (RELAY_CALLBACK) RELAY4_Clear();
                    outputs[7].relay_set = (RELAY_CALLBACK) RELAY5_Set();
                    outputs[7].relay_clear = (RELAY_CALLBACK) RELAY5_Clear();
                    outputs[8].relay_set = (RELAY_CALLBACK) RELAY6_Set();
                    outputs[8].relay_clear = (RELAY_CALLBACK) RELAY6_Clear();
                    outputs[9].relay_set = (RELAY_CALLBACK) RELAY7_Set();
                    outputs[9].relay_clear = (RELAY_CALLBACK) RELAY7_Clear();


                    state = ADC_SEND_CMD;
                    break;

                case ADC_SEND_CMD:

                    ADC_Select_Chnl(input_channel / 2);

                    state = ADC_READ;
                    break;

                    //In this state the ADC is read for the current input channel
                case ADC_READ:
                    //Read the data
                    ADC_Read_Data(&inputs[input_channel / 2].analog_input->raw_data);

                    //Store the data in past storage if 30 seconds have past
                    if (thirty_sec_passed) {
                        pastDataUpdate(&inputs[input_channel / 2].analog_input->scaled_data);
                        thirty_sec_passed = false;
                    }


                    ANALOG* input = inputs[input_channel / 2].analog_input;
                    //Convert to user scale and save
                    input->scaled_data = ((double) input->raw_data / (16777215)) * (input->max - input->min) + input->min;
                    state = DIGITAL_INPUTS_READ;
                    break;

                case DIGITAL_INPUTS_READ:
                    inputs[input_channel % 4].digital = inputs[input_channel % 4].digital_get() == 1;

                    state = DISPLAY;
                    break;

                    //TODO
                case DISPLAY:
                    state = USER_INPUT_PERFORM_LOGIC;
                    break;

                    //TODO
                case USER_INPUT_PERFORM_LOGIC:

                    state = ALARMS;
                    break;

                    //In this state the alarms are checked and triggered
                case ALARMS:
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
                                        output->relay_set();
                                        //if data is below reset point clear relay output    
                                    } else if (input->analog_input->scaled_data < output->reset) {
                                        output->relay_clear();
                                    }
                                } else {
                                    //If data is below trigger point set relay output
                                    if (input->analog_input->scaled_data < output->trigger) {
                                        output->relay_set();
                                        //if data is above reset point clear relay output 
                                    } else if (input->analog_input->scaled_data > output->reset) {
                                        output->relay_clear();
                                    }
                                }
                                //Input is digital, simply check on and off
                            } else {
                                if (input->digital) {
                                    output->relay_set();
                                } else {
                                    output->relay_clear();
                                }
                            }
                        }
                    }

                    state = DAC1_SEND_CMD;
                    break;

                case DAC1_SEND_CMD:

                    SS_DAC1_Clear();
                    SPI1_Write(&outputs[DAC_channel], 2);
                    while (SPI1_IsBusy());
                    SS_DAC1_Set();

                    state = DAC2_SEND_CMD;
                    break;

                case DAC2_SEND_CMD:

                    SS_DAC2_Clear();
                    SPI1_Write(&outputs[DAC_channel], 2);
                    while (SPI1_IsBusy());
                    SS_DAC2_Set();

                    state = INCREMENT_CHANNEL;
                    break;


                case INCREMENT_CHANNEL:

                    //Reset ADC Channel at max
                    if (input_channel == 3)
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

