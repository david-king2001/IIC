#include "definitions.h"
#include "input_output.h"



void ConfigureInput(INPUT* input, bool ang_dig, double max, double min){
    
    input->ang_dig = ang_dig;
    
    if (ang_dig){
        input->max = max;
        input->min = min;        
    }

    
    input->is_set=true;
}
