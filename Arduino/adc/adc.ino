#include<SPI.h>

volatile boolean received;
volatile byte rec, out;
volatile byte command = 0;
byte data[3] = {0};
int data_i = 0;
unsigned int val;

ISR (SPI_STC_vect)        //Inerrrput routine function 
{
  byte c = SPDR;
   received = true;
    switch (command)
    {
    // no command? then this is the command
    case 0:
      command = c;
      data_i = 0;
      if (c == 0x4){
        SPDR = data[0];
        data_i++;
      }else{
        SPDR=0;
      }
      break;

    case 0x4:
      SPDR = data[data_i];
      data_i++;
      break;
      
    default:
      SPDR=0;
      break;
    } // end of switch
}

void setup()
{
  pinMode(MISO,OUTPUT);   //Sets MISO as OUTPUT
  SPCR |= _BV(SPE);       //Turn on SPI in Slave Mode
  received = false;
  SPI.attachInterrupt();  //Activate SPI Interuupt 
  //Serial.begin(9600); // send and receive at 9600 baud
}


void loop()
{ 
  if (digitalRead (SS) == HIGH)
    command = 0;

  val = analogRead(A0);
  data[0] = (byte)val;
  data[1] = (byte)(val >> 8);
  data[2] = 0;
  //Serial.println((int)(data[0])+(int)(data[1] <<8));
//  if (received){
//    if (data[2] == 255){
//      data[0] = 0;
//      data[1] = 0;
//      data[2] = 0;
//    }else if (data[1] == 255){
//      data[2]++;
//    }else if (data[0] == 255){
//      data[1]++;
//    }else{
//      data[0]++;
//    }
//    received = 0;
//  }
    
}
