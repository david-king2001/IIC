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

    //!Control Registers-ADC input range 2.56 ± VREF = 2.5 V
    #define Input_Range 0b111 

    //!Command for Continuous mode
    #define MODE_Continuous 0b00000011

    //Register Selections
    #define DATA 0b0100 //!< Select Data Register cmd
    #define MODE 0b0001 //!< Select Mode Register cmd
    #define CONTROL 0b0010 //!< Select Control Register cmd
    #define STATUS 0b0000 //!< Select STATUS Register cmd
    #define ID 0b1111   //!< Select ID Register cmd
    #define FILTER 0b0011 //!< Select FILTER Register cmd
    
    #define READ 0b01000000 //!< READ cmd
    #define WRITE 0b00000000 //!< WRITE cmd
    

    //!Initialize ADC at power on
    /*!
     * Send command to ADC Communication Register to select MODE register\n
     * Send command to ADC MODE Register to select Continuous Conversion mode
     */
    void ADC_Initialize();

    //!Selects Channel
    /*!
     * Send command to ADC Communication Register to select Control register\n
     * Send command to ADC Control Register to select channel
     * \param chnl a integer with the selected input channel
     */
    void ADC_Select_Chnl(uint8_t);
    
    //!Reads data from ADC
    /*!
     * Send command to ADC Communication Register to select data register\n
     * Read from ADC data Register into input buffer
     * \param RxData a pointer to start of buffer that stores that channels data
     */
    void ADC_Read_Data(uint8_t* RxData);
    
    //!Reads data from selected register ADC
    /*!
     * Send command to ADC Communication Register to select register\n
     * Read from ADC Register into input buffer
     * \param reg the register to read from
     */
    uint8_t ADC_Read_Registor(uint8_t reg);

#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */

