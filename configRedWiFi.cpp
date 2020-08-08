#include <ESP8266WiFi.h> 
#include <PubSubClient.h>
#include <ArduinoJson.h> 
#include "datosWeb.h"
#include "HTTPSRedirect.h"
#include "DebugMacros.h"
#include "mqtt_beebotte.h"

unsigned char pedido_ayuda = 0 ; 
WiFiServer AppInventor(PORT_TCP)  ; 
extern PubSubClient beebotte  ;
extern char datos_voluntario ; 
char nombre_vol[20]  ; 
char tel[13]    ;
char idvol[3] ;  





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
  //Serial.print("s1:") ; Serial.println(s1) ;  
  
} 






char postGAS(char *idVol, float latitud,float longitud,char *PR) 
{
  /* ---------------- URLS Y DEFINICION DE PUERTOS ------------------------*/
  String IDGAS = "AKfycbyC5EeC2GMrgbfqBfslR3o4tPxg-WATkut75z2BVBPmAwsun1E" ; 
  char* host = "script.google.com" ; 
  int httpsPort = HTTPS_PORT ; 
  char *fingerprint = FINGERPRINT ;   
  String payload = ""  ; 
  String url = String("/macros/s/") +IDGAS + "/exec" ; 
 
  HTTPSRedirect*client = new HTTPSRedirect(httpsPort); 
  client->setInsecure() ;  
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
 // conexion -- espera 5 veces 
  bool flag = false;
  for (int i=0; i<5; i++){
    int retval = client->connect(host, httpsPort);    
    if (retval == 1) {
       Serial.println("conectado a GAS") ; 
       flag = true;
       break;
    }
  }
  if (!flag){
      return 'n' ;    
  }  
 
  payload = "{\"ID\":\""+ String(idVol) +"\",\"coordinates\":[{\"latitud\":"+String(latitud,6)+",\"long\":"+String(longitud,6)+"}],\"PR\":\""+String(PR)+"\"}" ; 
  client->POST(url,host,payload,true) ;  
  String bodyPost = client->_myResponse.body;
  Serial.println("respuestapost: " + bodyPost) ; 
  Serial.println("fin_setup") ;    

  delete client; 
  return 'o' ; 
  //initMQTT() ;  
}



/*-----------------------Funciones para servidor TCP IP -------------------------------------------*/ 


void initServerTCP(){
  AppInventor.begin() ; 
}


