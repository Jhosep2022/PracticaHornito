#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h> 
#include <Arduino_JSON.h>
#include <TimeLib.h>
#include <FS.h>
#include <ctime>
#include <NTPClient.h> //Nueva Biblioteca
#include "SPIFFS.h"
#include "config.h"

#define ADC_VREF_mV    3300.0 // 3.3v en millivoltios
#define ADC_RESOLUTION 4096.0
#define PIN_LM35       36 // ESP32 pin GIOP36 (ADC0) conectado al LM35
#define factor 0.0805860805860

String HOST_NAME = "http://192.168.31.218";
String PATH_NAME = "/insert_litle_oven.php";
//String server_url = "http://192.168.31.218/insert_litle_oven.php";

//Para la hora de internet
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

String hornoId="1";
String zona="habitacion";
String estadoHorno;
String calefactor;
String enfriador;
String setPoint;
String tempProceso;
String sensorLM35;

bool onn=true, off=false;
//Definimos los pines del led integrado y el rele
const int ledPin = 2;
const int rele = 18;
int off_count = 0;

bool movement = false;

//controla los modos
bool flag_mode_a = false, dato;
bool flag_mode_b = false;
bool flag_mode_c = false;
bool flag_mode_d = false;
bool flag_mode_e = false;

//Definimos los pines del LedRGB
const int PinLedR = 19;
const int PinLedG = 21;
const int PinLedB = 22;
const int pinLedVent1 = 16;
const int pinLedVent2 = 17;
// Para guardar el estado del LED
int estadoR = HIGH;
int estadoG = HIGH;
int estadoB = HIGH;
int estadoLV1 = HIGH;
int estadoLV2 = HIGH;
//Definimos la variable que va a recoger el estado del LED
String ledStateR;
String ledStateG;
String ledStateB;
String ledStateLV1;
String ledStateLV2;

//----------- Timer On - Off -----------

unsigned long onTimeSeconds = 0;
unsigned long offTimeSeconds = 0;
bool ledState = false;

////----------- End Timer On - Off -----------

float milliVolt,tempC,tempF;
int focoxd = 5;
int ventilador1 = 16;
int ventilador2 = 17;
int ledRojo = 22; 
int ledVerde = 21;
int ledAzul = 19;
int ledRojov = 14; 
int ledVerdev = 12;
int ledAzulv = 27;
int estadoFocoR = LOW;
int estadoFoco2 = LOW;
int estadoFoco3 = LOW;
String pwmValue2;
String estadoFoco;
String estadoVenti;

String pwmValue1;
String man;
String minu;
int freq = 2000;
int channel = 0;
int resolution = 8;
String s_rssi = "";
String s_ip = "";
int datoVal;
int page = 0;
// actualizacion cada segundo//30000;
int datoADC;
boolean flag_mode=true;
float temp=0.0;
String pwmValue;
int PinLed = 1;
int flag=0;
int fla=0;
int mode1=0;
int boton=0;
int valADC;

int band =0;
const int Foco1 = 6;
const int Vent1 = 5;
const int Vent2 = 4;
const int Sensor = 8;
JSONVar readings;

String getSensorReadings(){
  readings["temperature"] = String(read_ADC().toInt()/30);
  String jsonString = JSON.stringify(readings);
  return jsonString;
}

String read_ADC(){
  valADC = analogRead(PIN_LM35);
  milliVolt = valADC * (ADC_VREF_mV / ADC_RESOLUTION);
  tempC = valADC * factor;
  tempF = tempC * 9/5+32;
  Serial.print("Valor ADC =");
  Serial.println(valADC);
  setPoint = tempC/10;
  sensorLM35 = tempC/10;
  return String(tempC/10);
}
void read_ADC1() {
  valADC=analogRead(PIN_LM35);
  milliVolt = valADC * (ADC_VREF_mV / ADC_RESOLUTION);
  // Convirtiendo el voltaje al temperatura en °C
  tempC = valADC * factor ;
  // convirtiendo °C a °F
  tempF = tempC * 9 / 5 + 32;
  Serial.print("Valor ADC=");
  Serial.println(valADC);
  tempProceso=tempF;
}

String estado1[2] = {"<div class='mid' ></div>     <input type='image' src='http://esploradores.com/Practicas_html/apagar_300x88.png' style='display:block; margin: 200px 0px 500px 0px;' width='250px' heigth='30px' onClick=location.href='/LED1=ON'>",
                     " <div class='mid'style='background-color: #faa307;'></div>     <input type='image' src='http://esploradores.com/Practicas_html/encender_300x88.png' style='display:block; margin: 200px 0px 500px 0px;' width='250px' heigth='30px' onClick=location.href='/LED1=OFF'>"
                    };
