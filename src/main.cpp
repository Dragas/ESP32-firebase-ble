#include <Arduino.h>
#include <WiFi.h>
#include "FirebaseESP32.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <cstring>
// nesu tikras del sito bet tau gali reiket
// jeigu nesukompiliuos naudok sita
// cstring nuo string.h skirias tik tuom kad cstring yra ale
// pataisytas variantas  ir "pagal c++ standarta"
//#include <string.h>
#define FIREBASE_HOST "ble-scan-test.firebaseio.com" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "9V5QTidiD123wqsyzF9YchGUVWZB290rM7eCHWjx"
#define WIFI_SSID "Zavuciai"
#define WIFI_PASSWORD "Tustukai115"
#define BLE_SERVER_MAC "C8:1E:E7:44:F9:36"
#define MAC_COUNT 3 // kai prisidesi daugiau mac padidink sita skaiciu iki tiek
#define MAC_LENGTH 17
//Define Firebase Data objects
FirebaseData firebaseData;


const String path = "/ESP32-firebase-test";

const int scanTime = 5; //In seconds
BLEScan *pBLEScan;
const String MACS[MAC_COUNT] = {"mac 1", "mac 2", "mac n"};

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    Serial.print("Advertised Device: ");
    String address = advertisedDevice.getAddress().toString().c_str();
    Serial.println(address);
    for(int i = 0; i < MAC_COUNT; i++) {
        if(std::strlen(address) == MAC_LENGTH && std::strcmp(MACS[i], address) == 0) { // uztikrina kad nepraskanuosi
            Serial.print(" RSSI: "); // "netinkamu" MAC adresu
            String actualPath = path + "/data/" + address;
            int RSSI = advertisedDevice.getRSSI();
            Serial.println(RSSI);
            Serial.print("Etag:");
            String ETag = Firebase.getETag(firebaseData, actualPath);
            Serial.println(ETag);
            // creating json object to post
            String json = "{\"ETag\":\"" + ETag + "\",\"RSSI\": " + RSSI + "}";
            Firebase.pushJSON(firebaseData, actualPath, json);
            Firebase.setTimestamp(firebaseData, actualPath + ETag);
            Serial.println(json);
            break;
        }
    }
    Serial.println("Ignoring...\r\n")
  };
};

void setup()
{

  Serial.begin(115200);
  Serial.println();
  Serial.println();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.println("Scanning Bluetooth devices...");
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
    // aktyvus skanavimas tave parodo visiems kitiems kad uzsiemi skanavimu
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99); // less or equal setInterval value

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void loop()
{
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.println("Scan done!");
    // idomu ar sitai yra blokuojantis ar asinchroninis iskvietimas
    // nes jeigu asinchroninis, tai per daug ankstyvas rezultatu isvalymas
    // gali pakenkti tavo onresult funkcijai
    // is kitos puses nesu skaites tavo bluetooth libo
    // tai negaliu daryti tokiu drasiu spejimu
  pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
  delay(1000);
}
