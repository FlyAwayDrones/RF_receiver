#include <Servo.h>
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
int j1x,j1y,j2x,j2y;

Servo myservo;

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
  myservo.attach(3);
  pinMode(4,OUTPUT);
  digitalWrite(4,HIGH);
}
int hextodecimal(char c){
  switch (c){
    case 48: return 0; break;
    case 49: return 1; break;
    case 50: return 2; break;
    case 51: return 3; break;
    case 52: return 4; break;
    case 53: return 5; break;
    case 54: return 6; break;
    case 55: return 7; break;
    case 56: return 8; break;
    case 57: return 9; break;
    case 97: return 10; break;
    case 98: return 11; break;
    case 99: return 12; break;
    case 100: return 13; break;
    case 101: return 14; break;
    case 102: return 15; break;
    default: return -1; break;
    
  }
  
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
      j1y=hextodecimal(receivedControls[1]);
      Serial.print("Valor: ");
      Serial.print(j1y);
      j1y=map(j1y,0,15,0,180);
      j1y=180-j1y;
      myservo.write(j1y);
      Serial.print("Servo: ");
      Serial.print(j1y);
      // First, stop listening so we can talk
      radio.stopListening();

      
      // Send the final one back.
      Serial.println(" ... Enviando resposta.");
      radio.write( &receivedControls, sizeof(receivedControls) );
      
      // Now, resume listening so we catch the next packets.
      radio.startListening();
    }
  
}
