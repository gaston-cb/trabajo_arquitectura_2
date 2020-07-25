void onMessage(char* topic, byte* payload, unsigned int length)  ; 
const char * generateID()  ;
boolean reconnect()  ;
void initMQTT() ; 
void publicar(const char* datos) ; 
