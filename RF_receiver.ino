#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//Defines radio pins (CE and CSN) as 9 and 10 respectively

RF24 radio(9,10);

//Create channels for communication
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

void setup(void)
{
  //Start serial communication and print info
  Serial.begin(57600);
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
  
    // if there is data ready
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      unsigned long got_time;
      bool done = false;
      while (!done)
      {
        // Fetch the payload, and see if this was the last one.
        done = radio.read( &got_time, sizeof(unsigned long) );

        // Spew it
        printf("Got payload %lu...",got_time);

	// Delay just a little bit to let the other unit
	// make the transition to receiver
	delay(20);
      }

      // First, stop listening so we can talk
      radio.stopListening();

      // Send the final one back.
      radio.write( &got_time, sizeof(unsigned long) );
      printf("Sent response.\n\r");

      // Now, resume listening so we catch the next packets.
      radio.startListening();
    }
  }

  //
  // Change roles
  //

  if ( Serial.available() )
  {
    char c = toupper(Serial.read());
    if ( c == 'T' && role == role_pong_back )
    {
      printf("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK\n\r");

      // Become the primary transmitter (ping out)
      role = role_ping_out;
      radio.openWritingPipe(pipes[0]);
      radio.openReadingPipe(1,pipes[1]);
    }
    else if ( c == 'R' && role == role_ping_out )
    {
      printf("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK\n\r");
      
      // Become the primary receiver (pong back)
      role = role_pong_back;
      radio.openWritingPipe(pipes[1]);
      radio.openReadingPipe(1,pipes[0]);
    }
  }
}
// vim:cin:ai:sts=2 sw=2 ft=cpp
