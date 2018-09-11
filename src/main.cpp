
#include <Arduino.h>
#include "./konker/mqtt/pubsubMQTT.h"
#include "./konker/konker.h"


#define presence_pin  5


bool estadoPorta=false; // LOW=fechada - HIGH =aberta
bool ultimoEstadoPorta=false;//guarda o último valor do estado da porta
bool lerEstadoPorta=true;

char status_channel[] = "status";

unsigned long intPeriodoEnvio=10000;


int CURRENT_FIRMWARE_VERSION=1;


char *type;
char typeC[32];
int presenceCount=0;
bool presenceDetected=0;
unsigned long lasttimeCheck=0;

ADC_MODE(ADC_VCC);


void enviaPresencas(){
      checkConnections();
      Serial.println("lasttimeCheck=" +(String)lasttimeCheck);
      Serial.println("intPeriodoEnvio=" +(String)intPeriodoEnvio);
      lasttimeCheck = millis();


      StaticJsonBuffer<220> jsonBuffer;
      JsonObject& jsonMSG = jsonBuffer.createObject();


      delay(10);

      jsonMSG["deviceId"] = (String)getChipId();
      jsonMSG["p"] = presenceCount;

      jsonMSG.printTo(bufferJ, sizeof(bufferJ));
      char mensagemjson[1024];
      strcpy(mensagemjson,bufferJ);
      Serial.println("Publicando no canal:" + (String)status_channel);
      Serial.println("A mensagem:");
      Serial.println(mensagemjson);

      if(!pubHttp(status_channel, mensagemjson)){
        appendToFile(healthFile,(char*)"1", _mqttFailureAdress);

        delay(3000);
        ESP.restart();
      }else{
        healthUpdate(health_channel);
      }
}



//atualiza contador de presença
void presencafunc(){
  if (digitalRead(presence_pin) == LOW){

		if(presenceDetected==0){
			presenceCount=presenceCount+1;
			Serial.println("presenceCount=" + (String)presenceCount);
		}
		presenceDetected=1;
  }else {
		presenceDetected=0;
  }
}


// byte* payload, unsigned int length or you could use CHANNEL_CALLBACK_SIGNATURE
void ledCallback(byte* payload, unsigned int length){
    int i;
    int state=0;
    char *receivedMsg= new char[length];
    Serial.print("Mensagem recebida [");
    Serial.print("led");
    Serial.print("] :");
    for (i = 0; i < length; i++) {
    receivedMsg[i] = (char)payload[i];
    Serial.print((char)payload[i]);
    }
    receivedMsg[i] = '\0';

    Serial.println(String(receivedMsg));

    char ligado[8];
    if(parse_JSON_item(receivedMsg,"ligado",ligado)){
        bool state =atoi(ligado);
        digitalWrite(D2, state);
        Serial.println("Led : " + String(state));
    }else{
        Serial.println("Failed to parse"); 
    }

}

void setup(){
    Serial.begin(115200);
    Serial.println("Setup");

    Serial.println("BUILD: " + (String)PIO_SRC_REV);

	//uncomment for tests
    //resetALL();

    //change flag to true to use encripted wifi password
    konkerConfig((char*)"data.demo.konkerlabs.net:80",(char*)"S0101",false);

    //statusUpdate();

    pinMode(D2, OUTPUT);
    subMQTT("led",ledCallback);


    pinMode(presence_pin, INPUT);

	Serial.println("Setup finished");
	//Serial.println("Turning off Wifi");
	//client.disconnect();
	//WiFi.mode(WIFI_OFF);
	delay(1000);


    lasttimeCheck = millis();

}

void loop(){

    konkerLoop();
    delay(100);
    presencafunc();


    if ((millis()-lasttimeCheck) > intPeriodoEnvio){

        enviaPresencas();

        checkForUpdates();

        lasttimeCheck = millis();
        presenceCount=0;
	}
}