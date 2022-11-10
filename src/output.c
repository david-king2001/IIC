#include "definitions.h"

//! Creates Alarm
/*!
 This checks to make sure the desired output is not already configured as well
 as creates the alarm only if spot is open
 \param output the output that the alarm would be set to
 \param input the input the would trigger the alarm
 \param trigger a double that is the point the alarm triggers at
 \param reset a double this the point the alarm untriggers at
 \param input_chl an integer that is the channel number for the input
 \param high_low a bool of wether the alarm is High/Low or Low/High trigger type
 */
bool CreateAlarm(OUTPUT* output, INPUT* input, double trigger, double reset, short int input_chnl, bool high_low) {
    if (output->input_chnl != -1) {
        return false;
    } else {
        for (uint8_t i = 0; i < 4; i++) {
            //Check if alarm exist in list before adding
            if (input->alrms[i] != NULL) {
                output->input_chnl = input_chnl;
                output->trigger = trigger;
                output->reset = reset;
                output->high_low = high_low;
                return true; //Exit loop returning success
            }

        }
    }
    return false;
};

void DeleteAlarm(OUTPUT* output) {
    output->input_chnl = -1;
}

void EditAlarm(OUTPUT* output, double trigger, double reset, short int input_chnl, bool high_low, RELAY_CALLBACK relay_set, RELAY_CALLBACK relay_clear) {
    output->input_chnl = input_chnl;
    output->trigger = trigger;
    output->reset = reset;
    output->high_low = high_low;
}
