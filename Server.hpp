
ESP8266WebServer server(80);

extern char nombre_vol[20] ;  
extern char tel[13] ;  
extern char idvol[3] ; 

// Función que se ejecutará en la URI '/'

void Raiz(){
   server.send(200, "text/html", "Hola! Esta es la página de inicio");
}

// Función que envía al cliente la web con los datos del voluntario más cercano

void EnviarDatosVoluntario(){
  String html = "<!DOCTYPE html>\
  <html>\
    <head>\
      <title>Datos Voluntario</title>\
      <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
    </head>\
    <body>\
      <h1>Voluntario más cercano</h1>\
      <ul>\
      <li>nombre: ";
  String telefono = "</li><li>telefono: ";
  String ID = "</li><li>ID: ";
  String final1 = "</li>\
      </ul>\
    </body>\
  </html>";
   String nombre = String(nombre_vol);
   String tel_vol = String(tel);
   String ID_vol = String(idvol);

   server.send(200, "text/html", html + nombre + telefono + tel_vol + ID + ID_vol + final1);
}
 
// Función que se ejecutará en URI desconocida

void PaginaNoExiste(){
   server.send(404, "text/html", "Página no encontrada");
}

void InitServer()
{
   // Ruteo para '/'
   
   server.on("/", Raiz);

   // Ruteo para petición GET de web html

   server.on("/DatosVoluntario", EnviarDatosVoluntario);

   // Ruteo para URI desconocida
   
   server.onNotFound(PaginaNoExiste);
 
   // Iniciar servidor
   
   server.begin();
   Serial.println("Servidor HTTP escuchando en el puerto 80");
 
   // Ruteo para '/inline' usando función lambda
   //server.on("/inline", [](){
   //   server.send(200, "text/plain", "Esto tambien funciona");
   //});
 
}
