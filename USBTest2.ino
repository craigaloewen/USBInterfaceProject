#include <spi.h>
#include <Usb.h>
#include "torqueDevice.h"


#define CONFVALUE 1
#define EP_MAXPKTSIZE 64

void setup();
void loop();

EpInfo* ep_info;
EpInfo* newEp;

USB Usb;
USB* Usb_p = &Usb;
torqueDevice device1(Usb_p);

uint8_t epSearch;

void setup() {
 
  Serial.begin(115200);
  Serial.println("Start");
  
  
  //Start USB
  if (Usb_p->Init() == -1) {
   Serial.println("Usb did not start"); 
  } else {
  }
  
  
  epSearch = 0;
  
  
  
  
  delay(200);
}

void loop() {
 Usb.Task();
 
 uint8_t rcode;

 rcode = Usb_p->getUsbTaskState();
 
 if (rcode == 0x90) {
   
   //Once Device is Setup
   Serial.println("Using Device");
   delay(1000);  
 
   //Try to send some data?
   device1.dataSend();

  //Delay forever
  while (1) {
   epSearch += 1;
   delay(10);
   Usb.Task();
  }
   
 }
 
}






