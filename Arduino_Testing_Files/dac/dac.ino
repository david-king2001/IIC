#include<SPI.h>

volatile byte command = 0;
byte data[2] = {0};
bool data_i = 0;
unsigned int val;

ISR (SPI_STC_vect)        //Inerrrput routine function 
{
  byte c = SPDR;
  data[data_i]=c;
  data_i = !data_i;
  SPDR=0;
}

void setup()
{
  pinMode(MISO,OUTPUT);   //Sets MISO as OUTPUT
  SPCR |= _BV(SPE);       //Turn on SPI in Slave Mode
  SPI.attachInterrupt();  //Activate SPI Interuupt 
  Serial.begin(9600); // send and receive at 9600 baud
}


void loop()
{

    Serial.println((unsigned int)(data[0])+(unsigned int)(data[1]<<8));
    
}