void data_received(){
  int index = 0 ; 
  WiFiClient clientes = AppInventor.available();
  String request = ""  ;
  
  if (clientes) 
  {
    while (clientes.connected()) {
      Serial.println("clientes_connected") ; 
      while (clientes.available()>0) 
      {        
        request = clientes.readString() ;       
      }      
      Serial.print("request:") ; Serial.print(request) ; 
      if(request!="")
      {
        if (pedido_ayuda==3)
        { 
            clientes.println("voluntarioencasa") ;          
        }else if (datos_voluntario == 3)
        {
          datos_voluntario = 0 ;
          clientes.println("voluntariodisponible") ;
        }else if (request =="dataDiario")
        {
          pedido_ayuda = 4 ; 
          clientes.println("mensaje recibido") ;
        }else
        {
          clientes.println("mensaje recibido") ;
        }
      }
      
      Serial.println("clientes_availables") ;
      clientes.flush() ;
      delay(10);    
    }
    clientes.stop();
    Serial.println("Client disconnected");
  }
  if (request=="")
  {
    return ; 
  }
 
 /* -----------------Separacion de datos recibidos desde app inventor por TCP/IP - PUERTO:2000 --------------------------- */
  if (pedido_ayuda == 0 )
  { 
     char help [5]  ; 
     char id[3] ; 
     char nombre [10] ;
     char telefono[10] ;  
     float latitud ; 
     float longitud ; 
     char nombre_pr[15]  ;  
     (request.substring(0,4)).toCharArray(help,5) ;
     (request.substring(5,7)).toCharArray(id,3) ; 
     index = request.indexOf(',',8) ; // ind 1  
     (request.substring(8,index)).toCharArray(nombre,10) ; 
     (request.substring(index+1,request.indexOf(',',index+1))).toCharArray(telefono,10) ; 
     index = request.indexOf(',',index+1); 
     latitud = (request.substring(index+1,request.indexOf(',',index+1))).toFloat(); 
     index = request.indexOf(',',index+1); 
     longitud = (request.substring(index+1,request.indexOf(',',index+1))).toFloat() ; 
     index = request.indexOf(',',index+1)  ;
     (request.substring(index+1,request.length()-1)).toCharArray(nombre_pr,15) ; 
     request = "" ; 
     // envio al GAS para enviar mails con ubicacion -- 
     while (postGAS(id,latitud,longitud,nombre_pr)!='o')
     {
        Serial.println("envio al gas incorrecto") ;         
     }
     Serial.println("envio al gas correcto") ;  
     request = "";    
     publicar(help,"mqtt8266",0) ;
     return ;  
  }else if (pedido_ayuda==1)
  {
    index = request.indexOf("Presión SIST: ");
    index = request.indexOf(": ",index);  
    int presion_sistolica =request.substring(index+2,request.indexOf(' ',index+3)).toInt() ; 
    publicar("help","presion_sistolica",presion_sistolica) ;
     
    index = request.indexOf("Presión DIAS:") ; 
    index = request.indexOf(": ",index) ; 
    int presion_diastolica = request.substring(index+2,request.indexOf(' ',index+3)).toInt(); 
    publicar("help","presion_diastolica",presion_diastolica) ;
    index = request.indexOf("Pulso:")  ;  
    index = request.indexOf(':',index+1) ; 
    int pulso = request.substring(index+9,request.indexOf("lat/min")-1).toInt();
    publicar("help","pulso",pulso) ;
    index = request.indexOf("°C"); 
    float temp = request.substring(index-4,index).toFloat() ;
    int temper = temp *1000 ; 
    publicar("help","temperatura",temper) ;
    
  }else if (pedido_ayuda==3)
  {
    Serial.println("voluntario en lugar - salir de emergencia del dispositivo - ") ; 
    pedido_ayuda = 0 ; 
  }/* else if (pedido_ayuda==4)
  {
    pedido_ayuda=0
    //publicar datos diarios -- 
   publicar("help","pulso_diario",temper) ;
    publicar("help","presion_sist_diaria",temper) ;
    publicar("help","presion_diast_diaria",temper) ;    
    publicar("help","temperatura_diaria",temper) ;*/
  }

 
}


char getGas()
{
  String IDGAS = "AKfycbyC5EeC2GMrgbfqBfslR3o4tPxg-WATkut75z2BVBPmAwsun1E" ; 
  char* host = "script.google.com" ; 
  int httpsPort = HTTPS_PORT ; 
  char *fingerprint = FINGERPRINT ;   
  String payload = ""  ; 
  String url = String("/macros/s/") +IDGAS + "/exec?user=esp8266" ; 
  int index = 0 ; 
  HTTPSRedirect*client = new HTTPSRedirect(httpsPort); 
  client->setInsecure() ;  
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
 // conexion -- espera 5 veces 
  bool flag = false;
  for (int i=0; i<5; i++){
    int retval = client->connect(host, httpsPort);
    
    if (retval == 1) {
       Serial.println("conectado a GAS") ; 
       flag = true;
       break;
    }
  }
  if (!flag){
      return 'n' ;    
  } 
  client->GET(url,host) ;  
  String bodyget = client->_myResponse.body;
  // parsear nombre,dato,idvol 
  index = bodyget.indexOf(',') ; 
  (bodyget.substring(0,index)).toCharArray(nombre_vol,20) ;   
  bodyget.substring(index+1 ,bodyget.indexOf(',',index+1)).toCharArray(tel,13) ;   
  index =  bodyget.indexOf(',',index+1) ;  
  bodyget.substring(index+1).toCharArray(idvol,3); 
  Serial.println("respuestapost: " + bodyget) ; 
  Serial.println("fin_setup") ;    

  delete client; 
  return 'o' ; 

  
}



 
