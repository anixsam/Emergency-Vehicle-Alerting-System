// ask_transmitter.pde
// -*- mode: C++ -*-
// Simple example of how to use RadioHead to transmit messages
// with a simple ASK transmitter in a very simple way.
// Implements a simplex (one-way) transmitter with an TX-C1 module

//RF Connects VCC to 5V, GND to GND, Data to GPIO12

#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

RH_ASK driver;
int i=0;
void setup()
{
    pinMode(4,INPUT_PULLUP);
    pinMode(5,INPUT_PULLUP);
    pinMode(6,INPUT_PULLUP);
    pinMode(7,INPUT_PULLUP);
    Serial.begin(9600);	  // Debugging only
    if (!driver.init())
         Serial.println("init failed");
}

void loop()
{
    char *msg;
    
    if(digitalRead(4) == LOW)
      msg = "l";
    else if(digitalRead(5) == LOW)
      msg = "r";
    else if(digitalRead(6) == LOW)
      msg = "s";
    else if(digitalRead(7) == LOW)
      msg = "v";
    else
      msg = "o";

    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
    Serial.println(msg);
    delay(200);
}
