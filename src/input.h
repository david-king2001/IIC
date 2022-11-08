/* 
 * File:   input.h
 * Author: david
 *
 * Created on November 6, 2022, 9:26 PM
 */

#ifndef INPUT_H
#define	INPUT_H

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

    }INPUT_ANLG;
    


#ifdef	__cplusplus
}
#endif

#endif	/* INPUT_H */

