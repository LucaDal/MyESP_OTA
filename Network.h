#ifndef NETWORK_H
#define NETWORK_H

//#define DEBUG

#include <ArduinoJson.h>
#include "MyUpdater.h"
#include "MyFirmware.h"
#include "Certs.h"

#ifdef ESP8266
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <WiFiClientSecureBearSSL.h>
  #include <BearSSLHelpers.h>
#endif
#ifdef ESP32
  #include <WiFiClientSecure.h>
  #include <WiFi.h>
#endif


class Network {
private:
  HTTPClient httpClient;

  #ifdef ESP8266
    std::unique_ptr<BearSSL::WiFiClientSecure> client {new BearSSL::WiFiClientSecure};
    BearSSL::X509List trustedRoots;
  #endif
  #ifdef ESP32
    std::unique_ptr<WiFiClientSecure> client {new WiFiClientSecure};
  #endif
  JsonDocument doc;
  String BASE_URL;
  bool useTLS;
  void setClock();
  bool startConnectionTo(const char * server_api_address, String api_key);

public:
  Network(const char *base_url, bool useTLS);
  void WiFiBegin();
  bool isConnected();
  Firmware checkVersion(String api_key);
  bool fileDownload(String api_key, String md5Checksum, String currentVersion);
};
#endif