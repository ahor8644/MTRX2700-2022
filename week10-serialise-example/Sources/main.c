#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

#include <stdio.h>
#include <stdlib.h>

#include "simple_serial.h"


struct MSG_HEADER{	//tells whoever is reading it, 'what is to come next?'
  int sentinel;		//Indicating the start of the header
  char msg_type[8];	//tells python what type of message is coming (button, gyro...)
  unsigned int msg_size;	//how big is the message going to be? (no neccesarily needed)
  unsigned int header_time;	//read from TCNT (what was the time at which this header was sent?)
  int end_sentinel;	//indicating the end of the header
};

struct MSG_GYRO{
  int sentinel;
  int rotation_x;
  int rotation_y;
  int rotation_z;
  unsigned int last_sample_time;
};

struct MSG_BUTTONS{
  int sentinel;
  unsigned char button_states;
  unsigned int last_press_time;
};

void SendGyroMsg(int rot_x, int rot_y, int rot_z) {		//sending gyro message
  struct MSG_HEADER gyro_header = {0xABCD, "gyro", 0, 0, 0xDCBA};	//make a gyro header
  struct MSG_GYRO gyro_message = {0x9876, 0, 0, 0, 0};			//make an arbitrary message (to be filled in)
  
  //FILL IN THE HEADER AND GYRO MESSAGE CONTENTS
  gyro_header.msg_size = sizeof(struct MSG_GYRO);	//message size
  gyro_header.header_time = TCNT;
  
  gyro_message.last_sample_time = TCNT;			
  gyro_message.rotation_x = rot_x;
  gyro_message.rotation_y = rot_y;
  gyro_message.rotation_z = rot_z;
  
  SerialOutputBytes((char*)&gyro_header, sizeof(struct MSG_HEADER), &SCI1);  //outputting the HEADER
  SerialOutputBytes((char*)&gyro_message, sizeof(struct MSG_GYRO), &SCI1);  //outputting the actual message/gyro data
}

void SendButtonsMsg() {
  struct MSG_HEADER button_header = {0xABCD, "buttons", 0, 0, 0xDCBA};
  struct MSG_BUTTONS button_message = {0x4321, 0x0A, 0};
                             
  button_header.msg_size = sizeof(struct MSG_BUTTONS);
  button_header.header_time = TCNT;
  
  button_message.last_press_time = TCNT;
  
  SerialOutputBytes((char*)&button_header, sizeof(struct MSG_HEADER), &SCI1);  
  SerialOutputBytes((char*)&button_message, sizeof(struct MSG_BUTTONS), &SCI1);   
}


void SendTextMsg(char* text_message) {
  struct MSG_HEADER text_header = {0xABCD, "text", 0, 0, 0xDCBA};
  text_header.msg_size = strlen(text_message);
  text_header.header_time = TCNT;
  
  SerialOutputBytes((char*)&text_header, sizeof(struct MSG_HEADER), &SCI1);  
  SerialOutputBytes(text_message, text_header.msg_size, &SCI1);
}




void main(void) {
  /* put your own code here */
  char text_buffer[32];
  int rot_x = 0;
  int rot_y = 10;
  int rot_z = 100;

  _DISABLE_COP();

  
  // initialise the serial
  SerialInitialise(BAUD_115200, &SCI1);
  
  // initialise the timer
  TSCR1_TEN = 1;  
  TSCR2_PR = 0b111;
  
  sprintf(text_buffer, "first 12345");
  SendTextMsg(text_buffer);
  SendTextMsg(text_buffer);
  SendTextMsg(text_buffer);
  
  sprintf(text_buffer, "second 123456");
  SendTextMsg(text_buffer);
  SendTextMsg(text_buffer);
  SendTextMsg(text_buffer);
  
	EnableInterrupts;

  SendButtonsMsg();
  SendButtonsMsg();
  SendButtonsMsg();
  
  _DISABLE_COP();

  for(;;) {
  
    SendGyroMsg(rot_x, rot_y, rot_z);
    
    //_FEED_COP(); /* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}
