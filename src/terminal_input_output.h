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

    #define OUT_BUFFER 2048
    
    
    extern bool enter;
    //Terminal out with UART
    extern char buffer[OUT_BUFFER];
    extern char receive_buffer[128];
    extern char rec_char;
    extern int receive_buff_size;
    extern int nbytes;
    extern uint8_t reg_data;
    extern uint8_t cmd;
    
    
    void Terminal_Initialize();
        void PrintInstructions();
    
    void PrintRegister(uint8_t);
    
    void PrintAnalogInputs();
    
    void PrintDigitalInputs();
    
    void PrintAnalogOutputs();
    
    void PrintRelays();
    
    void PrintAlarmSettings();
    
    void ParseInputForAlarm(char* input_string);
    
    void ParseInputForInput(char* input_string);
    
    void ParseInputForOutput(char* input_string);
    
    void PrintHistory();

#ifdef	__cplusplus
}
#endif

#endif	/* TERMINAL_INPUT_OUTPUT_H */

