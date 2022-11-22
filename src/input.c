#include "definitions.h"
void ConfigureInput(INPUT* input, bool ang_dig, double max, double min){
    
    input->ang_dig = ang_dig;
    
    if (ang_dig){
        input->analog_input->max = max;
        input->analog_input->min = min;        
    }

    
    input->is_set=true;
}
