#ifndef globals
#define globals

#include <FS.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>


bool shouldSaveConfig = false;
bool failedComm=-1;
char server[64];
int port;
char NAME[6]="S0000";
char ChipId[32];
char device_login[32];
char device_pass[32];
const char sub_dev_modifier[4] = "sub";
const char pub_dev_modifier[4] = "pub";
char prefix[5] = "data";
char _rootURL[64]="data.demo.konkerlabs.net";

char *getChipId(){
  return  ChipId;
}

bool interpretHTTPCode(int httpCode){
  if (httpCode > 0 && httpCode<300) { //Check the returning code
    return 1;

  }else{
    return 0;
  }
}

#include "../helpers/subChanTuple.h"

#endif