String estado2[2] = {"<input type='image' src='https://ibb.co/HnpRSVX' style='display:block; margin: 500px 0px 500px 0px;' width='250px' heigth='30px' onClick=location.href='/LED2=ON'> <input type='checkbox'  id='engine' checked   /> <div> <span class='fly'></span> <span class='fly'></span> <span class='fly'></span><div class='fan__blades'> <input type='checkbox' id='engine2' checked/> <div class='fan__blades'> <div class='center'></div> <div class='blade'><span></span></div> <div class='blade'><span></span></div> <div class='blade'><span></span></div> <div class='blade'><span></span></div> </div> </div> </div>",
                     "<input type='image' src='https://ibb.co/JycZZck' style='display:block; margin: 500px 0px 500px 0px;' width='250px' heigth='30px' onClick=location.href='/LED2=OFF'> <input type='checkbox'  id='engine' checked   /> <div> <span class='fly'></span> <span class='fly'></span> <span class='fly'></span><div class='fan__blades'> <input type='checkbox' id='engine2' /> <div class='fan__blades'> <div class='center'></div> <div class='blade'><span></span></div> <div class='blade'><span></span></div> <div class='blade'><span></span></div> <div class='blade'><span></span></div> </div> </div> </div>"
                    };

AsyncWebServer server(80);
AsyncEventSource events("/events");


void initFS() {
 // Iniciamos  SPIFFS
  if(!SPIFFS.begin())
     { Serial.println("ha ocurrido un error al montar SPIFFS");
       return; }
}
String getPressure() {
    float rssi = WiFi.RSSI();
    Serial.println(rssi);
    return String(rssi);
}
String getIP() {
    Serial.println(WiFi.localIP());
    return String(WiFi.localIP());
}
String getHost() {
    Serial.println(WiFi.getHostname());
    return String(WiFi.getHostname());
}
String getSSID() {
    Serial.println(WiFi.SSID());
    return String(WiFi.SSID());
}
String getStat() {
    Serial.println(WiFi.status());
    return String(WiFi.status());
}

String getPSK() {
    return String(WiFi.psk());
}

String processor(const String& var){
  if(var == "ESTADO1"){
    if(estadoFocoR == HIGH){
      return estado1[1];
    }else{
      return estado1[0];
    }
  }else if(var == "IP"){
    return WiFi.localIP().toString();
  }else if(var == "HOSTNAME"){
    return String(WiFi.getHostname());
  }else if(var == "STATUS"){
    return String(WiFi.status());
  }else if(var == "PSK"){
    return String(WiFi.psk());
  }else if(var == "RSSI"){
    return String(WiFi.RSSI());
  }else if(var == "SSID"){
    return String(WiFi.SSID());
  }else if(var == "ESTADO2"){
    if(estadoFoco2 == HIGH){
      return estado2[1];
    }else{
      return estado2[0];
    }
  }
  return var;
}

void EstadoFoco(){
  if(onn == false){
    calefactor="0";
    enfriador="0";
    estadoHorno="false";
    onn=false;
    off=false;
    digitalWrite(ledAzul,LOW);
    digitalWrite(ledRojo,HIGH);
    digitalWrite(ledVerde,LOW);
    digitalWrite(focoxd, LOW);
    digitalWrite(ventilador1, LOW);
    digitalWrite(ventilador2, LOW);
    SolicitudHttp();
  }
}
void EstadoHorno(){
  if(off == true){
    calefactor="1";
    enfriador="1";
    estadoHorno="true";
    onn=true;
    off=true;
    digitalWrite(ledRojo,LOW);
    digitalWrite(ledAzul,HIGH);
    digitalWrite(ledVerde,HIGH);
    digitalWrite(focoxd, HIGH);
    digitalWrite(ventilador1, HIGH);
    digitalWrite(ventilador2, HIGH);
    SolicitudHttp();
  }
}

void EstadoVentilador1(){
  if(off){
    calefactor="1";
    enfriador="0";
    estadoHorno="true";
    onn=false;
    digitalWrite(ledAzul,LOW);
    digitalWrite(ledRojo,LOW);
    digitalWrite(ledVerde,HIGH);
    digitalWrite(focoxd, HIGH);
    digitalWrite(ventilador1, LOW);
    digitalWrite(ventilador2, LOW);
    SolicitudHttp();
  }
}




//Controla el tiempo en el que el horno esta prendido y dependiendo de esto va
//encendiendo los ventiladores con la espera de un segundo
void tiempo(int seg, int minu){
 digitalWrite(focoxd, HIGH);
 for(int i=0;i<(seg+(minu*60));i++){
  delay(1000); 
 }
 digitalWrite(focoxd, LOW);
 delay(1000);
 digitalWrite(ventilador1, HIGH);
 digitalWrite(ventilador2, HIGH);
 // espera la mitad de tiempo de espera donde sale el for
 delay(((seg+(minu*60))*1000)/2);
 digitalWrite(ventilador1, LOW);
 digitalWrite(ventilador2, LOW);
}

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
  
   s_ip = WiFi.localIP().toString().c_str();
}

