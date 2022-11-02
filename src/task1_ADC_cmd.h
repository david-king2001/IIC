/* 
 * File:   task1_ADC_cmd.h
 * Author: david
 *
 * Created on November 1, 2022, 2:02 PM
 */


#ifndef TASK1_ADC_CMD_H
#define	TASK1_ADC_CMD_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#ifdef	__cplusplus
extern "C" {
#endif


typedef enum
{
    /* Application's state machine's initial state. */
    TASK1_STATE_INIT=0,
    TASK1_STATE_SERVICE_TASKS,
    /* TODO: Define states used by the application state machine. */

} TASK1_STATES;


typedef struct
{
    /* The application's current state */
    TASK1_STATES state;

    /* TODO: Define any additional data used by the application. */

} TASK1_DATA;

void TASK1_Initialize ( void );

void TASK1_Tasks( void );



#ifdef	__cplusplus
}
#endif

#endif	/* TASK1_ADC_CMD_H */

