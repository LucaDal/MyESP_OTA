#include "MyUpdater.h"

#define DEBUG

void update_started() {
  Serial.println("*OTA: CALLBACK:  HTTP update process started");
}

void update_finished() {
  Serial.println("*OTA: CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total) {
  Serial.printf("*OTA: CALLBACK:  HTTP update process at %d of %d bytes...\n", cur,
                total);
}

void update_error(int err) {
  Serial.printf("*OTA: CALLBACK:  HTTP update fatal error code %d\n", err);
}

MyUpdater::MyUpdater(String md5Checksum) {
#if defined(DEBUG) && defined(ESP8266)
  ESPhttpUpdate.onStart(update_started);
  ESPhttpUpdate.onEnd(update_finished);
  ESPhttpUpdate.onProgress(update_progress);
  ESPhttpUpdate.onError(update_error);
#endif
#if defined(DEBUG) && defined(ESP32)
  httpUpdate.onStart(update_started);
  httpUpdate.onEnd(update_finished);
  httpUpdate.onProgress(update_progress);
  httpUpdate.onError(update_error);
#endif
#ifdef ESP8266
  ESPhttpUpdate.rebootOnUpdate(false);
  ESPhttpUpdate.setMD5sum(md5Checksum);
#endif
#ifdef ESP32
  httpUpdate.rebootOnUpdate(false);
#endif
}

bool MyUpdater::startUpdate(HTTPClient &client, String currentFirmwareVersion) {
  t_httpUpdate_return ret;
#ifdef ESP8266
  ret = ESPhttpUpdate.update(client, currentFirmwareVersion);
#endif
#ifdef ESP32
  ret = httpUpdate.update(client, currentFirmwareVersion);
#endif
  switch (ret) {
  case HTTP_UPDATE_FAILED:
#ifdef DEBUG
#ifdef ESP8266
    Serial.printf("OTA*: HTTP_UPDATE_FAILD Error (%d): %s\n",
                  ESPhttpUpdate.getLastError(),
                  ESPhttpUpdate.getLastErrorString().c_str());
#elif defined(ESP32)
    Serial.printf("*OTA: HTTP_UPDATE_FAILD Error (%d): %s\n",
                  httpUpdate.getLastError(),
                  httpUpdate.getLastErrorString().c_str());
#endif
#endif
    return false;

  case HTTP_UPDATE_NO_UPDATES:
#ifdef DEBUG
    Serial.println("*OTA: HTTP_UPDATE_NO_UPDATES");
#endif
    break;

  case HTTP_UPDATE_OK:
#ifdef DEBUG
    Serial.println("*OTA: HTTP_UPDATE_OK");
#endif
    return true;
  }
  return false;
}
