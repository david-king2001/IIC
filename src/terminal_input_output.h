/* 
 * File:   terminal_input_output.h
 * Author: david
 *
 * Created on February 3, 2023, 11:25 PM
 */

#ifndef TERMINAL_INPUT_OUTPUT_H
#define	TERMINAL_INPUT_OUTPUT_H

#ifdef	__cplusplus
extern "C" {
#endif


    bool errorStatus;
    bool writeStatus;
    bool readStatus;
    //Terminal out with UART
    char buffer[512];
    char receive_buffer[128];
    char rec_char;
    int receive_buff_size;
    int nbytes;
    uint8_t reg_data;
    uint8_t cmd;
    
    void Terminal_Initialize();
    
    void PrintRegister(uint8_t);

#ifdef	__cplusplus
}
#endif

#endif	/* TERMINAL_INPUT_OUTPUT_H */

