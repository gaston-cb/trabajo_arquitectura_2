#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "HTTPSRedirect.h"
#include "DebugMacros.h"
#include "configRedWiFi.h";
#include "datosWeb.h"
#include "mqtt_beebotte.h"


extern PubSubClient beebotte  ;
//extern WiFiServer AppInventor ; 

void setup()
{
 // wdt_disabled() ; 
  Serial.begin(115200) ;    
  pinMode(14,INPUT_PULLUP ) ;
  //digitalWrite(LED_BUILTIN,LOW) ;
  conectar_wifi() ; 
  initMQTT() ;   
  initServerTCP() ; 
  //getGas() ; 


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
    
    delay(10) ;
    beebotte.loop() ;
  }
  delay(10) ; 
  data_received() ; 
  Serial.flush() ; 
}
