#ifndef pubsubMQTT
#define pubsubMQTT

#include "../helpers/globals.h"
#include <PubSubClient.h>

#ifdef konkerMQTTs
  WiFiClientSecure espClient;
  #include "secureCheck.h"
#endif
#ifndef konkerMQTTs
	WiFiClient espClient;
#endif


PubSubClient client(espClient);

void MQTTLoop(){
  client.loop();
}

bool connectMQTT(char r_server[], int r_port, char r_device_login[], char r_device_pass[]){
    Serial.print("Trying to connect to MQTT broker ");


	  Serial.print(" URI:" + String(server) + " Port:" + String(port) + ", ");

	  client.setServer(r_server, r_port);
	  client.setCallback(callback);

	  Serial.print(" U:" + String(r_device_login) + " P:" + String(r_device_pass));
	  
		int connectCode=client.connect(r_device_login, r_device_login, r_device_pass);

		Serial.println(", connectCode=" + String(connectCode));


    if (connectCode==1) { //Check the returning code
      Serial.println("sucess");
      Serial.println("");
      return 1;
    }else{
      Serial.println("failed");
      Serial.println("");
      return 0;
		}
}


bool connectMQTT(){
	return connectMQTT(server,port,device_login,device_pass);
}

void callback(char* topic, byte* payload, unsigned int length) {
  int i;
  int state=0;

  Serial.print("Message received [");
  Serial.print(topic);
  Serial.print("] ");

  callSubChannelCallback(topic, payload, length);

}



void buildMQTTSUBTopic(char const device_login[], char const channel[], char *topic){
  String SString;
  SString = String(prefix) + String("/") + String(device_login) + String("/") + String(sub_dev_modifier) + String("/") + String(channel); //sub
  SString.toCharArray(topic, SString.length()+1);
}

void buildMQTTPUBTopic(char const device_login[], char const channel[], char *topic){
  String SString;
  SString = String(prefix) + String("/") + String(device_login) + String("/") + String(pub_dev_modifier) + String("/") + String(channel); //pub
  SString.toCharArray(topic, SString.length()+1);
}



bool pubMQTT(char const channel[], char const msg[]){
  int pubCode=-1;
  char topic[32];

  buildMQTTPUBTopic(device_login, channel, topic);

  Serial.println("Publishing to: " + String(topic) + " msg: " + msg );
  Serial.print(">");
  delay(200);
  pubCode=client.publish(topic, msg);

  if (pubCode!=1){
    Serial.println("failed");
    Serial.println("pubCode:" + (String)pubCode);
    failedComm=1;
    return 0;
  }else{
    Serial.println("sucess");
    Serial.println("pubCode:" + (String)pubCode);
    return 1;
  }

}



bool subMQTT(char const channel[],CHANNEL_CALLBACK_SIGNATURE){
  int subCode=-1;
  char topic[32];

  buildMQTTSUBTopic(device_login, channel, topic);
  
  Serial.println("Subscribing to: " + String(topic));
  Serial.print(">");
  delay(200);
  subCode=client.subscribe(topic);

  if (subCode!=1){
    Serial.println("failed");
    Serial.println("");
    failedComm=1;
    return 0;
  }else{
    addSubChannelTuple(topic,chan_callback);
    Serial.println("sucess");
    Serial.println("");
    return 1;
  }

}

#endif
