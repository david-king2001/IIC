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
    typedef enum{
        test = 213
    } ADC_CMD;
    
    
    typedef struct{
        uint8_t data[3][2]; //4 analog inputs with 24bits each
        uint8_t cmd; //8 bit command for adc 
    } ADC;


#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */

