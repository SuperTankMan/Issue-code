#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define LED_BUILTIN 26

/* Define Service */
//Custome service UUID
#define serviceID BLEUUID((uint16_t)0x1700)


BLEDescriptor *myBLE2902 = new BLE2902();
bool deviceConnected = false;

uint8_t counter = 1;
unsigned long previousMillis = 0;  // will store last time LED was updated
const long interval = 1000;  // interval at which to blink (milliseconds)

BLECharacteristic counterCharacteristic
{
  BLEUUID((uint16_t)0x1A00),
  BLECharacteristic::PROPERTY_READ |
  BLECharacteristic::PROPERTY_NOTIFY 
};

class MyServerCallbacks : public BLEServerCallbacks 
{
  void onConnect(BLEServer* MyServer)
  {
    deviceConnected = true;
  }
  void onDisconnect(BLEServer* MyServer)
  {
    deviceConnected = false;
    MyServer->getAdvertising()->start();
  }
};

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  
//  Serial.begin(115200);
  
  //Create and name the BLE device
  BLEDevice::init("ESP32BLETest");

  /* Create the BLE server */
  BLEServer *MyServer = BLEDevice::createServer();
  MyServer->setCallbacks(new MyServerCallbacks()); // Set the function that handles Server Callbacks

  /* Create service on the server */
  //BLEService *MyService = MyServer->createService(BLEUUID((uint16_t)0x1700)); // A random ID has been selected
  BLEService *MyService = MyServer->createService(serviceID); // A random ID has been selected

  /* Add characteristic to the service */
  MyService->addCharacteristic(&counterCharacteristic);
  counterCharacteristic.addDescriptor(myBLE2902);
  counterCharacteristic.setValue(&counter, 2);

  /* Configure advertising with the service to advertise */
  MyServer->getAdvertising()->addServiceUUID(serviceID);

  // Start the service 
  MyService->start();

  // Start the Server/Advertising
  MyServer->getAdvertising()->start();

  Serial.println("Waiting for a Client to connect...");
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);

  if (!deviceConnected)
  {
    //Not connected so reset Notifications
    //((BLE2902*)myBLE2902)->setNotifications(false);
  }
  else
  {
    //if registered for Notification then flash LED and send counter value to Client by notification
    if (((BLE2902*)myBLE2902)->getNotifications())
    {
      //loop to blink without delay
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval)
      {
        // save the last time you blinked the LED
        previousMillis = currentMillis;
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        counter++;
        counterCharacteristic.setValue(&counter, 1);
        counterCharacteristic.notify();       
      }
    }
  }
}
