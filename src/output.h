/* 
 * File:   output.h
 * Author: david
 *
 * Created on November 6, 2022, 9:16 PM
 */

#ifndef OUTPUT_H
#define	OUTPUT_H

#ifdef	__cplusplus
extern "C" {
#endif


    
    
    typedef struct{
        //Point that triggers the alarm
        int trigger; 
        
        //Point that resets alarm
        int reset; 
        
        //Channel that gets set by the alarm
        uint8_t chnl_out; 
        
        //Channel that is used to trigger the alarm
        //0-3 analog inputs; 4-7 digital inputs
        uint8_t chnl_trig; 
        
        //true: High-Low Alarm; set when value rises trigger and drops below reset
        //false: Low-High Alarm; set when value drops to trigger and raise above reset
        bool high_low; 
    }ALARM;

    
#ifdef	__cplusplus
}
#endif

#endif	/* OUTPUT_H */

