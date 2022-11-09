/* 
 * File:   input.h
 * Author: david
 *
 * Created on November 6, 2022, 9:26 PM
 */

#ifndef INPUT_H
#define	INPUT_H

#include "output.h"

#ifdef	__cplusplus
extern "C" {
#endif
    
    typedef struct{
        //The value corresponding to 20mA
        int max;
        //The value corresponding to 4mA
        int min;
        //name
        
        //The raw data received from ADC
        uint32_t raw_data;
        
        //The data converted using max and min
        double scaled_data;
        
        //Check if channel is used
        bool set; 
        
    }ANALOG;
    
    typedef struct{
        //true-Analog Input, false-Digital Input
        bool ang_dig;
        
        //true-On, false-Off
        bool digital;
        
        //Pointer to analog input if input is analog type
        ANALOG* analog_input;
        
        //Each input can have 4 alarms
        ALARM* alrms[4];
    }INPUT;
    
    
    


#ifdef	__cplusplus
}
#endif

#endif	/* INPUT_H */

