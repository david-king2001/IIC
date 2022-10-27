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
	APP_STATE_INITIALIZE,
    APP_STATE_TRANSFER_COMPLETE_WAIT,       
    APP_STATE_SEND_READ_DATA_CMD,
    APP_STATE_READ_DATA,
    APP_STATE_VERIFY,
    APP_STATE_ERROR,

} APP_STATES;

#define ADC_Read_Data    0x44
APP_STATES state = APP_STATE_INITIALIZE;
APP_STATES nextState;
volatile bool isTransferDone = false;

void SPIEventHandler(uintptr_t context )
{    
    isTransferDone = true;

    /* De-assert the CS line */
    SS_ADC_Set();
}
uint8_t adcDATA[3];
uint8_t td[1];
uint8_t a[1];

int main ( void )
{
    SYS_Initialize ( NULL );
    
    a[0] = 'A';
    LED_RED_Clear();
    LED_YELLOW_Clear();
    LED_GREEN_Clear();
    
    SS_ADC_Set();
    
    while (SW1_Get() == 1);
    LED_YELLOW_Set();
    
    while(1)
    {
        switch (state)
        {
            case APP_STATE_INITIALIZE:
                SPI1_CallbackRegister(SPIEventHandler, (uintptr_t) 0); 
                state = APP_STATE_SEND_READ_DATA_CMD;
                
                break;          
            
            case APP_STATE_TRANSFER_COMPLETE_WAIT:
                if (isTransferDone == true)
                {
                    isTransferDone = false;      
                    /* Wait for the SPI slave to become ready before sending next commands */ 
                    while (RDY_Get() == 1);
                    state = nextState;
                }
                break;

            case APP_STATE_SEND_READ_DATA_CMD: 
                LED_RED_Clear();
                SS_ADC_Clear(); 
                td[0]= ADC_Read_Data;
                SPI1_WriteRead(td, 1, NULL, 0);
                state = APP_STATE_TRANSFER_COMPLETE_WAIT;
                nextState = APP_STATE_READ_DATA;
                break;
                
            case APP_STATE_READ_DATA:
                SS_ADC_Clear(); 
                SPI1_WriteRead(NULL, 0, adcDATA, 1);
                state = APP_STATE_TRANSFER_COMPLETE_WAIT;
                nextState = APP_STATE_VERIFY;
                break;
                
            case APP_STATE_VERIFY: 
                if (memcmp(adcDATA, a, 1) == 0)
                {
                    LED_GREEN_Set();    
                    LED_RED_Clear();
                    /* Repeat the test */
                    state = APP_STATE_SEND_READ_DATA_CMD;
                }else{
                    LED_RED_Set();
                    LED_GREEN_Clear();
                    state = APP_STATE_ERROR;
                }
                LED_YELLOW_Clear();
                break;
                
            case APP_STATE_ERROR:
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

