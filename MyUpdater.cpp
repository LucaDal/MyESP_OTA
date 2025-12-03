#include "MyUpdater.h"

void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
}

void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err) {
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
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

bool MyUpdater::startUpdate(HTTPClient& https, String currentFirmwareVersion) {
  t_httpUpdate_return ret;
#ifdef ESP8266
  ret = ESPhttpUpdate.update(https, currentFirmwareVersion);
#endif
#ifdef ESP32
  ret = httpUpdate.update(https, currentFirmwareVersion);
#endif
  switch (ret) {
  case HTTP_UPDATE_FAILED:
#ifdef DEBUG
    //Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
    Serial.println(F("Retry in passed secs!"));
#endif
    return false;

  case HTTP_UPDATE_NO_UPDATES:
#ifdef DEBUG
    Serial.println("HTTP_UPDATE_NO_UPDATES");
#endif
    break;

  case HTTP_UPDATE_OK:
#ifdef DEBUG
    Serial.println("HTTP_UPDATE_OK");
#endif
    return true;
  }
  return false;
}