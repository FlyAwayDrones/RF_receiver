#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//Defines radio pins (CE and CSN) as 9 and 10 respectively

RF24 radio(9,10);

//Create channels for communication
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

//Variables needed
char receivedControls[6] = "aaaa;";
char nadave[6] = "CUCU;";


void setup(void)
{
  //Start serial communication and print info
  Serial.begin(57600);
  printf_begin();
  Serial.println("-- RF_receiver - Drone Board --");

  //Start radio communication, open writing and reading pipes
  radio.begin();
  radio.setRetries(15,15);

  //Here it opens channel 1 as writing and 0 as reading.
  //This is default for receiving.
  //To change for transmitting, we change to the exact opposite.
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);
  
  //Listen to Radio and print details about it.
  radio.startListening();
  radio.printDetails();
  
}

void loop(void)
{
    //check if there is data available
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      
      bool done = false;
            
      while(!done)
        done=radio.read( &receivedControls, sizeof(receivedControls));
      
      Serial.print("Posicoes: ");
      Serial.print(receivedControls);
      
      
      // First, stop listening so we can talk
      radio.stopListening();

      
      // Send the final one back.
      Serial.println(" ... Enviando resposta.");
      radio.write( &receivedControls, sizeof(nadave) );
      
      // Now, resume listening so we catch the next packets.
      radio.startListening();
    }
  
}
