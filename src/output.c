#include "definitions.h"

//! Creates Alarm
/*!
 This checks to make sure the desired output is not already configured as well
 as creates the alarm only if spot is open
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
