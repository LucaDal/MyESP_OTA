#ifndef NETWORK_H
#define NETWORK_H

//#define DEBUG

#include <ArduinoJson.h>
#include "MyUpdater.h"
#include "MyFirmware.h"
#include "Certs.h"
#include "OtaDebug.h"

#ifdef ESP8266
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <WiFiClientSecureBearSSL.h>
  #include <BearSSLHelpers.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <WiFiClientSecure.h>
  #include <HTTPClient.h>
#endif

// -----------------------------------------------------------------------------
// if USE_TLS is defined via build_flags in platformio.ini):
//   - use ONLY client "secure" (HTTPS)
// If NOT defined:
//   - use ONLY WiFiClient (HTTP)
// If not defined: only HTTP
// -----------------------------------------------------------------------------
#ifdef USE_TLS
  #ifdef ESP8266
    using NetClient = BearSSL::WiFiClientSecure;
  #elif defined(ESP32)
    using NetClient = WiFiClientSecure;
  #endif
#else
  using NetClient = WiFiClient;
#endif

class Network {
private:
  HTTPClient httpClient;

  std::unique_ptr<NetClient> client { new NetClient };

  // root CA only f
#ifdef ESP8266
  #ifdef USE_TLS
    BearSSL::X509List trustedRoots;
  #endif
#endif
  JsonDocument doc;
  String BASE_URL;
  void setClock();
  bool startConnectionTo(const char *server_api_address, String api_key, String path);

public:
  // il bool ora puoi usarlo come "verifyCert" (true = cert valido, false = setInsecure)
  Network(const char *base_url, bool verifyCert);
  void WiFiBegin();
  bool isConnected();
  Firmware checkVersion(String api_key);
  bool fileDownload(String api_key, String md5Checksum, String currentVersion);
};

#endif // NETWORK_H
