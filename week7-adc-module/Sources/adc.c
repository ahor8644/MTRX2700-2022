#include "adc.h"

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

//all ADC interfaces are stored in an array --> this array is filled in by the 'RegisterADC()' function
ADCInterface interfaces[16];
int interface_count = 0;


void RegisterADC(int channel_to_read, ADCHandler callback_function) {
  //This function takes in an ADC channel to read from, as well as a callback
  //function, and stores them in the array of ADCInterface structs using a global counter
  
  
  interfaces[interface_count].channel_to_read = channel_to_read; 
  interfaces[interface_count].callback_function = callback_function;
  
  interface_count += 1;   //incrementing array counter 
}
  



// the interrupt for timer 6 which is used for a iic timeout
#pragma CODE_SEG __NEAR_SEG NON_BANKED /* Interrupt section for this module. Placement will be in NON_BANKED area. */
__interrupt void TC6_ISR(void) { 
   
  int counter = 0;
   
  TC6 =TCNT + 24000;   // interrupt at 100Hz assuming prescaler =1
  TFLG1=TFLG1 | TFLG1_C6F_MASK;   //reset flag --> enable channel 6 interrupt to happen again

  PORTB = 0x00;   //turn off LEDs

  //convert each input (pot and LDR) to digital
  for (counter = 0; counter < interface_count; counter++) {
   
    ATD0CTL5 = interfaces[counter].channel_to_read;   //starting the conversion by writing to CTL5
  
    // wait for the ADC to finish
    while(!(ATD0STAT0 & 0x80));
    
    // call the callback associated with this ADC channel    
    interfaces[counter].callback_function(ATD0DR0L);    //input to the callback_function is the conversion result! (unsigned char)
  }
}


// register the timer callback with the TIMER 6 ISR vector
typedef void (*near tIsrFunc)(void);

const tIsrFunc _vect @Vtimch6 = TC6_ISR;
