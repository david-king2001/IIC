#include "definitions.h"
#include "input_output.h"



void ConfigureInput(INPUT* input, bool ang_dig, double max, double min, int input_num){
    
    
    //Configuration only needed for Analog inputs
    if (input_num > 3) return;
            
    //New input delete old data  
    for (int i=0; i<30; i++){
        pastData[input_num][i] = min;
    }
    
    //New input delete all links
    for (int i=0; i<4; i++){
        if (input->alrms[i] ) {
            input->alrms[i]->input_chnl = -1;
            outputs[inputs[input_num].alrms[i]-outputs].relay=false;
        }
    }
    
    //Clear linked outputs
    if (outputs[0].input_chnl == input_num) outputs[0].input_chnl = -1;
    //Clear linked outputs
    if (outputs[1].input_chnl == input_num) outputs[1].input_chnl = -1;
    
    input->ang_dig = ang_dig;
    
    if (ang_dig){
        input->max = max;
        input->min = min;        
    }

    
    input->is_set=true;
}

void DeleteInput(INPUT* input){
    input->is_set = false;
    
    for (int i=0; i<4; i++)
        input->alrms[i]->input_chnl = -1;
}