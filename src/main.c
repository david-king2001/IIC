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
        RELAY7_Toggle();
    }else{
        ms_counter++;
    }
    
}

int test = 0;


GPIO_PIN button_pressed = GPIO_PIN_NONE; //!< Used to store last button pressed

//!Callback for user buttons interrupt
/*!
 * Whenever a button is released, check to make sure last button pressed was handled i.e pin = GPIO_PIN_NONE, 
 * then set button_pressed to pin for button that was pressed
 */
void InputBtn(GPIO_PIN pin, uintptr_t context){
    if (BTN0_Get()==1 && button_pressed == GPIO_PIN_NONE){
        button_pressed = pin;
    }
}



//!Main
int main(void) {
    SYS_Initialize(NULL);

    GPIO_PinInterruptCallbackRegister(BTN0_PIN, InputBtn, (uintptr_t)NULL);
    GPIO_PinInterruptCallbackRegister(BTN1_PIN, InputBtn, (uintptr_t)NULL);
    GPIO_PinInterruptCallbackRegister(BTN2_PIN, InputBtn, (uintptr_t)NULL);
    GPIO_PinInterruptCallbackRegister(BTN3_PIN, InputBtn, (uintptr_t)NULL);
    GPIO_PinInterruptCallbackRegister(BTN4_PIN, InputBtn, (uintptr_t)NULL);
    GPIO_PinInterruptEnable(BTN0_PIN);
    GPIO_PinInterruptEnable(BTN1_PIN);
    GPIO_PinInterruptEnable(BTN2_PIN);
    GPIO_PinInterruptEnable(BTN3_PIN);
    GPIO_PinInterruptEnable(BTN4_PIN);
    
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


    while (1) {
        if (task_FLAG) {
            switch (state) {
                case STATE_INITIALIZE:

                    //Send Commands to ADC to set mode to continuous 
                    ADC_Initialize();

                    //Initialize Analog inputs
                    for (uint8_t i = 0; i < 4; i++) {
                        inputs[i].analog_input = (ANALOG*) malloc(sizeof (ANALOG));
                        inputs[i].ang_dig = true;
                    }
                    for (uint8_t i=0; i<10; i++){
                        outputs[i].input_chnl = -1;
                    }
                    
                    inputs[0].ang_dig = true;
                    CreateAlarm(&outputs[8], &inputs[0], 10, 5, 0, true);
                    
                    inputs[5].ang_dig = false;
                    CreateAlarm(&outputs[2], &inputs[5], 0, 0, 5, false);
                    inputs[6].ang_dig = false;
                    CreateAlarm(&outputs[3], &inputs[6], 0, 0, 6, false);
                    inputs[7].ang_dig = false;
                    CreateAlarm(&outputs[4], &inputs[7], 0, 0, 7, false);
                    
                    
                        

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
                    input->scaled_data = SCALE((double) input->raw_data, 16777215, 0, input->max, input->min);
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
                case DISPLAY:
                    state = USER_INPUT_PERFORM_LOGIC;
                    break;

                    //TODO
                case USER_INPUT_PERFORM_LOGIC:
                    if (button_pressed == BTN0_PIN){
                        if (test==15){
                            test = 0;
                            RELAY7_Toggle();
                        }else{
                            test++;
                        }
                        button_pressed = GPIO_PIN_NONE;
                    }
                    inputs[0].analog_input->scaled_data = test;
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

                        //Scale 24bit data down to 16 bits and then scale according to set scale factor
                        outputs[DAC_channel].data = (uint16_t)SCALE((dac1_input->analog_input->raw_data >> 4), outputs[DAC_channel].trigger, outputs[DAC_channel].reset, 65535, 0);
                        
                        SPI1_Write(&outputs[DAC_channel].data, 2);
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

