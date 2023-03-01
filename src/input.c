#include "definitions.h"
#include "input_output.h"



void ConfigureInput(INPUT* input, bool ang_dig, double max, double min, int input_num){
    
    //New input delete old data
    for (int i=0; i<30; i++){
        pastData[input_num][i] = min;
    }
    
    input->ang_dig = ang_dig;
    
    if (ang_dig){
        input->max = max;
        input->min = min;        
    }

    
    input->is_set=true;
}
