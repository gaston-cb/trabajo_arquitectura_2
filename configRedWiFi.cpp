#include <ESP8266WiFi.h> 
#include <ArduinoJson.h> 
#include "datosWeb.h"
#include "HTTPSRedirect.h"
#include "DebugMacros.h"
#include "mqtt_beebotte.h"


WiFiServer AppInventor(PORT_TCP)  ; 



void conectar_wifi() 
{
  IPAddress ip(192, 168, 0, 150);
  IPAddress gateway(192, 168, 0, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress dns1(8, 8, 8, 8); //dnsGoogle
  IPAddress dns2(181, 30, 128, 20); //dnsFibertel
  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet, dns1, dns2);
  WiFi.begin(REDWIFI,PASSWIFI) ;
  Serial.print("Conectando a:\t");
  Serial.println(REDWIFI);
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print('.');
  }

  // Mostrar mensaje de exito y dirección IP asignada
  Serial.println();
  Serial.print("Conectado a:\t");
  Serial.println(WiFi.SSID());
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
   
  
} 



char postGAS(char *dato1,float dato2,float dato3) 
{
  /* ---------------- URLS Y DEFINICION DE PUERTOS ------------------------*/
  String IDGAS = "AKfycbyC5EeC2GMrgbfqBfslR3o4tPxg-WATkut75z2BVBPmAwsun1E" ; 
  char* host = "script.google.com" ; 
  int httpsPort = HTTPS_PORT ; 
  char *fingerprint = FINGERPRINT ; 
  String payload_base =  "{\"command\": \"appendRow\", \
                    \"sheet_name\": \"Sheet1\", \
                    \"values\": ";
  
  //"{\"data\":\"hola mundo\",\"coordinates\":[{\"latitud\":-54.0886256,\"long\":-34.25685258}]}";
  
  String payload = ""  ; 
  String url = String("/macros/s/") +IDGAS + "/exec" ; 
//   
  
  HTTPSRedirect* client = new HTTPSRedirect(HTTPS_PORT);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
 
  // conexion -- espera 5 veces 
  bool flag = false;
  for (int i=0; i<5; i++){
    int retval = client->connect(host, httpsPort);
    Serial.print("i=") ; Serial.println(i) ; 
    if (retval == 1) {
       flag = true;
       break;
    }
  }
  if (!flag){
      return 'n' ; 
    
    }
  
  Serial.println("init_post") ; 
  payload = "{\"data\":\"hola mundo\",\"coordinates\":[{\"latitud\":-54.0886256,\"long\":-34.25685258}]}" ; 
  Serial.print("payload:  ") ; Serial.println(payload) ; 
  
  client->POST(url,host,payload,true) ;
  
  String bodyPost = client->_myResponse.body;
  Serial.println("respuestapost: " + bodyPost) ; 
  Serial.println("fin_setup") ;   
  return 'o' ;  
}



/*-----------------------Funciones para servidor TCP IP -------------------------------------------*/ 


void initServerTCP(){
  AppInventor.begin() ; 
}


void data_received(){
 
  WiFiClient clientes = AppInventor.available();
  String request = ""  ;
  if (clientes) {
    while (clientes.connected()) {
      Serial.println("clientes_connected") ; 
      while (clientes.available()>0) {        
        request = clientes.readString() ;       
      }
      
      Serial.print("request:") ; Serial.print(request) ; 
      Serial.print(": despues req") ; 
      clientes.println("hola soy MACRI_GATO") ;
      clientes.flush() ; 
      Serial.println("clientes_availables") ;
      delay(100);    
    }
    clientes.stop();
    Serial.println("Client disconnected");
  }

 // Serial.println("revisar_request") ; 
  if (request == "help\n"){
      //Serial.println("publicar") ; 
      publicar(request.c_str()) ; //publica 
  } 
  
      
}




 
