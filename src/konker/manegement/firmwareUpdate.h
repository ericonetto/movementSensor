
#ifndef firmwareUpdate
#define firmwareUpdate

#include "../helpers/globals.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include "../helpers/jsonhelper.h"

#ifdef ESP8266
#include <functional>
#define UPDATE_SUCCESS_CALLBACK_SIGNATURE std::function<void(char[16])> succes_update_callback
#else
#define UPDATE_SUCCESS_CALLBACK_SIGNATURE void (*succes_update_callback)(char[16])
#endif


class ESP8266HTTPKonkerUpdate: public ESP8266HTTPUpdate{
  public:
  t_httpUpdate_return update(const String& host, uint16_t port, const String& uri = "/",
                               const String& currentVersion = ""){
    HTTPClient http;
    http.begin(host, port, uri);

    Serial.println("Authorizing.."); 
    http.setAuthorization(device_login, device_pass);
    return ESP8266HTTPUpdate::handleUpdate(http, currentVersion, false);
  }
};

void getVersion(String strPayload, char *version){


  if(parse_JSON_item(strPayload,"version",version)){
    Serial.println("Got version =" + String(version));
  }else{
    strcpy(version,"");
    Serial.println("Failed to parse version"); 
  }
}

void updateSucessCallBack(char *version){
  Serial.println("[update] Update ok, sending confirmation."); 
  bool subCode=0;

  String fwUpdateURL= "http://" + String(_rootDomain) + String (":") + String(_rootPort) + String("/firmware/") + String(device_login); 
  HTTPClient http;  //Declare an object of class HTTPClient
  http.begin(fwUpdateURL);  //Specify request destination
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");
  http.setAuthorization(device_login, device_pass);

  String smsg=String("{\"version\": \"" + String(version) + "\",\"status\":\"UPDATED\"}");
  int httpCode = http.sendRequest("PUT", String(smsg));


  Serial.println("Confirmantion send: " + fwUpdateURL+  + "; Body: " + smsg + "; httpcode: " + String(httpCode));
  Serial.print(">");

  http.end();   //Close connection


  Serial.print(">");

  subCode=interpretHTTPCode(httpCode);

  if (!subCode){
    Serial.println("failed");
    return;
  }else{
    Serial.println("sucess");
    return;
  }
  Serial.println("failed");
  Serial.println("");
}

void hasUpdate(char const rootDomain[],int rootPort, char *version){
  bool subCode=0;

  String fwUpdateURL= "http://" + String(rootDomain) + String (":") + String(rootPort) + String("/firmware/") + String(device_login); 
  
  HTTPClient http;  //Declare an object of class HTTPClient
  http.addHeader("Content-Type", "application/json");
  http.setAuthorization(device_login, device_pass);
  http.begin(fwUpdateURL);  //Specify request destination
  int httpCode = http.GET();

  Serial.println("Checking update: " + fwUpdateURL+ "; httpcode:" + String(httpCode));
  Serial.print(">");

  subCode=interpretHTTPCode(httpCode);

  if (!subCode){
    Serial.println("failed");
    Serial.println("");
    failedComm=1;
  }else{
    Serial.println("sucess");
    Serial.println("");

    String strPayload = http.getString();
    Serial.println("strPayload=" + strPayload);
    int playloadSize=http.getSize();
    http.end();   //Close connection
    if (strPayload!="[]"){
      getVersion(strPayload,version);
      return;
    }
  }
  Serial.println("failed");
  Serial.println("");
  strcpy(version,"");
}

void checkForUpdates(char const rootDomain[],int rootPort, char *expectedVersion, UPDATE_SUCCESS_CALLBACK_SIGNATURE){
    char version[16];
    hasUpdate(rootDomain, rootPort, version);

    if (strcmp(version,expectedVersion)==0  || strcmp("",expectedVersion)==0){
      Serial.println("UPDATING...."); 
      ESP8266HTTPKonkerUpdate ESPhttpKonkerUpdate;
      ESPhttpKonkerUpdate.rebootOnUpdate(false);
      t_httpUpdate_return ret = ESPhttpKonkerUpdate.update(String(rootDomain), rootPort, String("/firmware/") + String(device_login) +String("/binary"));    
      switch(ret) {
      case HTTP_UPDATE_FAILED:
          Serial.println("[update] Update failed.");
          break;
      case HTTP_UPDATE_NO_UPDATES:
          Serial.println("[update] Update no Update.");
          break;
      case HTTP_UPDATE_OK:          
          updateSucessCallBack(version); 
          ESP.restart();
          break;
      }
    }
}

void checkForUpdates(){
  char expectedVersion[16]="";
  checkForUpdates(_rootDomain, _rootPort, expectedVersion, updateSucessCallBack);
}


#endif