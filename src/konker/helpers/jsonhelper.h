#ifndef jsonhelper
#define jsonhelper

#include <ArduinoJson.h>
//Buffer das mensagens MQTT
char bufferJ[1024];

/*
long long char2LL_old(char *str)
{
long long result = 0; // Initialize result
// Iterate through all characters of input string and update result
for (int i = 0; str[i] != '\0'; ++i){


result = result*10 + str[i] - '0';
}

return result;
}*/


long long char2LL(char *str){
	long long result = 0; // Initialize result
	long long m=1000000000000;
	// Iterate through all characters of input string and update result
	for (int i = 0; m>=1; ++i){
		result = result + str[i] * m;
		m=m/10;
	}

	return result;
}



//DEPRECATED, use directly >> jsonMSG.printTo(bufferJ, sizeof(bufferJ));
//char *buildJSONmsg(JsonObject& jsonMSG){
//	jsonMSG.printTo(bufferJ, sizeof(bufferJ));
//	return bufferJ;
//}




bool parse_JSON_item(JsonObject& jsonMSG, char *itemName, char *returnVal){
	if (jsonMSG.containsKey(itemName)){
		strcpy(returnVal,jsonMSG[itemName]);
		return 1;
	}else{
		return 0;
	}
}

//----------------- Decodificacao da mensagem Json In -----------------------------
bool parse_JSON_item(String json, char *itemName, char *returnVal){
	StaticJsonBuffer<1024> jsonBuffer;
	JsonObject& jsonMSG = jsonBuffer.parseObject(json);
	return parse_JSON_item(jsonMSG,itemName, returnVal);
}


char  *parse_JSON_dataItem(String json, char *itemName){
	const char *ival = NULL;
	Serial.println("Parsing json item: " + (String)itemName);
	StaticJsonBuffer<1024> jsonBuffer;
	JsonArray& array = jsonBuffer.parseArray(json);
	JsonObject& jsonMSG = array[0]["data"];
	if (jsonMSG.containsKey(itemName)) ival = jsonMSG[itemName];
	char *it = (char*) ival;

	return it;
}


void  parse_JSON_timestamp(String json, char *chrTS, int chrTSsize){
	const char *ival = NULL;
	StaticJsonBuffer<1024> jsonBuffer;
	JsonArray& array = jsonBuffer.parseArray(json);
	JsonObject& jsonMSG = array[0]["meta"];
	if (jsonMSG.containsKey("timestamp")) ival = jsonMSG["timestamp"];
	//char *it = (char*)ival;
	strncpy(chrTS,(char*)ival,chrTSsize);

}



void updateJSON(JsonObject& jsonToUpdate,  String keyNameToSave,  String itemValue){
	if (jsonToUpdate.containsKey(keyNameToSave)){
		Serial.print("Key json found: " + keyNameToSave);
		String fileValue=jsonToUpdate[keyNameToSave];
		if(fileValue!=itemValue ){
			jsonToUpdate[keyNameToSave]=itemValue;
			Serial.println(", value updated!");
		}else{
			//Serial.println(", sem alterações de valor.");
		}
	}else{
		// Key not found... Creating it
		Serial.print("Key json not found, creating key: " + keyNameToSave);
		jsonToUpdate.createNestedObject(keyNameToSave);
		jsonToUpdate[keyNameToSave]=itemValue;
	}
}



void updateJSON(JsonObject& jsonToUpdate, JsonObject& jsonNewValues){
	//Serial.println("Checando valores recebidos..");
	for (JsonObject::iterator it=jsonNewValues.begin(); it!=jsonNewValues.end(); ++it) {
		String keyNameToSave=it->key;
		updateJSON(jsonToUpdate,keyNameToSave,(it->value));
	}

}



bool updateJsonFile(String filePath, JsonObject& jsonNewValues){
	char fileContens[1024];
	//first read file...
	Serial.println("updateJsonFile, opening file to update");
	if(readFile(filePath,fileContens)){
		Serial.println("Parsing: " + (String)fileContens);
	}else{
		Serial.println("Failed to open, creating it :" + filePath);
		jsonNewValues.printTo(bufferJ, sizeof(bufferJ));
		if(!saveFile(filePath, bufferJ)){
			Serial.println("Failed to create file : " + filePath);
			return 0;
		}else{
			return 1;
		}
	}



	//updating file
	DynamicJsonBuffer jsonBuffer;
	JsonObject& jsonFromFile = jsonBuffer.parseObject(fileContens);
	if (jsonFromFile.success()) {
		Serial.println("Updating json readed from file");
		updateJSON(jsonFromFile,jsonNewValues);

		Serial.println("Saving file with changed values..");
		jsonNewValues.printTo(bufferJ, sizeof(bufferJ));
		return saveFile(filePath, bufferJ);
	}else{
		Serial.println("Failed to read Json file");
		return 0;
	}
}



bool updateJsonFile(String filePath, String jsonString){
	Serial.println("updateJsonFile, parsing jsonString..");
	//updating file
	DynamicJsonBuffer jsonBuffer;
	JsonObject& jsonParsed = jsonBuffer.parseObject(jsonString);
	if (jsonParsed.success()) {
		return updateJsonFile(filePath,jsonParsed);
	}else{
		Serial.println("Failed to parse: " + jsonString);
		return 0;
	}

}


bool  getJsonItemFromFile(String filePath, char *itemName, char *returnVal){
	char jsonfileContens[1024];
	//first read file...
	Serial.println("Opening file to read");
	if(readFile(filePath,jsonfileContens)){
		Serial.println("Parsing: " + (String)jsonfileContens);
	}else{
		Serial.println("Failed to open file: " + filePath);
		return 0;
	}

	//updating file
	DynamicJsonBuffer jsonBuffer;
	JsonObject& fileJson = jsonBuffer.parseObject(jsonfileContens);
	if (fileJson.success()) {
		return parse_JSON_item(fileJson,itemName,returnVal);
	}else{
		Serial.println("Failed to read Json file");
		return 0;
	}
}







#endif
