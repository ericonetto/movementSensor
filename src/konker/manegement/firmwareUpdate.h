
#ifndef firmwareUpdate
#define firmwareUpdate

#include "../helpers/globals.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>



#ifdef ESP8266
#include <functional>
#define CALLBACK_SIGNATURE std::function<bool(uint8_t*, unsigned int)> chan_callback
#else
#define CALLBACK_SIGNATURE bool (*chan_callback)(uint8_t*, unsigned int)
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


bool checkVersionCallBack(byte* payload, unsigned int length){
    return 1;
}


//http://localhost:8090/registry-data/firmware/2q7kibmutjdj
bool hasUpdate(char const rootURL[], CALLBACK_SIGNATURE){
  bool subCode=0;

  String fwUpdateURL;
  if (String(_rootURL).indexOf("http://", 0)>0){
    fwUpdateURL = String(rootURL) + String("/firmware/") + String(device_login); 
  }else{
    fwUpdateURL = "http://" + String(rootURL) + String("/firmware/") + String(device_login); 
  }


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
    return 0;
  }else{
    Serial.println("sucess");
    Serial.println("");

    String strPayload = http.getString();
    Serial.println("strPayload=" + strPayload);
    int playloadSize=http.getSize();
    http.end();   //Close connection
    if (strPayload!="[]"){
      unsigned char* payload = (unsigned char*) strPayload.c_str(); // cast from string to unsigned char*
      return chan_callback(payload,playloadSize);
    }
  }
  Serial.println("failed");
  Serial.println("");
  return 0;
}


//http://localhost:8090/registry-data/firmware/2q7kibmutjdj
bool hasUpdate(char const rootURL[]){
    return hasUpdate(rootURL, checkVersionCallBack);
}


void checkForUpdates(){
    if (hasUpdate(_rootURL)){
        String fwUpdateURL;
        if (String(_rootURL).indexOf("http://", 0)>0){
          fwUpdateURL = String(_rootURL); 
        }else{
          fwUpdateURL = "http://" + String(_rootURL); 
        }
        Serial.println("UPDATING...."); 
        ESP8266HTTPKonkerUpdate ESPhttpKonkerUpdate;
        //curl -u 23sv4apjs6p4:aJI832fjsog6 -X GET http://192.168.0.52:8090/firmware/23sv4apjs6p4/binary
        //ESPhttpUpdate.update(fwUpdateURL, 8090, String("/firmware/") + String(device_login) +String("/binary")); 
        String testUpdate =  "http://192.168.0.52";

        
        t_httpUpdate_return ret = ESPhttpKonkerUpdate.update("192.168.0.52", 8090, String("/firmware/") + String(device_login) +String("/binary"));    
        switch(ret) {
        case HTTP_UPDATE_FAILED:
            Serial.println("[update] Update failed.");
            break;
        case HTTP_UPDATE_NO_UPDATES:
            Serial.println("[update] Update no Update.");
            break;
        case HTTP_UPDATE_OK:
            Serial.println("[update] Update ok."); // may not called we reboot the ESP
            break;
      }
    }
}


#endif