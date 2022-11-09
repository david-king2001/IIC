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

typedef enum
{
	STATE_INITIALIZE,
    ADC_SEND_CMD,
    ADC_READ,
    CHECK_ALARMS,
    TRIGGER_ALARMS,
    DAC1_SEND_CMD,
    DAC2_SEND_CMD,
    DISPLAY,
    USER_INPUT_PERFORM_LOGIC,
    INCREMENT_CHANNEL
} STATES;

STATES state = STATE_INITIALIZE;
STATES nextState = STATE_INITIALIZE;

// ***********************
// DATA STORAGE
// ***********************

INPUT inputs[8]; //4 Analog inputs from ADC (inputs[0-3]), 4 Digital inputs (inputs[4-7])
ALARM alarms[32]; //32 possible alarms, 4 for each input
double pastData[4][30]; //Store history of input data
OUTPUT outputs[10]; //2 Analog outputs to DAC (outputs[0-1]), 8 Relays (outputs[2-9])

//************************


// ***********************
// FLAGS
// ***********************

volatile bool task_FLAG = false;
bool thirty_sec_passed = false;

//************************


// ***********************
// COUNTERS
// ***********************

uint8_t ADC_channel = 0;//0-3
uint8_t DAC_channel = 0;//0-1
uint16_t ms_counter = 0;

//************************




void pastDataUpdate(double* data){
    for (int i=0; i<29; i++){
        pastData[ADC_channel][i] = pastData[ADC_channel][i+1];
    }
    pastData[ADC_channel][29] = *data;
}




//This callback routine is called every 1ms
void TIMER2_InterruptSvcRoutine(uint32_t status, uintptr_t context){
    //Set flag to true to switch to new task
    task_FLAG = true;
    
    //Check if 30seconds have past
    if (ms_counter == 30000){
        ms_counter = 0;
    }
    RELAY7_Toggle();
}

#define MY_WORD_SWAP(x) ( ((x & 0xff00)>>8) | ((x & 0x00ff)<<8) )


