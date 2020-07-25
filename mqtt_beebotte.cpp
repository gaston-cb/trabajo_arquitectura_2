#include <Arduino.h> 
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "datosWeb.h" 

WiFiClient espClient;
PubSubClient beebotte(espClient); // PROTOCOLO mqtt 



void onMessage(char* topic, byte* payload, unsigned int length) {
  // decode the JSON payload
  //const size_t CAPACITY = JSON_ARRAY_SIZE(2);
 // char *data ; 
  StaticJsonDocument<128> jsonInBuffer; 
  DeserializationError err = deserializeJson(jsonInBuffer,payload) ; 
  // Test if parsing succeeds.
  if (err) {
    Serial.println("parseObject() failed");
    return;
  }

  // led resource is a boolean read it accordingly
  const char *data  = jsonInBuffer["data"];
  const String Comp = data ; 
  // Set the led pin to high or low
  

  // Print the received value to serial monitor for debugging
  Serial.print("Received message of length ");
  Serial.print(length);
  Serial.println();
  Serial.print("data ");
  Serial.print(data);
  Serial.println();
  Serial.println(Comp + ", " + "ayuda") ; 
  if (Comp=="ayuda"){
      Serial.println("dentro de comp")  ;     
      digitalWrite(LED_BUILTIN,LOW) ;
    }
}




const char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

const char * generateID()
{
  //const char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
  char id[17];
  randomSeed(analogRead(0));
  int i = 0;
  for(i = 0; i < sizeof(id) - 1; i++) {
    id[i] = chars[random(sizeof(chars))];
  }
  id[sizeof(id) -1] = '\0';

  return id;
}




boolean reconnect() {
  if (beebotte.connect(generateID(), TOKEN, "")) {
    char topic[64];
    sprintf(topic, "%s/%s", CHANNEL, RESOURCE);
    beebotte.subscribe(topic);
    Serial.println("Connected to Beebotte MQTT");
    Serial.print("olalalalal") ; 
  }
  return beebotte.connected();
}



void publicar(const char* datos){
  //const int tam = JSON_OBJECT_SIZE(2) +100 ; 
  StaticJsonDocument<128> mqtt_bebbotte; 
   
  mqtt_bebbotte["Resource"] = "mqtt8266" ;   
  mqtt_bebbotte["data"] = datos ; 
  mqtt_bebbotte["write"] = true ; 
  serializeJson(mqtt_bebbotte,Serial) ; 
  char buffer[128] ; 
  serializeJson(mqtt_bebbotte,buffer) ; 
  char topic[40] ; 
  sprintf(topic,"%s/%s",CHANNEL,RESOURCE) ; 
  beebotte.publish(topic,buffer) ;  
}


void initMQTT() {
  beebotte.setServer(BBT, 1883);  
  beebotte.setCallback(onMessage);  
  if (beebotte.connect(generateID(), TOKEN, "")) {
    char topic[64];
    sprintf(topic, "%s/%s", CHANNEL, RESOURCE);
    beebotte.subscribe(topic);
    Serial.println("Connected to Beebotte MQTT");
  }


}
