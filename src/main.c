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
    ADC_STATE_TRANSFER_COMPLETE_WAIT,       
    ADC_SEND_CMD,
    ADC_READ,
    DAC_STATE_TRANSFER_COMPLETE_WAIT,  
    DAC_SEND_CMD,
    DISPLAY_STATE_TRANSFER_COMPLETE_WAIT,
    DISPLAY_SEND_CMD,
    USER_INPUT_PERFORM_LOGIC
} STATES;



volatile bool isSPI1Done = false;
volatile bool isSPI2Done = false;

STATES state = STATE_INITIALIZE;
STATES nextState = STATE_INITIALIZE;

ADC adc;


void SPI1EventHandler(uintptr_t context )
{    
    isSPI1Done = true;
    if (SS_ADC_Get()){
        SS_ADC_Set();
    }
    if (SS_DAC_Get()){
        SS_DAC_Set();
    }
}



void SPI2EventHandler(uintptr_t context )
{    
    isSPI2Done = true;

    /* De-assert the CS line */
    SS_DISPLAY_Set();
}




int main ( void )
{
    SYS_Initialize ( NULL );
    
    LED_RED_Clear();
    LED_YELLOW_Clear();
    LED_GREEN_Clear();
    
    SS_ADC_Set();
    SS_DAC_Set();
    SS_DISPLAY_Set();
    
    while (SW1_Get() == 1);
    LED_YELLOW_Set();
    
    while(1)
    {
        switch (state)
        {
            case STATE_INITIALIZE:
                SPI1_CallbackRegister(SPI1EventHandler, (uintptr_t) 0); 
                state = ADC_SEND_CMD;
                
                break;          
            
            case ADC_STATE_TRANSFER_COMPLETE_WAIT:
                for(int i=0; i<10000; i++);
                if (isSPI1Done){
                    isSPI1Done = false;
                    //while(RDY_Get());
                    state = nextState; 
                }
                break;
                
            case DAC_STATE_TRANSFER_COMPLETE_WAIT:
                for(int i=0; i<10000; i++);
                if (isSPI1Done){
                    isSPI1Done = false;
                    state = nextState; 
                }
                break;
                
            case DISPLAY_STATE_TRANSFER_COMPLETE_WAIT:
                if (isSPI2Done){
                    isSPI2Done = false;
                   state = nextState; 
                }
                
                break;

            case ADC_SEND_CMD: 
                adc.cmd = 1;
                SS_ADC_Clear(); 
                SPI1_Write(&adc.cmd, 1);
                state = ADC_STATE_TRANSFER_COMPLETE_WAIT;
                nextState = ADC_READ;
                break;
                
            case ADC_READ:
                SS_ADC_Clear(); 
                SPI1_Read(adc.data[0], 3);
                state = ADC_STATE_TRANSFER_COMPLETE_WAIT;
                nextState = DAC_SEND_CMD;
                break;
                
            case DAC_SEND_CMD: 
                adc.cmd = 2;
                SS_DAC_Clear();
                SPI1_Write(&adc.cmd, 1);
                state = DAC_STATE_TRANSFER_COMPLETE_WAIT;
                nextState = USER_INPUT_PERFORM_LOGIC;
                break;
                
                
            case USER_INPUT_PERFORM_LOGIC: 
                state = DISPLAY_SEND_CMD;
                break;
                
            case DISPLAY_SEND_CMD:
                //state = DISPLAY_STATE_TRANSFER_COMPLETE_WAIT;
                state = ADC_SEND_CMD;
                break;
                
            default:
                break;
        }
    }
    

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

