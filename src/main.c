/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

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
    DAC1_SEND_CMD,
    DAC2_SEND_CMD,
    DISPLAY_SEND_CMD,
    USER_INPUT_PERFORM_LOGIC,
    INCREMENT_CHANNEL
} STATES;

STATES state = STATE_INITIALIZE;
STATES nextState = STATE_INITIALIZE;

// ***********************
// DATA STORAGE
// ***********************
INPUT inputs[4];
ALARM alarms[32];


uint16_t dac_data[2]; //Store data to be written to DACs size 16bits
uint32_t pastData[4][30]; //Store history of input data
uint32_t adc_data[4]; //Store data from 4 analog inputs size 24bits
uint8_t adc_cmd; //8 bit command for adc size 8 bits
uint8_t alarms = 0; //Store which alarms are active
uint8_t digital = 0; //Store which 


// ***********************
// FLAGS
// ***********************
volatile bool task_FLAG = false;

// ***********************
// COUNTERS
// ***********************

uint8_t Input_Channel = 0; 
uint8_t Output_Channel = 0;



void pastDataUpdate(uint32_t* data){
    for (int i=0; i<29; i++){
        pastData[Input_Channel][i] = pastData[Input_Channel][i+1];
    }
    pastData[Input_Channel][29] = *data;
}


void SPI1EventHandler(uintptr_t context )
{   

    //If DAC1 turn off DAC
    if (SS_DAC1_Get()==0){
        SS_DAC1_Set();
    }
    
    //If DAC2 turn off DAC
    if (SS_DAC2_Get()==0){
        SS_DAC2_Set();
    }
}


//TODO For display SPI
//void SPI2EventHandler(uintptr_t context )
//{    
//
//}

void TIMER2_InterruptSvcRoutine(uint32_t status, uintptr_t context){
    task_FLAG = true;
    ALARM6_Toggle();
}

#define MY_WORD_SWAP(x) ( ((x & 0xff00)>>8) | ((x & 0x00ff)<<8) )

int main ( void )
{
    SYS_Initialize ( NULL );
    
    SPI1_CallbackRegister(SPI1EventHandler, (uintptr_t) 0); 
    //SPI2_CallbackRegister(SPI2EventHandler, (uintptr_t) 0); 

    TMR2_CallbackRegister(TIMER2_InterruptSvcRoutine, (uintptr_t)NULL);
    TMR2_Start();
    
    LED_RED_Clear();
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
                    ADC_Select_Chnl();

                    state = ADC_READ;
                    break;

                case ADC_READ:
                    ADC_Read_Data(inputs[Input_Channel].data);
                    pastDataUpdate(&adc_data[Input_Channel]);
                    
                    //TEST
                    
                    adc_data[0]=(uint32_t)0xABCDEF;
                    adc_data[1]=(uint32_t)0xABCDEF;
                    dac_data[0] = MY_WORD_SWAP(dac_data[0]);
                    dac_data[1] = MY_WORD_SWAP(dac_data[1]);
                    //
                    state = DAC1_SEND_CMD;
                    break;

                case DAC1_SEND_CMD: 
                    while(SPI1_IsBusy());
                    SS_DAC1_Clear(); 
                    
                    SPI1_Write(&dac_data[Output_Channel/4], 2);
                    while(SPI1_IsBusy());
                    
                    state = DAC2_SEND_CMD; 
                    break;
                    
                case DAC2_SEND_CMD: 
                    while(SPI1_IsBusy());
                    SS_DAC2_Clear(); 

                    SPI1_Write(&dac_data[Output_Channel/4], 2);
                    while(SPI1_IsBusy());
                    
                    state = USER_INPUT_PERFORM_LOGIC; 
                    break;

                case USER_INPUT_PERFORM_LOGIC: //TODO
                    dac_data[0] = adc_data[0]>>8; 
                    dac_data[1] = adc_data[1]>>8; 
                    state = DISPLAY_SEND_CMD;
                    break;

                case DISPLAY_SEND_CMD: //TODO
                    state = INCREMENT_CHANNEL;
                    break;
                
                case INCREMENT_CHANNEL:
                    
                    //Reset Input Channel at max
                    if (Input_Channel==3)
                        Input_Channel = 0;
                    else
                        Input_Channel++;
                    
                    //Reset Output Channel at max
                    if (Output_Channel==7)
                        Output_Channel = 0;
                    else
                        Output_Channel++;
                    
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

