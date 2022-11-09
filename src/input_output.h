/* 
 * File:   input_output.h
 * Author: david
 *
 * Created on November 9, 2022, 4:25 PM
 */

#ifndef INPUT_OUTPUT_H
#define	INPUT_OUTPUT_H

#ifdef	__cplusplus
extern "C" {
#endif
    

    typedef void (*RELAY_CALLBACK) ();

    typedef struct {
        //! true-Relay Output, false-DAC Output
        bool rel_dac;

        //!The data sent out if the output is a DAC
        uint16_t data;

        //!true-relay on, false: relay off 
        bool relay;

        //ALARM **********

        //!Alarm functions to turn on and off relay
        RELAY_CALLBACK relay_set;
        //!Alarm functions to turn on and off relay
        RELAY_CALLBACK relay_clear;

        //!Point that triggers the alarm
        double trigger;

        //!Point that resets alarm
        double reset;

        //!Input that triggers alarm
        short int input_chnl;

        //!true: High-Low Alarm; set when value rises trigger and drops below reset
        //!false: Low-High Alarm; set when value drops to trigger and raise above reset
        bool high_low;

        //*****************
    } OUTPUT;

    
    typedef uint8_t(*DIGITAL_INPUT_CALLBACK) ();

    typedef struct {
        //!The value corresponding to 20mA
        double max;
        //!The value corresponding to 4mA
        double min;

        //!The raw data received from ADC
        uint32_t raw_data;

        //!The data converted using max and min
        double scaled_data;


    } ANALOG;

    typedef struct {
        //!true-Analog Input, false-Digital Input
        bool ang_dig;

        //!true-On, false-Off
        bool digital;

        //!Callback to function to get state of GPIO pin
        DIGITAL_INPUT_CALLBACK digital_get;

        //!Pointer to analog input if input is analog type
        ANALOG* analog_input;

        //!Each input can have 4 alarms
        OUTPUT* alrms[4];
    } INPUT;

    bool CreateAlarm(OUTPUT* output, INPUT* input, double trigger, double reset, short int input_chnl, bool high_low);
    
    void DeleteAlarm(OUTPUT* output);
    
    void EditAlarm(OUTPUT* output, double trigger, double reset, short int input_chnl, bool high_low, RELAY_CALLBACK relay_set, RELAY_CALLBACK relay_clear);
#ifdef	__cplusplus
}
#endif

#endif	/* INPUT_OUTPUT_H */

