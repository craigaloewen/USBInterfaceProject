#include "torqueDevice.h"

//Constructor, initialize all to 0 
torqueDevice::torqueDevice(USB* inpUsb) {

  pUsb = inpUsb;
  bAddress = 0;
  
  
  for (uint8_t i = 0; i < 3; i++) {
   epInfo[i].epAddr = 0;
   epInfo[i].maxPktSize = (i) ? 0 : 8;
   epInfo[i].bmSndToggle = 0;
   epInfo[i].bmRcvToggle = 0; 
   epInfo[i].bmNakPower = (i) ? USB_NAK_NOWAIT : USB_NAK_MAX_POWER;
  }
  
  if (pUsb) {
   pUsb->RegisterDeviceClass(this); 
  }
  
  

}





//Initializer, using Kristian Lauszus' code as a base
uint8_t torqueDevice::Init(uint8_t parent, uint8_t port, bool lowspeed) {
  
  UsbDevice *p = 0;
  
  uint8_t buf[sizeof (USB_DEVICE_DESCRIPTOR)];
  USB_DEVICE_DESCRIPTOR * udd = reinterpret_cast<USB_DEVICE_DESCRIPTOR*>(buf);
  
  uint8_t rcode;
  EpInfo *oldep_ptr = 0;
  uint16_t PID;
  uint16_t VID;
  
  
  AddressPool &addrPool = pUsb->GetAddressPool();
  
  Serial.println("Torque Init");
  
  p = addrPool.GetUsbDevicePtr(0);
  
  if (!p) {
   Serial.println("Address not found");
  return USB_ERROR_ADDRESS_NOT_FOUND_IN_POOL; 
  }
  
  if (!p->epinfo) {
   Serial.println("EpInfo is null");
  return USB_ERROR_EPINFO_IS_NULL; 
    
  }
  
  oldep_ptr = p->epinfo;
  
  p->epinfo = epInfo;
  
  p->lowspeed = lowspeed;
  
  rcode = pUsb->getDevDescr(0,0,sizeof (USB_DEVICE_DESCRIPTOR), (uint8_t*)buf);
  
  if (rcode) {
   Serial.print("Failure to get DeviceDescript ");
   Serial.println(rcode, HEX);
   return rcode; 
  }
  
  VID = udd->idVendor;
  PID = udd->idProduct;
  
  if (VID != 0x04D8 || PID != 0xFFE9) {
   Serial.println("Unknown Device");
   return USB_DEV_CONFIG_ERROR_DEVICE_NOT_SUPPORTED; 
  }
  
  bAddress = addrPool.AllocAddress(parent, false, port);
  
  if (!bAddress) {
   Serial.println("Out of Address Space");
   return USB_ERROR_OUT_OF_ADDRESS_SPACE_IN_POOL; 
  }
  
  epInfo[0].maxPktSize = udd->bMaxPacketSize0;
  
  rcode = pUsb->setAddr(0,0,bAddress);
  
  if (rcode) {
   p->lowspeed = false;
   addrPool.FreeAddress(bAddress);
   bAddress = 0;
   Serial.print("No set Address? ");
   Serial.print(rcode,HEX);
   return rcode; 
  }
  
  Serial.print("Address: ");
  Serial.println(bAddress,HEX);
  
  delay(200);
  
  p->lowspeed = false;
  
  p=addrPool.GetUsbDevicePtr(bAddress);
  
  
  
  if (!p) { 
   Serial.println("Address not found in pool");
   return USB_ERROR_ADDRESS_NOT_FOUND_IN_POOL; 
  }
  
  p->lowspeed = lowspeed;
  
  rcode = pUsb->setEpInfoEntry(bAddress,1,epInfo);
  
  if (rcode) {
   Serial.print("Problem assigning first EPinfo: ");
   Serial.println(rcode,HEX); 
  }
  
  //Assigning the endpoints from the device descriptor. Don't know which sndToggle values to use
   
  epInfo[0].epAddr = 0x01;
  epInfo[0].maxPktSize = 0x40;
  epInfo[0].epAttribs = USB_TRANSFER_TYPE_INTERRUPT;
  epInfo[0].bmSndToggle = 0;
  epInfo[0].bmRcvToggle = 0;
  epInfo[0].bmNakPower = USB_NAK_NOWAIT;
  
  epInfo[1].epAddr = 0x81;
  epInfo[1].maxPktSize = 0x40;
  epInfo[1].epAttribs = USB_TRANSFER_TYPE_INTERRUPT;
  epInfo[1].bmSndToggle = 0;
  epInfo[1].bmRcvToggle = 0;
  epInfo[1].bmNakPower = USB_NAK_NOWAIT;
  
  epInfo[2].epAddr = 0x00;
  epInfo[2].maxPktSize = 0x40;
  epInfo[2].bmSndToggle = 0;
  epInfo[2].bmRcvToggle = 0;
  epInfo[2].bmNakPower = USB_NAK_NOWAIT;
  
  rcode = pUsb->setEpInfoEntry(DEVADDR,3,epInfo);
  
  if (rcode) {
   Serial.print("Error assigning torque Specific Addresses");
   Serial.println(rcode,HEX);
   return rcode; 
  }

  delay(200);


  delay(300);
  
  
  //Set Configuration to 1
  rcode = pUsb->setConf(bAddress, epInfo[2].epAddr, 1);

  if (rcode) {
   Serial.print("Error Setting Config: ");
   Serial.println(rcode,HEX);
   return rcode;
  }
  
  
  Serial.println("Configuration Successful");
  return 0;
  
  
  
}


//Try to send some data that I've seen been sent using a Sniffer
uint8_t torqueDevice::dataSend() {
  uint8_t rcode;
  uint8_t sendBuffer[10] = {0};
  uint8_t receiveBuffer[10] = {0};
  uint16_t numBytes = 9;
  
  
  
  sendBuffer[0] = 0x02;
  
  rcode = pUsb->outTransfer(bAddress,epInfo[0].epAddr,buffLength,buf);
  
  if (rcode) {
   Serial.print("Error Sending Data: ");
   Serial.println(rcode,HEX);
   return rcode;
 }
 
 delay(20);
 
 //Receive the 9 byte transfer back? Gives error code: 7 usually, which means that the PID is not correct
 rcode = pUsb->inTransfer(bAddress,epInfo[1].epAddr,&numBytes,receiveBuffer,0x04);
 if (rcode) {
   Serial.print("Error receiving");
   Serial.println(rcode,HEX);
   return rcode;
 }
 delay(20);

 
 return 0;


}

