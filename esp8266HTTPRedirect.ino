#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "HTTPSRedirect.h"
#include "DebugMacros.h"
#include "configRedWiFi.h";
#include "datosWeb.h"
#include "mqtt_beebotte.h"

char datos_voluntario = 0 ; 


extern PubSubClient beebotte  ;
//extern WiFiServer AppInventor ; 

void setup()
{
 // wdt_disabled() ; 
  Serial.begin(115200) ;    
  //pinMode(14,INPUT_PULLUP ) ;
  pinMode(LED_BUILTIN, OUTPUT);
  delay(1000)  ;  
  digitalWrite(LED_BUILTIN,HIGH) ;
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
  if (datos_voluntario==1)
  {
     datos_voluntario = 0 ; 
     while (getGas()!='o'){} ; 
  }

}
