/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

//Uncomment one
//#define NOTIFY_ON
#define INDICATE_ON


int count = 0;

class CharCallbacks : public BLECharacteristicCallbacks
{
  public:  
    void onWrite(BLECharacteristic *pChar) 
    {
      pChar->setValue(++count);
      #ifdef NOTIFY_ON
        pChar->notify();
      #endif
      #ifdef INDICATE_ON
        pChar->indicate();
      #endif
//      delay(5000);
//      pChar->setValue(++count);
//      pChar->indicate();      
      #if 0
      pChar->setValue(++count);
      pChar->indicate();      
      pChar->setValue(++count);
      pChar->indicate();      
      #endif
    }
};

class CCCDCallbacks : public BLEDescriptorCallbacks
{
  public:  
  void onWrite(BLEDescriptor *pDesc) 
  {
    Serial.printf("[%i]:Notification registered\n");    
  }
};

BLECharacteristic *pCharacteristic;

BLEDescriptor *myBLECCCD = new BLE2902();


void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  BLEDevice::init("Indicate issue Server");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                       CHARACTERISTIC_UUID,
                       BLECharacteristic::PROPERTY_READ |
                       BLECharacteristic::PROPERTY_WRITE |
                       #ifdef NOTIFY_ON
                          BLECharacteristic::PROPERTY_NOTIFY //|
                       #endif
                       #ifdef INDICATE_ON
                          BLECharacteristic::PROPERTY_INDICATE
                       #endif
                     );
  

  pCharacteristic->setCallbacks(new CharCallbacks);

  myBLECCCD->setCallbacks(new CCCDCallbacks());
  pCharacteristic->addDescriptor(myBLECCCD);  //Add this line only if the characteristic has the Notify property


  pCharacteristic->setValue(count);
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() 
{
#if 0
  // put your main code here, to run repeatedly:
  if (((BLE2902*)myBLECCCD)->getNotifications())
  if (((BLE2902*)myBLECCCD)->getIndications())
  {
//    pCharacteristic->setValue(++count);
//    pCharacteristic->notify();
//    pCharacteristic->indicate();
  }
#endif
//  delay(50);
}
