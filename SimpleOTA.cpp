#include "SimpleOTA.h"

static SimpleOTA *instance = NULL;

SimpleOTA::SimpleOTA() {
  #ifdef DEBUG
    Serial.println("starting the OTA client");
  #endif
  t1 = 0;
  instance = this;
}


void SimpleOTA::init(int EEPROMSize, const char * API_KEY){
  this->initVersion(EEPROMSize);
  this->API_KEY = API_KEY;
  checkUpdates(0);
}

/**
 * @brief initialize OTA class and check updates.
 * Firmware data will be saved to the last EEPROM address
 * address_IP is the DNS or IP address without https or http type 
 */
void SimpleOTA::begin(int EEPROMSize, const char * server_IP, const char * API_KEY, bool useTLS) {
  this->initNetwork(server_IP, useTLS);
  init(EEPROMSize,API_KEY);
}

/**
 * @brief return false if something goes wrong
 * called from the main thread
 */
bool SimpleOTA::checkUpdates(unsigned long seconds) {
    if (millis() - t1 >= seconds * 1000) {
      t1 = millis();
      if(network->isConnected()) 
        return this->serverFirmwareCheck();
    }
    return true;
}

void SimpleOTA::initVersion(int EEPROMSize) {
  version = new FirmwareData(EEPROMSize);
  #ifdef DEBUG
    Serial.printf("*OTA: Current Version: %s\n",version->getNewFirmwareVersion().c_str());
  #endif
}

void SimpleOTA::initNetwork(const char * base_url, bool useTLS) {
  #ifdef DEBUG
    Serial.println("*OTA: initNetwork");
  #endif
  network = new Network(base_url, useTLS); 
  network->WiFiBegin();
}

bool SimpleOTA::startDownload() {
  if(network->fileDownload(API_KEY,version->getFirmwareMD5Image(),version->getOldFirmwareVersion())){
    version->saveVersion(version->getNewFirmwareVersion());//save only if update goes fine
    #ifdef DEBUG
      Serial.println("*OTA: Restarting the board");
    #endif
    delay(1000); // Wait a second and restart
    ESP.restart();
  }
  return false;
}
/**
 * return false if failed, true if no update found.
 */
bool SimpleOTA::serverFirmwareCheck() {
  version->setNewFirmware(network->checkVersion(API_KEY));
  if (version->getNewFirmwareVersion() == "-1") {
    #ifdef DEBUG
      Serial.println("*OTA: Server Not Responding");
    #endif
    return false;
  } else {
    if (version->hasNewUpdate()) {
      #ifdef DEBUG
        Serial.println("*OTA: New Build Available!");
        Serial.println("*OTA: Starting the donwload!");
      #endif
      return startDownload();
    }else{
      #ifdef DEBUG
        Serial.println("*OTA: Current version up to date");
      #endif
    }
    return true;
  }
}


