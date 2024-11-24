#ifndef NETWORK_H
#define NETWORK_H

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include "MyUpdater.h"
#include "MyFirmware.h"
#include "Certs.h"
#include <BearSSLHelpers.h>

class Network {
private:
  HTTPClient httpClient;
  std::unique_ptr<BearSSL::WiFiClientSecure> client {new BearSSL::WiFiClientSecure};
  JsonDocument doc;
  BearSSL::X509List trustedRoots;
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