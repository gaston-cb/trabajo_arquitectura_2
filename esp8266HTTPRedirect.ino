#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "HTTPSRedirect.h"
#include "DebugMacros.h"
#include "configRedWiFi.h";
#include "datosWeb.h"
#include "mqtt_beebotte.h"


extern PubSubClient beebotte  ;
extern WiFiServer AppInventor ; 

void setup()
{
 // wdt_disabled() ; 
  
  Serial.begin(115200) ;   
 
  pinMode(14,INPUT_PULLUP ) ;
  //digitalWrite(LED_BUILTIN,LOW) ;
  conectar_wifi() ; 
  Serial.println(postGAS("hola_mundo",-58.2589,-34.25686)) ;   
  initMQTT() ;   
  initServerTCP() ; 
 


} 
void loop() {
  if (WiFi.status()!=WL_CONNECTED){
    conectar_wifi() ; 
  }
  
  if (!beebotte.connected())
  {
     if(!reconnect()){
       Serial.println("\n reconexion") ;  
  
      } ;   
  }else 
  { 
    
    delay(50) ;
    beebotte.loop() ;
  }
  data_received() ; 
  /*if (digitalRead(14)==LOW){
      char *data = "2586,32558,658" ; 
      publicar(data) ; 
  }*/
}
