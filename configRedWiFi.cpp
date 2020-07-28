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



char postGAS(char *dato1, float latitud,float longitud) 
{
  /* ---------------- URLS Y DEFINICION DE PUERTOS ------------------------*/
  static char cant_veces = 0 ;
  
  String IDGAS = "AKfycbyC5EeC2GMrgbfqBfslR3o4tPxg-WATkut75z2BVBPmAwsun1E" ; 
  char* host = "script.google.com" ; 
  int httpsPort = HTTPS_PORT ; 
  char *fingerprint = FINGERPRINT ; 
 

  
  String payload = ""  ; 
  String url = String("/macros/s/") +IDGAS + "/exec" ; 
  
  
  HTTPSRedirect* client = new HTTPSRedirect(HTTPS_PORT);
  client->setInsecure() ; 
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
 
  // conexion -- espera 5 veces 
  bool flag = false;
  for (int i=0; i<5; i++){
    int retval = client->connect(host, httpsPort);
    delay(50) ; 
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
 // payload = "{\"data\":\"hola mundo\",\"coordinates\":[{\"latitud\":-54.0886256,\"long\":-34.25685258}]}" ; 
  //sprintf()
  payload = "{\"data\":\""+ String(dato1) +"\",\"coordinates\":[{\"latitud\":"+String(latitud,5)+",\"long\":"+String(longitud,5)+"}]}" ; 
  Serial.print("payload:  ") ; Serial.println(payload) ; 
  
  client->POST(url,host,payload,true) ;
  
  String bodyPost = client->_myResponse.body;
  Serial.println("respuestapost: " + bodyPost) ; 
  Serial.println("fin_setup") ;   
  delete client; 
  return 'o' ;  
}



/*-----------------------Funciones para servidor TCP IP -------------------------------------------*/ 


void initServerTCP(){
  AppInventor.begin() ; 
}


void data_received(){
  int index = 0 ; 
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
  if (request==""){
    return ; 
  }
 
 /* -----------------Separacion de datos recibidos desde app inventor por TCP/IP - PUERTO:2000 --------------------------- */
 char help [5]  ; 
 char id[3] ; 
 char nombre [10] ;
 char telefono[10] ;  
 float latitud ; 
 float longitud ; 
 (request.substring(0,4)).toCharArray(help,5) ;
 (request.substring(5,7)).toCharArray(id,3) ; 
 index = request.indexOf(',',8) ; // ind 1  
 (request.substring(8,index)).toCharArray(nombre,10) ; 
 (request.substring(index+1,request.indexOf(',',index+1))).toCharArray(telefono,10) ; 
 index = request.indexOf(',',index+1); 
 latitud = (request.substring(index+1,request.indexOf(',',index+1))).toFloat(); 
 index = request.indexOf(',',index+1); 
 longitud = (request.substring(index+1)).toFloat() ; 

 /* ------------------Final de separacion de datos datos recibidos desde app inventor por TCP/IP - PUERTO:2000 --------------------------- */
    if (postGAS(id,latitud,longitud)=='o')
    {
      Serial.println("envio al gas correcto") ;   
      
    } ; 
  
 /*-------------------Envio al Google Apps Script ------------------------------------------------------------------------------------------ */


  
 /*-------------------Fin envio google apps script-------------------------------------------------------------------------------------------*/
   
 /*
  if (request == "help\n"){
      //Serial.println("publicar") ; 
      publicar(request.c_str()) ; //publica 
  } 
  
*/       
}




 
