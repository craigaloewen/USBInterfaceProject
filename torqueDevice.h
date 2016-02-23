#ifndef TORQUEDEVICE_HPP
#define TORQUDEVICE_HPP


#include <Usb.h>

#define DEVADDR 1

//See the .cpp for comments!

class torqueDevice : public USBDeviceConfig {

public:

    torqueDevice(USB* inUsb_p);
    uint8_t Init(uint8_t parent, uint8_t port, bool lowspeed);
    uint8_t dataSend();
    uint8_t sendData(uint8_t buffLength, uint8_t* buf);

	
  
private:

  uint8_t bAddress;
  USB* pUsb;
  EpInfo epInfo[3];

   
  
};

#endif  //TORQUEDEVICE_HPP
