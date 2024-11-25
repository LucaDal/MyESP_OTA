#include "Network.h"
//#define DEBUG

Network::Network(const char * base_url, bool useTLS){

  this->useTLS = useTLS;
  this->BASE_URL = String("https://") + String(base_url);
  if (useTLS){
    this->trustedRoots.append(cert_ISRG_X1);
    this->trustedRoots.append(cert_ISRG_X2);
    this->client->setTrustAnchors(&trustedRoots);
    this->client->setSSLVersion(BR_TLS12, BR_TLS12);
    setClock();
  }else{
    this->client->setInsecure();
  }
}

void Network::setClock() {
    configTime("Europe/Rome", "europe.pool.ntp.org");
    #ifdef DEBUG
      Serial.printf_P(PSTR("*OTA: %lu: Waiting for NTP time sync "), millis());
      time_t now = time(nullptr);
      while (now < 8 * 3600 * 2) {
          delay(250);
          Serial.print(".");
          now = time(nullptr);
      }
      Serial.print(F("\r\n"));
      struct tm timeinfo; // NOLINT(cppcoreguidelines-pro-type-member-init)
      gmtime_r(&now, &timeinfo);
      Serial.printf_P(PSTR("*OTA: Current time (UTC):   %s"), asctime(&timeinfo));
      localtime_r(&now, &timeinfo);
      Serial.printf_P(PSTR("*OTA: Current time (Local): %s"), asctime(&timeinfo));
    #endif
}

void Network::WiFiBegin() {
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}

bool Network::isConnected(){
  return WiFi.status() == WL_CONNECTED;
}

bool Network::startConnectionTo(const char * server_api_address, String api_key){
  bool http_connected = false;
    String targetURL = this->BASE_URL + server_api_address + api_key;
    #ifdef DEBUG
      Serial.println("*OTA: Connecting to: " + targetURL);
    #endif
    http_connected = httpClient.begin(*client, targetURL);

  return http_connected;
}


Firmware Network::checkVersion(String api_key) {

  Firmware firmware;
  firmware.version = "-1";
  #ifdef DEBUG
    Serial.println("*OTA: checking version");
  #endif
  if (isConnected()) {    

    bool http_connected = startConnectionTo("/ota/api/get/version/", api_key);

    if(http_connected){
        #ifdef DEBUG
          Serial.println("*OTA: Connected");
        #endif
      int httpCode = httpClient.GET();
      if (httpCode == HTTP_CODE_OK) {
        String payload = httpClient.getString();
        #ifdef DEBUG
          Serial.println(payload);
        #endif
        DeserializationError error = deserializeJson(doc, payload);
        if (error) {
          #ifdef DEBUG
            Serial.print(F("*OTA: deserializeJson() failed: "));
            Serial.println(error.f_str());
          #endif
          return firmware;
        }
        firmware.version = doc["version"].as<String>();
        firmware.md5_checksum = doc["md5Checksum"].as<String>();
      } else {
        #ifdef DEBUG
          Serial.printf("*OTA: [httpClient] GET... failed retrieving version number: %s\n", httpClient.errorToString(httpCode).c_str());
        #endif
      }
      httpClient.end();
    }else{
        #ifdef DEBUG
          Serial.println("*OTA: [httpClient] Unable to connect\n");
        #endif
    }
  }
  return firmware;
}

bool Network::fileDownload(String api_key, String md5Checksum, String currentVersion){

  if (isConnected()) {
    MyUpdater update = MyUpdater(md5Checksum);
    bool http_connected = startConnectionTo("/ota/api/post/update/", api_key);

    if(http_connected){
      #ifdef DEBUG
          Serial.println("*OTA: Connected - starting download");
      #endif
      bool return_value = update.startUpdate(this->httpClient, currentVersion);
      httpClient.end();
      return return_value;
    }
  }
  return false;
}