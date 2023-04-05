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
    

    #define ANALOG true
    #define DIGITAL false

    #define HIGH_LOW_ALARM true
    #define LOW_HIGH_ALARM false
    
    
    typedef struct {
        //! true-Relay Output, false-DAC Output
        bool rel_dac;

        //!The data sent out if the output is a DAC
        uint16_t data;
        

        //!true-relay on, false: relay off 
        bool relay;

        //ALARM **********

        //!Point that triggers the alarm
        double trigger;

        //!Point that resets alarm
        double reset;

        //!Input that triggers alarm
        int input_chnl;

        //!true: High-Low Alarm; set when value rises trigger and drops below reset
        //!false: Low-High Alarm; set when value drops to trigger and raise above reset
        bool high_low;

        
        
        //*****************
    } OUTPUT;


    /*!Stores data relevant to inputs. 
     * Includes pointers to ANALOG Struct is input is analog type
     * Includes list of 4 pointers to OUTPUTs used for alarms
     */
    typedef struct {
        //!true-Analog Input, false-Digital Input
        bool ang_dig;

        //!true-On, false-Off
        bool digital_on;

                //!The value corresponding to 20mA
        double max;
        //!The value corresponding to 4mA
        double min;

        //!The raw data received from ADC
        uint32_t raw_data;

        //!The data converted using max and min
        double scaled_data;

        //!Each input can have 4 alarms
        OUTPUT* alrms[4];
        
        //!Determines if input has been configured, false means input is ignored as it has not be configured
        bool is_set;
    } INPUT;
    
    extern INPUT inputs[8]; //!<4 Analog inputs from ADC (inputs[0-3]), 4 Digital inputs (inputs[4-7])
    extern OUTPUT outputs[10]; //!<2 Analog outputs to DAC (outputs[0-1]), 8 Relays (outputs[2-9])
    extern double pastData[4][30]; //!<Store history of input data
    extern bool converting;
    
    void ConfigureInput(INPUT* input, bool ang_dig, double max, double min, int input_num);
    
    bool CreateAnalogAlarm(OUTPUT* output, INPUT* input, double trigger, double reset, int input_chnl, bool high_low);
    
    bool CreateDigitalAlarm(OUTPUT* output, INPUT* input, int input_chnl);
    
    void DeleteAlarm(OUTPUT* output);
    
    
    
    void EditAlarm(OUTPUT* output, INPUT* input, double trigger, double reset, int input_chnl, int alarm_num, bool high_low);
    
    bool ConfigureAnalogOutput(OUTPUT* output, INPUT* input, int input_chnl, double max, double min );
    
    
#ifdef	__cplusplus
}
#endif

#endif	/* INPUT_OUTPUT_H */

