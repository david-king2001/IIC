/* 
 * File:   adc.h
 * Author: david
 *
 * Created on November 2, 2022, 5:30 PM
 */

#ifndef ADC_H
#define	ADC_H

#ifdef	__cplusplus
extern "C" {
#endif
    
    
    //
    // COMMMANDS
    //
    
    //Control Registers-ADC input range 2.56 ± VREF = 2.5 V
    #define Input_Range 0b111 
    
    //Mode Register
    #define MODE_Continuous 0b00100011

    //Mode Register Selections
    #define DATA 0b0100
    #define MODE 0b0001
    #define CONTROL 0b0010


    void ADC_Initialize();
    
    void ADC_Select_Chnl(uint8_t);
    
    void ADC_Read_Data();

#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */

