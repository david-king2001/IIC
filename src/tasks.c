#include "definitions.h"
#include "device.h"

// *****************************************************************************
// *****************************************************************************
// Section: RTOS "Tasks" Routine
// *****************************************************************************
// *****************************************************************************
/* Handle for the TASK1_Tasks. */
TaskHandle_t xTASK1_Tasks;

void _TASK1_Tasks(  void *pvParameters  )
{   
    while(1)
    {
        TASK1_Tasks();
    }
}





// *****************************************************************************
// *****************************************************************************
// Section: System "Tasks" Routine
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void SYS_Tasks ( void )

  Remarks:
    See prototype in system/common/sys_module.h.
*/
void SYS_Tasks ( void )
{
    /* Maintain system services */
    

    /* Maintain Device Drivers */
    

    /* Maintain Middleware & Other Libraries */
    

    /* Maintain the application's state machine. */
        /* Create OS Thread for TASK1_Tasks. */
    xTaskCreate((TaskFunction_t) _TASK1_Tasks,
                "ADC Sending Cmd",
                1024,
                NULL,
                1,
                &xTASK1_Tasks);

    /* Create OS Thread for TASK2_Tasks. */


    /* Start RTOS Scheduler. */
    
     /**********************************************************************
     * Create all Threads for APP Tasks before starting FreeRTOS Scheduler *
     ***********************************************************************/
    vTaskStartScheduler(); /* This function never returns. */

}

