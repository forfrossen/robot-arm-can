#ifndef UTILS_WIFI
#define UTILS_WIFI
#include "WiFiS3.h"
#include "../Debug.h"
#include "../inc/arduino_secrets.h"
#include "Arduino_LED_Matrix.h" //Include the LED_Matrix library

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS; // the WiFi radio's status
ArduinoLEDMatrix matrix;

void printMacAddress(byte mac[])
{

  Serial.print("MAC address: ");
  for (int i = 0; i < 6; i++)
  {
    if (i > 0)
    {
      Serial.print(":");
    }
    if (mac[i] < 16)
    {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
  }
  Serial.println();
}

void printWifiData()
{
  Debug debug("UTILS_WIFI", __func__);
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  debug.info();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  debug.info();
  printMacAddress(mac);
}

void printCurrentNet()
{
  Debug debug("UTILS_WIFI", __func__);
  // print the SSID of the network you're attached to:
  debug.info();
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  debug.info();
  Serial.print("BSSID: ");
  WiFi.BSSID(bssid);
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  debug.info();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  debug.info();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
}

void connectWifi()
{
  Debug debug("UTILS_WIFI", __func__);

  matrix.loadSequence(LEDMATRIX_ANIMATION_WIFI_SEARCH);
  matrix.begin();
  matrix.play(true);

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)
  {
    debug.error();
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION)
  {
    debug.info();
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED)
  {
    debug.info();
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(1000);
  }

  // you're connected now, so print out the data:
  debug.info();
  Serial.println("You're connected to the network");
  printCurrentNet();
  printWifiData();
  matrix.loadSequence(LEDMATRIX_ANIMATION_BOUNCING_BALL);
  matrix.play(true);
}

#endif