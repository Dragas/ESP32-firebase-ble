#include <Arduino.h>
#include <WiFi.h>
#include "FirebaseESP32.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#define FIREBASE_HOST "ble-scan-test.firebaseio.com" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "9V5QTidiD123wqsyzF9YchGUVWZB290rM7eCHWjx"
#define WIFI_SSID "Zavuciai"
#define WIFI_PASSWORD "Tustukai115"
#define BLE_SERVER_MAC "C8:1E:E7:44:F9:36"
//Define Firebase Data objects
FirebaseData firebaseData;

String json = "";
std::string address = "";
int RSSI = 0;
String path = "/ESP32-firebase-test";
String ETag = "";

int scanTime = 5; //In seconds
BLEScan *pBLEScan;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    Serial.print("Advertised Device: ");
    address = advertisedDevice.getAddress().toString().c_str();
    Serial.println(address.c_str());
    Serial.print(" RSSI: ");
    RSSI = advertisedDevice.getRSSI();
    Serial.println(RSSI);
    Serial.print("Etag:");
    ETag = Firebase.getETag(firebaseData, path + "/data/");
    Serial.println(ETag);
    // creating json object to post

    json = "{\"ETag\":\"" + String(ETag) + "\",\"address\":\"" + String(address.c_str()) + "\",\"RSSI\":\"" + int(RSSI) + "\"}";
    Firebase.pushJSON(firebaseData, path + "/data/", json);
    Firebase.setTimestamp(firebaseData, path + "/data/" + String(ETag));
    Serial.println(json);
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
  pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
  delay(1000);
}
