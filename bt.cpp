
#include "bt.h"
#include <TFT_eSPI.h>
#include <SPI.h>
#include "WiFi.h"
#include <Wire.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include "BLEDevice.h"
#include "BluetoothSerial.h"
#include "VescUart.h"

uint8_t vesc_bt_address[6] = {0x98, 0xD3, 0x31, 0x90, 0x5B, 0xC7};  // change to your HC-06 BT addres you can get it using BluetoothCL.exe from http://www.nirsoft.net
char * vesc_pin = "1234";                                           // change to your BT pin
bool BTConnected = false;

BluetoothSerial SerialBT;
VescUart VescConnection;

bool bt_connect(void)
{

  SerialBT.enableSSP();
  SerialBT.begin("ESP32test", true);
  SerialBT.setPin(vesc_pin);
  BTConnected = SerialBT.connect(vesc_bt_address);
  if(BTConnected) 
  {
    Serial.println("Connected Succesfully!");
    VescConnection.setSerialPort(&SerialBT);
  } else {
      Serial.println("Failed to connect. Make sure remote device is available and in range, then restart app.");
  }

  return BTConnected;
  
}

void bt_scan(void)
{
  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true);
  BLEScanResults scanResults = pBLEScan->start(30);
  int count = scanResults.getCount();
  String noOfDevicesStr = "No of devs:" + String(count);
  Serial.println(noOfDevicesStr);

  for (int i = 0; i < count; i++) 
  {
    BLEAdvertisedDevice bleSensor = scanResults.getDevice(i);
    String sensorName = bleSensor.getName().c_str();
    String address = bleSensor.getAddress().toString().c_str();

    Serial.println("Find" + address + " " + sensorName);

  }

    
}
