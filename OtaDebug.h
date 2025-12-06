#ifndef OTA_DEBUG_H
#define OTA_DEBUG_H

#ifdef DEBUG
  #define OTA_LOG(msg) Serial.printf(PSTR("*OTA: %s\n"), (msg))
  #define OTA_LOGF(fmt, ...) Serial.printf(PSTR("*OTA: " fmt), ##__VA_ARGS__)
#else
  #define OTA_LOG(msg) ((void)0)
  #define OTA_LOGF(...) ((void)0)
#endif

#endif
