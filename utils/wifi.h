#ifndef UTILS_WIFI
#define UTILS_WIFI

#include "WiFiS3.h"
#include "../Debug.h"
#include "../inc/arduino_secrets.h"
#include "Arduino_LED_Matrix.h" // Include the LED_Matrix library

class WiFiHelper
{
private:
  static char ssid[32];
  static char pass[64];
  static int status;
  static ArduinoLEDMatrix matrix;

  static void printMacAddress(byte mac[])
  {
    Debug debug("UTILS_WIFI", __func__);
    debug.add("MAC address: ");
    for (int i = 0; i < 6; i++)
    {
      if (i > 0)
      {
        debug.add(":");
      }
      if (mac[i] < 16)
      {
        debug.add("0");
      }
      debug.add(mac[i], HEX);
    }
    debug.println();
  }

public:
  WiFiHelper()
  {
  }

  static void printWifiData()
  {
    Debug debug("UTILS_WIFI", __func__);
    // print your board's IP address:
    IPAddress ip = WiFi.localIP();
    debug.info();
    debug.add("IP Address: ");
    debug.print(ip);

    // print your MAC address:
    byte mac[6];
    WiFi.macAddress(mac);
    debug.info();
    printMacAddress(mac);
  }

  static void printCurrentNet()
  {
    Debug debug("UTILS_WIFI", __func__);
    // print the SSID of the network you're attached to:
    debug.info();
    debug.add("SSID: ");
    debug.print(WiFi.SSID());

    // print the MAC address of the router you're attached to:
    byte bssid[6];
    debug.info();
    debug.add("BSSID: ");
    WiFi.BSSID(bssid);
    printMacAddress(bssid);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    debug.info();
    debug.add("signal strength (RSSI):");
    debug.print(rssi);

    // print the encryption type:
    byte encryption = WiFi.encryptionType();
    debug.info();
    debug.add("Encryption Type:");
    debug.print(encryption, HEX);
  }

  static void connectWifi()
  {
    Debug debug("UTILS_WIFI", __func__);

    strcpy(ssid, SECRET_SSID);
    strcpy(pass, SECRET_PASS);

    matrix.loadSequence(LEDMATRIX_ANIMATION_WIFI_SEARCH);
    matrix.begin();
    matrix.play(true);

    // check for the WiFi module:
    if (WiFi.status() == WL_NO_MODULE)
    {
      debug.error();
      debug.print("Communication with WiFi module failed!");
      // don't continue
      while (true)
        ;
    }

    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION)
    {
      debug.info();
      debug.print("Please upgrade the firmware");
    }

    // attempt to connect to WiFi network:
    while (status != WL_CONNECTED)
    {
      debug.info();
      debug.add("Attempting to connect to WPA SSID: ");
      debug.print(ssid);
      // Connect to WPA/WPA2 network:
      status = WiFi.begin(ssid, pass);

      // wait 10 seconds for connection:
      delay(1000);
    }

    // you're connected now, so print out the data:
    debug.info();
    debug.print("You're connected to the network");
    printCurrentNet();
    printWifiData();
    matrix.loadSequence(LEDMATRIX_ANIMATION_BOUNCING_BALL);
    matrix.play(true);
  }
};

#endif // UTILS_WIFI