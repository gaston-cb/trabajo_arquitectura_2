#include <Arduino.h> 
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "datosWeb.h" 

WiFiClient espClient;
PubSubClient beebotte(espClient); // PROTOCOLO mqtt 
extern char datos_voluntario; 
extern unsigned char pedido_ayuda ; 


void onMessage(char* topic, byte* payload, unsigned int length) {
  /* Beebotte resources 
   * Resources:  mqtt8266 -- general 
   *             presion_diastolica
   *             presion_sistolica
   *             temperatura 
   *             pulso 
  */
  StaticJsonDocument<128> jsonInBuffer; 
  DeserializationError err = deserializeJson(jsonInBuffer,payload) ; 
  // Test if parsing succeeds.
  if (err) {
    Serial.println("parseObject() failed");
    return;
  }
  
  Serial.print("Received message of length ");
  Serial.print(length);
  Serial.println();
  Serial.print("data ");
  // reviso segun el topic suscrito ; 
  if (strcmp(topic,"TestESP8266/mqtt8266")==0)
  {
    const char *data  = jsonInBuffer["data"]; 
    String comp = data ; 
    Serial.print(data);
    if (strcmp(data,"help")==0)
    {
      pedido_ayuda = 1 ; 
      Serial.println("pedido_ayuda_activado") ; 
     digitalWrite(LED_BUILTIN,LOW) ; 
      
    }else if (strcmp(data,"voluntario_disponible")==0)
    {
      pedido_ayuda = 1 ; 
      Serial.println("hay un voluntario disponible") ;
      datos_voluntario = 1 ;  
      digitalWrite(LED_BUILTIN,LOW) ; 
    }else if (strcmp(data,"vol_casaPR")==0)
    {
       Serial.println("El voluntario se encuentra en el domicilio") ; 
       pedido_ayuda = 3 ; 
       digitalWrite(LED_BUILTIN, HIGH); 
    }
  }else if(strcmp(topic,"TestESP8266/presion_diastolica")==0)
  {
    //const int *data  = jsonInBuffer["data"]; 
    int presion = jsonInBuffer["data"];
    Serial.println(presion);
  
  }else if(strcmp(topic,"TestESP8266/presion_sistolica")==0)
  {
    int presion = jsonInBuffer["data"];
    Serial.println(presion);
  }else if (strcmp(topic,"TestESP8266/temperatura")==0)
  {
    int presion = jsonInBuffer["data"];
    Serial.println(presion);
  }else if (strcmp(topic,"TestESP8266/pulso")==0)
  {
    int presion = jsonInBuffer["data"];
    Serial.println(presion);
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
  /* Beebotte resources 
   * Resources:  mqtt8266 -- general 
   *             presion_diastolica
   *             presion_sistolica
   *             temperatura 
   *             pulso 
  */
  if (beebotte.connect(generateID(), TOKEN, "")) 
  {
      char topic[64];
      sprintf(topic, "%s/%s", CHANNEL, RESOURCE);
      beebotte.subscribe(topic);
       
      sprintf(topic, "%s/%s", CHANNEL,"presion_diastolica" );
      beebotte.subscribe(topic);
      sprintf(topic, "%s/%s", CHANNEL,"presion_sistolica" );
      beebotte.subscribe(topic);
      sprintf(topic, "%s/%s", CHANNEL,"temperatura" );
      beebotte.subscribe(topic);
      sprintf(topic, "%s/%s", CHANNEL,"pulso" );
      beebotte.subscribe(topic);
      
      Serial.println("Connected to Beebotte MQTT");
   
  }
  return beebotte.connected();
}



void publicar(const char* datos,const char *resource,int number)
{
   /* Beebotte resources 
   * Resources:  mqtt8266 -- general 
   *             presion_diastolica
   *             presion_sistolica
   *             temperatura 
   *             pulso 
  */
 
  StaticJsonDocument<128> mqtt_bebbotte; 
   
  mqtt_bebbotte["Resource"] = resource ;   
 
  mqtt_bebbotte["write"] = true ; 
  if (number == 0 )
  {
    mqtt_bebbotte["data"] = datos ;
  }else
  {
    mqtt_bebbotte["data"] = number ; 
  }
  
  serializeJson(mqtt_bebbotte,Serial) ; 
  char buffer[128] ;   
  serializeJson(mqtt_bebbotte,buffer) ; 
  char topic[40] ; 
  sprintf(topic,"%s/%s",CHANNEL,resource) ; 
  beebotte.publish(topic,buffer) ;  
}


void initMQTT() {
  /* Beebotte resources 
   * Resources:  mqtt8266 -- general 
   *             presion_diastolica
   *             presion_sistolica
   *             temperatura 
   *             pulso 
  */
  
  beebotte.setServer(BBT, 1883);  
  beebotte.setCallback(onMessage);  
  if (beebotte.connect(generateID(), TOKEN, "")) {
    char topic[64];
    sprintf(topic, "%s/%s", CHANNEL, RESOURCE);
    beebotte.subscribe(topic);
    sprintf(topic, "%s/%s", CHANNEL,"presion_diastolica" );
    beebotte.subscribe(topic);
    sprintf(topic, "%s/%s", CHANNEL,"presion_sistolica" );
    beebotte.subscribe(topic);
    sprintf(topic, "%s/%s", CHANNEL,"temperatura" );
    beebotte.subscribe(topic);
    sprintf(topic, "%s/%s", CHANNEL,"pulso" );
    beebotte.subscribe(topic);
    
    
    Serial.println("Connected to Beebotte MQTT");
  }


}
