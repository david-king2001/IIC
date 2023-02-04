#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include "input_output.h"
//! Creates Alarm from Analog source
/*!
 This checks to make sure the desired output is not already configured as well
 as creates the alarm only if spot is open
 \param output the output that the alarm would be set to
 \param input the input the would trigger the alarm
 \param trigger a double that is the point the alarm triggers at
 \param reset a double this the point the alarm untriggers at
 \param input_chl an integer that is the channel number for the input
 \param high_low a bool of wether the alarm is High/Low or Low/High trigger type
 \return true on success, false on falure. Returns false if output already configured or input channel already has 4 alarms
 */
bool CreateAnalogAlarm(OUTPUT* output, INPUT* input, double trigger, double reset, short int input_chnl, bool high_low) {
    if (output->input_chnl != -1 || !input->is_set) {
        return false;
    } else {
        for (uint8_t i = 0; i < 4; i++) {
            //Check if alarm exist in list before adding
            if (input->alrms[i] == NULL) {
                output->input_chnl = input_chnl;
                output->trigger = trigger;
                output->reset = reset;
                output->high_low = high_low;
                input->alrms[i] = output;
                return true; //Exit loop returning success
            }

        }
    }
    
    return false;
};

//! Creates Alarm from digital source
/*!
 This checks to make sure the desired output is not already configured as well
 as creates the alarm only if spot is open
 \param output the output that the alarm would be set to
 \param input the input the would trigger the alarm
 \param input_chl an integer that is the channel number for the input
 \return true on success, false on falure. Returns false if output already configured or input channel already has 4 alarms
 */
bool CreateDigitalAlarm(OUTPUT* output, INPUT* input, short int input_chnl){
    if (output->input_chnl != -1 || !input->is_set) {
        return false;
    }else{
        for (uint8_t i = 0; i < 4; i++) {
            //Check if alarm exist in list before adding
            if (input->alrms[i] == NULL) {
                output->input_chnl = input_chnl;
                input->alrms[i] = output;
                return true; //Exit loop returning success
            }

        }
    }
    return false;
}


//! Delete Alarm
/*!
 This deletes an alarm by setting the input_chnl to -1;
 \param output the output that the alarm would be set to
 */
void DeleteAlarm(OUTPUT* output) {
    output->input_chnl = -1;
}
//! Edit existing Alarm
/*!
 This edits the parameter on the given output
 \param output the output that the alarm would be set to
 \param trigger a double that is the point the alarm triggers at
 \param reset a double this the point the alarm untriggers at
 \param input_chl an integer that is the channel number for the input
 \param high_low a bool of wether the alarm is High/Low or Low/High trigger type
 */
void EditAlarm(OUTPUT* output, double trigger, double reset, short int input_chnl, bool high_low) {
    output->input_chnl = input_chnl;
    output->trigger = trigger;
    output->reset = reset;
    output->high_low = high_low;
}

//! Configure the analog outputs
/*!
 * This configures which input to use for the analog outputs as well as the scale for the data
 * \param output, the output that is getting configured
 * \param input, the input driving the output, used to get the min/max of the input to ensure selected range in in bounds
 * \param input_chnl the input channel driving this output, 0-3 for the 4 analog inputs
 * \param max double, max value of input to trigger 20mA
 * \param min double, min value of input to trigger 4mA
 * \returns true on success, false on failure. Return false if: input_chnl not in range (0-3), input is not analog type, or select range of min-max is out of input range bounds 
 */
bool ConfigureAnalogOutput(OUTPUT* output, INPUT* input, short int input_chnl, double max, double min ){
    //Check valid input channel was sent
    if (input_chnl>3 && input_chnl<0) return false;
    
    //Input is not analog return false
    if (!input->ang_dig) return false;
    
    //request bound are outside selected input bounds, return false
    if (input->analog_input->max < max || input->analog_input->min > min) return false;
    
    //set values
    output->input_chnl = input_chnl;
    output->trigger = max;
    output->reset = min;
    return true;
}