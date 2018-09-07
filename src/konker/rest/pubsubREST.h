#ifndef pubsubREST
#define pubsubREST

#include "../helpers/globals.h"
#include <ESP8266HTTPClient.h>


void buildHTTPSUBTopic(char const device_login[], char const channel[], char *topic){
  String SString;
  SString = String(sub_dev_modifier) + String("/") + String(device_login) + String("/") + String(channel); //sub
  SString.toCharArray(topic, SString.length()+1);
}

void buildHTTPPUBTopic(char const device_login[], char const channel[], char *topic){
  String SString;
  SString =  String(pub_dev_modifier) + String("/") + String(device_login) + String("/") +  String(channel); //pub
  SString.toCharArray(topic, SString.length()+1);
}


bool testHTTPSubscribeConn(){
  bool subCode=0;
  char topic[32];
  buildHTTPSUBTopic(device_login, "test", topic);

  HTTPClient http;  //Declare an object of class HTTPClient
  http.addHeader("Content-Type", "application/json");
  http.setAuthorization(device_login, device_pass);

  String url_to_call;
  if (String(_rootURL).indexOf("http://", 0)>0){
    url_to_call=String(_rootURL) + "/" + String(topic);  //Specify request destination
  }else{
    url_to_call=String("http://") + String(_rootURL) + "/" + String(topic);  //Specify request destination
  }

  http.begin(url_to_call);  //Specify request destination
  int httpCode = http.GET();

  Serial.println("Testing HTTP subscribe to: " + url_to_call + "; httpcode:" + String(httpCode));
  Serial.print(">");

  subCode=interpretHTTPCode(httpCode);

  if (!subCode){
    Serial.println("failed");
    Serial.println("");
    return 0;
  }else{
    Serial.println("sucess");
    Serial.println("");

    String strPayload = http.getString();
    Serial.println("strPayload=" + strPayload);
    http.end();   //Close connection
    return 1;
  }
}



bool pubHttp(char const channel[], char const msg[]){
  bool pubCode=0;
  char topic[32];

  buildHTTPPUBTopic(device_login, channel, topic);

  HTTPClient http;  //Declare an object of class HTTPClient
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");
  http.setAuthorization(device_login, device_pass);

  if (String(_rootURL).indexOf("http://", 0)>0){
    http.begin(String(_rootURL) + "/" + String(topic));  //Specify request destination
  }else{
    http.begin(String("http://") + String(_rootURL) + "/" + String(topic));  //Specify request destination
  }

  int httpCode=http.POST(String(msg));
  Serial.println("Publishing to " + String(topic) + "; Body: " + String(msg) + "; httpcode: " + String(httpCode));
  Serial.print(">");
  http.end();   //Close connection

  pubCode=interpretHTTPCode(httpCode);

  if (!pubCode){
    Serial.println("failed");
    Serial.println("");
    failedComm=1;
    return 0;
  }else{
    Serial.println("sucess");
    Serial.println("");
    return 1;
  }

}



void subHttp(char const channel[],CHANNEL_CALLBACK_SIGNATURE){
  bool subCode=0;
  char topic[32];

  buildHTTPSUBTopic(device_login, channel, topic);

  HTTPClient http;  //Declare an object of class HTTPClient
  http.addHeader("Content-Type", "application/json");
  http.setAuthorization(device_login, device_pass);

  if (String(_rootURL).indexOf("http://", 0)>0){
    http.begin(String(_rootURL) + "/" + String(topic));  //Specify request destination
  }else{
    http.begin(String("http://") + String(_rootURL) + "/" + String(topic));  //Specify request destination
  }
  
  int httpCode = http.GET();

  Serial.println("Subscribing to: " + String(topic) + "; httpcode:" + String(httpCode));
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
      unsigned char* payload = (unsigned char*) strPayload.c_str(); // cast from string to unsigned char*
      chan_callback(payload,playloadSize);
    }
  }


}


#endif