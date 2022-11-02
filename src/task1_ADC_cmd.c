#include "task1_ADC_cmd.h"
#include "definitions.h"

TASK1_DATA task1Data;

/* Mutex used to protect the shared resource - UART */
SemaphoreHandle_t spiMutex;

volatile bool isTransferDone = false;
extern ADC adc;

void SPIEventHandler(uintptr_t context )
{    
    isTransferDone = true;
    /* De-assert the CS line */
    SS_ADC_Set();
    
}

void TASK1_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    task1Data.state = TASK1_STATE_INIT;

    /* Create a mutex type semaphore. */
    spiMutex = xSemaphoreCreateMutex();

    
}



void TASK1_Tasks(void){
    SPI1_CallbackRegister(SPIEventHandler, (uintptr_t) 0); 
    while(1){
        adc.data[0] = 0x16;
        adc.data[1] = 0x16;
        adc.data[2] = 0x16;
        
        if( xSemaphoreTake( spiMutex, portMAX_DELAY ) == pdTRUE ){
            LED_RED_Set(); 
            SS_ADC_Clear(); 
            SPI1_WriteRead(adc.data, 1, NULL, 0);
            if (isTransferDone) isTransferDone = false;
            xSemaphoreGive(spiMutex);
                  
            vTaskDelay(1000 / portTICK_PERIOD_MS );  
            LED_RED_Clear();
            vTaskDelay(1000 / portTICK_PERIOD_MS );             
        }else{
            LED_YELLOW_Set();
        }
        

    }

}