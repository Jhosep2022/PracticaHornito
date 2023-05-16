  
  #include <Arduino.h>
  #include <WiFi.h>
  #include <ESPAsyncWebServer.h>
  #include "SPIFFS.h"
  #include <Arduino_JSON.h>
  #include "config.h"
  #define ADC_VREF_mV    3300.0 // 3.3v en millivoltios
  #define ADC_RESOLUTION 4096.0
  #define LIGHT_SENSOR_PIN       36 // ESP32 pin GIOP36 (ADC0) conectado al LDR
  int PinLedR = 5,PinLedG = 4,PinLedB = 0;                  
  int datoADC;
  float Porcentaje=0.0,Voltaje=0.0;
  float PorcentajeFactor=100.0/ADC_RESOLUTION;
  float VoltajeFactor = 3.3 / ADC_RESOLUTION;
  boolean flag_mode=true;
  int valADC;
  String ledState;
  String ipState="";
  String ssidState="";
  String macState="";
  String passState="";
  String rssiState="";
  
  
  const int rele=18;
  String releState;
  
  // Creamos el servidor AsyncWebServer en el puerto 80
  AsyncWebServer server(80);
  
  // Create an Event Source on /events
  AsyncEventSource events("/events");
  
  // Json Variable to Hold Sensor Readings
  JSONVar readings;
  
  // Timer variables
  unsigned long lastTime = 0;  
  unsigned long timerDelay = 1000; // actualizacion cada segundo//30000;
  
  
  // Get Sensor Readings and return JSON object
  String getSensorReadings(){
    datoADC = analogRead(LIGHT_SENSOR_PIN);
    Porcentaje=PorcentajeFactor*datoADC;
    Voltaje=VoltajeFactor*datoADC;
    readings["porcentaje"] = String(Porcentaje);
    readings["voltaje"] =  String(Voltaje);
    String jsonString = JSON.stringify(readings);
    return jsonString;
  }
  String processor(const String& var)
  {
    Serial.print(var+" LED: ");
      //esta función primero verifica si el marcador de posición es el ESTADO que hemos creado en el archivo HTML.
      if(var == "ESTADO")
      {   //Si lo está, entonces, de acuerdo con el estado del LED, ponemos la variable ledState en ON u OFF.
          if(digitalRead(rele))
              { releState = "ON"; }
          else{
             releState = "OFF";  }
         // Imprimimos el estado del led ( por el COM activo )
         //Finalmente, se devuelve la variable ledState. Esto reemplaza el marcador de posición STATE con el valor de cadena ledState.
         Serial.println(releState);
         return releState;
      }}
  
  String read_ADC() {
    valADC=analogRead(A0);
    Serial.print("Valor ADC=");
    Serial.println(valADC);
    return String(valADC);
  }
  
  // Initialize LittleFS
  void initFS() {
   // Iniciamos  SPIFFS
    if(!SPIFFS.begin())
       { Serial.println("ha ocurrido un error al montar SPIFFS");
         return; }
  }
  
  // Inicializando WiFi
  void initWiFi() {
  // conectamos al Wi-Fi
    WiFi.begin(ssid, password);
    // Mientras no se conecte, mantenemos un bucle con reintentos sucesivos
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        // Esperamos un segundo
        Serial.println("Conectando a la red WiFi..");
      }
    Serial.println();
    Serial.println(WiFi.SSID());
    Serial.print("Direccion IP:\t");
    // Imprimimos la ip que le ha dado nuestro router
    Serial.println(WiFi.localIP());
  }
  
  void setup() {
    Serial.begin(115200);
    pinMode(rele, OUTPUT);
  
    initWiFi();
    initFS();
  
    // Web Server Root URL
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/index.html", "text/html");
    });
  
    server.serveStatic("/", SPIFFS, "/");
    
    // Request for the latest sensor readings
    server.on("/readings", HTTP_GET, [](AsyncWebServerRequest *request){
      String json = getSensorReadings();
      request->send(200, "application/json", json);
      json = String();
    });
  
    events.onConnect([](AsyncEventSourceClient *client){
      if(client->lastId()){
        Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
      }
      // send event with message "hello!", id current millis
      // and set reconnect delay to 1 second
      client->send("hello!", NULL, millis(), 10000);
    });
    server.addHandler(&events);
  
   // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/index.html",String(), false, processor);
    });
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
              request->send(SPIFFS, "/style.css", "text/css");
              });       
     server.on("/ADC", HTTP_GET, [](AsyncWebServerRequest *request){
      if(flag_mode){request->send_P(200, "text/plain", read_ADC().c_str());}
    });
    server.on("/oncl", HTTP_GET, [](AsyncWebServerRequest *request){
              digitalWrite(rele, HIGH);
              request->send(SPIFFS, "/index.html", String(), false, processor);
              });
    
    // Ruta para poner el GPIO bajo
    server.on("/offcl", HTTP_GET, [](AsyncWebServerRequest *request){
              digitalWrite(rele, LOW);
              request->send(SPIFFS, "/index.html", String(), false, processor);
              });
  
    // Start server
    server.begin();
  }
  
  void loop() {
    if ((millis() - lastTime) > timerDelay) {
      // Send Events to the client with the Sensor Readings Every 30 seconds
      events.send("ping",NULL,millis());
      events.send(getSensorReadings().c_str(),"new_readings" ,millis());
      lastTime = millis();
    }
    datoADC = analogRead(LIGHT_SENSOR_PIN);
    Porcentaje=PorcentajeFactor*datoADC;
    Voltaje=VoltajeFactor*datoADC;
    Serial.print(" Dato ADC= "); Serial.print(datoADC);   
    Serial.print("  Porcentaje = ");  Serial.print(Porcentaje);  
    Serial.print("  Voltaje = ");  Serial.print(Voltaje);  
    if ((Voltaje >= 0) && (Voltaje <= 0.4)) 
      { Serial.print (" Hay Luz- "); } 
    else if ((Voltaje > 0.4) && (Voltaje <= 2))
       { Serial.print (" Muy Brillante - ");} 
    else {  Serial.print (" Esta Oscuro - ");    }
     Serial.println (" ");
    delay(500);
    
  }