void SolicitudHttp(){
  String queryString = "Horno_Id=" + hornoId;
  queryString = "&Zona" + zona;
  queryString = "&Estado_del_Horno" + estadoHorno;
  queryString = "&Calefactor=" + calefactor;
  queryString = "&Enfriador=" + enfriador;
  queryString = "&Set_Point=" + setPoint;
  queryString = "&TempProceso=" + tempProceso;
  queryString = "&SensorLM35=" + sensorLM35;
  Serial.print("Cadena Resultante = "); Serial.println(queryString);

  HTTPClient http;
  http.begin(HOST_NAME + PATH_NAME + queryString);
  int httpCode = http.GET();
  if(httpCode > 0){
    if(httpCode == HTTP_CODE_OK){
      String payload = http.getString();
      Serial.println(payload);
    }else{
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
  }else{
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

 void setup() {
  Serial.begin(115200);
  pinMode(ledRojo, OUTPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAzul, OUTPUT);
  pinMode(ledRojov, OUTPUT);
  pinMode(ledVerdev, OUTPUT);
  pinMode(ledAzulv, OUTPUT);
  pinMode(focoxd, OUTPUT);
  pinMode(ventilador1, OUTPUT);
  pinMode(ventilador2, OUTPUT);
  valADC = analogRead(PIN_LM35);
  ledcAttachPin(18, channel);
  initWiFi();
  initFS();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request){
    page = 0;
    request-> send(SPIFFS, "/index.html", String(), false, processor);
  });
  server.on("/focoxd.html", HTTP_GET, [](AsyncWebServerRequest * request){
    page = 1;
    request->send(SPIFFS,"/focoxd.html", String(), false, processor);
  });
  server.on("/slider1.html", HTTP_GET, [](AsyncWebServerRequest * request){
    page = 2;
    request->send(SPIFFS,"/slider1.html", String(), false, processor);
  });


  server.serveStatic("/", SPIFFS, "/");
    server.on("/LED1=ON", HTTP_GET, [](AsyncWebServerRequest * request){
      boton=1;
      request->redirect("/");
    });
    server.on("/LED1=OFF", HTTP_GET, [](AsyncWebServerRequest * request){
      boton=0;
      request->redirect("/");
    });
    server.serveStatic("/", SPIFFS, "/");
    server.on("/LED2=ON", HTTP_GET, [](AsyncWebServerRequest * request) {
      boton=1;
      request->redirect("/");
    });
    server.on("/LED2=OFF", HTTP_GET, [](AsyncWebServerRequest * request) {
      boton=0;
      request->redirect("/");
    });
    server.on("/readings", HTTP_GET, [](AsyncWebServerRequest *request){
        String json = getSensorReadings();
        request->send(200, "application/json", json);
        json = String();
     });

    server.on("/LED3=OFF", HTTP_GET, [](AsyncWebServerRequest * request) {
      pwmValue2 = request->arg("pwmValue2");
      Serial.println(pwmValue2); 
      Serial.println("UwU");
      
      request->redirect("/focoxd.html");
    });
    server.on("/counter", HTTP_POST, [](AsyncWebServerRequest *request){
            man = request->arg("man");
            minu = request->arg("min");
            //digitalWrite(PinLedR, pwmValue2.toInt());
            Serial.print("minutos:\t");
            Serial.println(minu);
            Serial.print("segundos:\t");
            Serial.println(man);
            fla=1;
            request->redirect("/focoxd.html");
     });
     server.on("/ON", HTTP_POST, [](AsyncWebServerRequest *request){
            request->redirect("/");
            digitalWrite(ledRojo, HIGH);
            boton=1;      
            });  
   server.on("/OF", HTTP_POST, [](AsyncWebServerRequest *request){
            delay(1000);
            request->redirect("/");
            digitalWrite(ledRojo, LOW); 
            boton=0;     
            });
  server.on("/V1N", HTTP_POST, [](AsyncWebServerRequest *request){
            delay(3000);
            request->redirect("/");
            digitalWrite(ledRojo, HIGH);   
            boton=1;   
            });  
   server.on("/V1F", HTTP_POST, [](AsyncWebServerRequest *request){
            delay(3000);
            request->redirect("/");
            digitalWrite(ledRojo, LOW);  
            boton=0;    
            }); 

  server.on("/SLIDERR", HTTP_POST, [](AsyncWebServerRequest *request){
            pwmValue1= request->arg("pwmValue1");
            band=1;
            Serial.print("setpoint:\t");
            Serial.println(pwmValue1);
            request->redirect("/");     
            });
  server.on("/SLIDER1", HTTP_POST, [](AsyncWebServerRequest *request){
            pwmValue = request->arg("pwmValue1");
            request->redirect("/");
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

  // Start server
  server.begin();
}


void loop() {
  if(fla==1){
    tiempo(man.toInt(),minu.toInt());
    fla=2;
  }

  
 if(band==1){
  Serial.print(read_ADC());
  if(read_ADC().toFloat()>pwmValue1.toFloat()){
    EstadoHorno();    
  }else{
    EstadoVentilador1();
  }
 }else{
  if(boton==0){
    EstadoHorno();
  }else{
    EstadoFoco();
  }
  delay(2000);
 }


}