int main ( void )
{
    SYS_Initialize ( NULL );
    

    TMR2_CallbackRegister(TIMER2_InterruptSvcRoutine, (uintptr_t)NULL);
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
    LED_RED_Set();
    
    
    while(1)
    {
        if (task_FLAG){
            switch (state)
            {
                case STATE_INITIALIZE:
                                        
                    //Send Commands to ADC to set mode to continuous 
                    ADC_Initialize();
                    
                    //Initialize Analog inputs
                    for (uint8_t i =0; i<4; i++){ 
                        inputs[i].analog_input = (ANALOG*)malloc(sizeof(ANALOG));
                    }
                    
                    //Set functions for easier calling
                    outputs[2].relay_set = (RELAY_CALLBACK)RELAY0_Set();
                    outputs[2].relay_clear = (RELAY_CALLBACK)RELAY0_Clear();
                    outputs[3].relay_set = (RELAY_CALLBACK)RELAY1_Set();
                    outputs[3].relay_clear = (RELAY_CALLBACK)RELAY1_Clear();
                    outputs[4].relay_set = (RELAY_CALLBACK)RELAY2_Set();
                    outputs[4].relay_clear = (RELAY_CALLBACK)RELAY2_Clear();                    
                    outputs[5].relay_set = (RELAY_CALLBACK)RELAY3_Set();
                    outputs[5].relay_clear = (RELAY_CALLBACK)RELAY3_Clear();
                    outputs[6].relay_set = (RELAY_CALLBACK)RELAY4_Set();
                    outputs[6].relay_clear = (RELAY_CALLBACK)RELAY4_Clear();
                    outputs[7].relay_set = (RELAY_CALLBACK)RELAY5_Set();
                    outputs[7].relay_clear = (RELAY_CALLBACK)RELAY5_Clear();
                    outputs[8].relay_set = (RELAY_CALLBACK)RELAY6_Set();
                    outputs[8].relay_clear = (RELAY_CALLBACK)RELAY6_Clear();
                    outputs[9].relay_set = (RELAY_CALLBACK)RELAY7_Set();
                    outputs[9].relay_clear = (RELAY_CALLBACK)RELAY7_Clear();
                    
                    state = ADC_SEND_CMD;
                    break;          

                case ADC_SEND_CMD: 
                    
                    ADC_Select_Chnl(ADC_channel/2);

                    state = ADC_READ;
                    break;
                    
                //In this state the ADC is read for the current input channel
                case ADC_READ:
                    //Read the data
                    ADC_Read_Data(&inputs[ADC_channel].analog_input->raw_data);
                    
                    //Store the data in past storage if 30 seconds have past
                    if (thirty_sec_passed){
                        pastDataUpdate(&inputs[ADC_channel].analog_input->scaled_data);
                        thirty_sec_passed = false;
                    }
                    ANALOG* input = inputs[ADC_channel].analog_input;
                    //Convert to user scale and save
                    input->scaled_data = ( (double)input->raw_data / (16777215) ) * (input->max - input->min) + input->min;
                    state = DISPLAY;
                    break;
                    
                case DISPLAY: //TODO
                    state = USER_INPUT_PERFORM_LOGIC;
                    break;
                    
                case USER_INPUT_PERFORM_LOGIC: 

                    state = CHECK_ALARMS;
                    break;

                //In this state the alarms are checked
                case CHECK_ALARMS:
                    for (uint8_t i=0; i<4; i++){
                        if (inputs[ADC_channel].alrms[i] != NULL){
                            //local pointers 
                            ALARM* alrm = inputs[ADC_channel].alrms[i];
                            
                            //Check if current input is Analog
                            if (inputs[ADC_channel].ang_dig){
                                ANALOG* input = inputs[ADC_channel].analog_input;
                                
                                //Check if alarm triggers at High or Low point
                                if (alrm->high_low){
                                    //Check if alarm should be triggered
                                    if (input->scaled_data > alrm->trigger){
                                        outputs[alrm->chnl_out].relay = true;
                                        outputs[alrm->chnl_out].alarm_count++;
                                    }else if (input->scaled_data < alrm->reset){
                                        //Check if other alarms trigger it this output before deactivating
                                        if (outputs[alrm->chnl_out].alarm_count>0){
                                            outputs[alrm->chnl_out].alarm_count--;
                                        }else{
                                            outputs[alrm->chnl_out].relay = false;
                                        }
                                        
                                    }
                                }else{
                                    if (input->scaled_data < alrm->trigger){
                                        outputs[alrm->chnl_out].relay = true;
                                    }
                                }                                
                                
                            }else{
                                INPUT* input = &inputs[ADC_channel];
                                
                                //Check if digital input is high
                                if (input->digital){
                                    outputs[alrm->chnl_out].relay = true;
                                    outputs[alrm->chnl_out].alarm_count++;
                                }else{
                                    //Check if other alarms trigger it this output before deactivating
                                    if (outputs[alrm->chnl_out].alarm_count>0){
                                        outputs[alrm->chnl_out].alarm_count--;
                                    }else{
                                        outputs[alrm->chnl_out].relay = false;
                                    }                                    
                                }
                            }
                            
                        }
                    }

                    
                    state = TRIGGER_ALARMS;
                    break;
                    
                case TRIGGER_ALARMS:
                    for(uint8_t i=2; i<10; i++) {
                        if (outputs[i].relay){
                            outputs[i].relay_set();
                        }else{
                            outputs[i].relay_clear();
                        }
                    }
                        
                    state = DAC1_SEND_CMD;
                    break;
                    
                case DAC1_SEND_CMD: 

                    SS_DAC1_Clear(); 
                    SPI1_Write(&outputs[ADC_channel], 2);
                    SS_DAC1_Set();
                    
                    state = DAC2_SEND_CMD; 
                    break;
                    
                case DAC2_SEND_CMD: 

                    SS_DAC2_Clear(); 
                    SPI1_Write(&outputs[ADC_channel], 2);
                    SS_DAC2_Set();
                    
                    state = INCREMENT_CHANNEL; 
                    break;

                
                case INCREMENT_CHANNEL:
                    
                    //Reset ADC Channel at max
                    if (ADC_channel==7)
                        ADC_channel = 0;
                    else
                        ADC_channel++;

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

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

