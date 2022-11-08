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
    ALARMS,
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

INPUT_ANLG inputs[4]; //4 Analog inputs from ADC
uint8_t digital_inputs; //4 Digital inputs
ALARM alarms[32]; //32 possible alarms
double pastData[4][30]; //Store history of input data
uint16_t dac_data[2]; //2 Analog outputs to DAC 

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

uint8_t input_channel = 0; 
uint16_t ms_counter = 0;

//************************

void pastDataUpdate(double* data){
    for (int i=0; i<29; i++){
        pastData[input_channel][i] = pastData[input_channel][i+1];
    }
    pastData[input_channel][29] = *data;
}




//This callback routine is called every 1ms
void TIMER2_InterruptSvcRoutine(uint32_t status, uintptr_t context){
    //Set flag to true to switch to new task
    task_FLAG = true;
    
    //Check if 30seconds have past
    if (ms_counter == 30000){
        ms_counter = 0;
    }
    ALARM7_Toggle();
}

#define MY_WORD_SWAP(x) ( ((x & 0xff00)>>8) | ((x & 0x00ff)<<8) )


int main ( void )
{
    SYS_Initialize ( NULL );
    

    TMR2_CallbackRegister(TIMER2_InterruptSvcRoutine, (uintptr_t)NULL);
    TMR2_Start();
    
    //Set GPIO pins to low for not in use
    LED_RED_Clear();
    ALARM0_Clear();
    ALARM1_Clear();
    ALARM2_Clear();
    ALARM3_Clear();
    ALARM4_Clear();
    ALARM5_Clear();       
    ALARM6_Clear();
    ALARM7_Clear();
    
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
                    
                    state = ADC_SEND_CMD;
                    break;          

                case ADC_SEND_CMD: 
                    ADC_Select_Chnl(input_channel);

                    state = ADC_READ;
                    break;
                    
                //In this state the ADC is read for the current input channel
                case ADC_READ:
                    //Read the data
                    ADC_Read_Data(&inputs[input_channel].raw_data);
                    
                    //Store the data in past storage if 30 seconds have past
                    if (thirty_sec_passed){
                        pastDataUpdate(&inputs[input_channel].scaled_data);
                        thirty_sec_passed = false;
                    }

                    //Convert to user scale and save
                    inputs[input_channel].scaled_data = ( (double)inputs[input_channel].raw_data / (16777215) ) * (inputs[input_channel].max - inputs[input_channel].min) + inputs[input_channel].min;
                    state = DISPLAY;
                    break;
                    
                case DISPLAY: //TODO
                    state = USER_INPUT_PERFORM_LOGIC;
                    break;
                    
                case USER_INPUT_PERFORM_LOGIC: 

                    state = ALARMS;
                    break;

                //In this state 
                case ALARMS:
                    for (uint8_t i=0; i<32; i++){
                        if (alarms[i].chnl_trig == input_channel+4){
                            
                        }
                    }


                    
                    state = DAC1_SEND_CMD;
                    break;
                    
                case DAC1_SEND_CMD: 

                    SS_DAC1_Clear(); 
                    SPI1_Write(&dac_data[input_channel/2], 2);
                    SS_DAC1_Set();
                    
                    state = DAC2_SEND_CMD; 
                    break;
                    
                case DAC2_SEND_CMD: 

                    SS_DAC2_Clear(); 
                    SPI1_Write(&dac_data[input_channel/2], 2);
                    SS_DAC2_Set();
                    
                    state = INCREMENT_CHANNEL; 
                    break;

                
                case INCREMENT_CHANNEL:
                    
                    //Reset Input Channel at max
                    if (input_channel==3)
                        input_channel = 0;
                    else
                        input_channel++;

                    
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

