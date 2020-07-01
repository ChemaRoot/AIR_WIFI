
///////////////////////////////////////////////////////////////
// --------------- PROGRAMA ESP8266 -------------------------//
// --------------- Emerson Wifi  ----------------------------//
// --------------- 23/04/2020 -------------------------------//
///////////////////////////////////////////////////////////////

// --- INICIALIZACION --- //
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>    
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Coolix.h>

#define WIFI_SSID           "XXXXXXXX" 
#define WIFI_PASSWORD       "XXXXXXXX"
#define LISTEN_PORT         80
#define AP_SSID             "AIR_1"                   // --- Colocar el numero de Air --- //
#define CLAVE_AP            "00001112"
#define AIR                 1                         // --- Colocar el numero de Air --- //
#define Velocidad           30
#define Velocidad_MQTT      3000 
#define mqtt_server         "192.168.1.200"
#define Out_Topic           "Status/AIR_1/POWER"      // --- Colocar el numero de Air --- //
#define In_Topic            "Command/AIR_1/POWER"     // --- Colocar el numero de Air --- //
#define Out_SP_Temp_Topic   "Status/AIR_1/SP_TEMP"    // --- Colocar el numero de Air --- //
#define In_SP_Temp_Topic    "Command/AIR_1/SP_TEMP"   // --- Colocar el numero de Air --- //
#define Out_MODE_Topic      "Status/AIR_1/MODE"       // --- Colocar el numero de Air --- //
#define In_MODE_Topic       "Command/AIR_1/MODE"      // --- Colocar el numero de Air --- //
#define Out_FAN_Topic       "Status/AIR_1/FAN"        // --- Colocar el numero de Air --- //
#define In_FAN_Topic        "Command/AIR_1/FAN"       // --- Colocar el numero de Air --- //
#define Out_SWING_Topic     "Status/AIR_1/SWING"      // --- Colocar el numero de Air --- //
#define In_SWING_Topic      "Command/AIR_1/SWING"     // --- Colocar el numero de Air --- //
#define Nombre_Dispositivo  "AIR PASILLO"             // --- Colocar el nombre de Air --- //
#define mqttUser            "XXXXXXXX"
#define mqttPassword        "XXXXXXXX"
#define IrLed               4                         // ESP8266 GPIO pin to use. Recommended: 4 (D2).
#define Debug               true

#define AUTO_MODE kCoolixAuto
#define COOL_MODE kCoolixCool
#define DRY_MODE kCoolixDry
#define HEAT_MODE kCoolixHeat
#define FAN_MODE kCoolixFan
#define FAN_AUTO kCoolixFanAuto
#define FAN_MIN kCoolixFanMin
#define FAN_MED kCoolixFanMed
#define FAN_HI kCoolixFanMax
#define SWING_ON kCoolixSwingV
#define SWING_OFF kCoolixSwing

ESP8266WebServer server(LISTEN_PORT);
ESP8266HTTPUpdateServer httpUpdater;

// --- Cliente MQTT --- //
WiFiClient espClient;
PubSubClient client(espClient);

// --- Objetos IR --- //
IRCoolixAC ac(IrLed);

// --- Inicializacion Servidor NTP --- //
IPAddress timeServerIP; // time.nist.gov NTP server address
const char* ntpServerName = "es.pool.ntp.org" ; // "time.nist.gov";
unsigned int localPort = 2390 ; // --- Puerto Local Recepcion Paquetes UDP
const int NTP_PACKET_SIZE = 48; // --- Total de Bytes en el paquete de recepcion NTP 
byte packetBuffer[ NTP_PACKET_SIZE]; // --- Buffer donde se guardan los paquetes 
WiFiUDP udp; // --- Instacncia de Nuevo UDP

// --- INICIALIZACION 2 --- //
bool LifeBit ;
bool Marca = false;
bool Marca_MQTT = false;
bool Cadena_Formada ;
bool Timeout = false ;
bool WIFI_OK = false ;
bool Startup = false ;
bool Estado_Interruptor = false ;
bool Estado_Interruptor_Old = false ;
bool Horario = false ;
bool Obtencion_Hora_OK = false ;
bool Pulsado = false ;
bool Primer_Ciclo = true ;
bool MQTT_INPUT = false; 
byte Hora = 0 ;
byte Hora_Old = 0 ;
byte Minuto = 0 ;
byte Minuto_Old = 0 ;
byte Segundo = 0 ;
byte Segundo_Old = 0 ;
byte Mes = 1 ;
byte Dia = 1 ;
byte Dia_Semana = 1 ;
byte Tiempo_Wifi = 0 ;
byte Tiempo_NTP = 0;
byte Intentos_NTP = 30 ; // --- Cambiar x 30 --- //
byte T_Consigna = 23 ;
byte T_Consigna_Old = 23 ;
byte Hora_Encender_1[]  = {0,0};
byte Hora_Apagar_1[]    = {0,0};
byte Hora_Encender_2[]  = {0,0};
byte Hora_Apagar_2[]    = {0,0};
byte Hora_Encender_3[]  = {0,0};
byte Hora_Apagar_3[]    = {0,0};
byte Modo = 0 ;   // --- 0-Apagado,1-Cool,2-Heat,3-Fan_Only --- //
byte Fan = 0 ;    // --- 0-Auto,1-Low,2-Medium,3-High --- //
byte Swing = 1 ;  // --- 0-Off,1-On --- //
byte Swing_Old = 1 ;  // --- 0-Off,1-On --- //
char Pagina[3000];
char msg[50];
int Anyo = 2000 ;
float T_Real = 0.0 ;
long Contador = 0 ;
long Contador_MQTT = 0 ;
long lastMsg = 0;
unsigned long highWord ;
unsigned long lowWord ; 
unsigned long Segundos_desde_1900 ;
unsigned long Tiempo_UNIX ;
String IP ;
String sEstado_Interruptor[15] = "Desconectado" ;
String S_T_Real = " " ;

const byte Zona_Horaria = 1 ; // --- Zona Horaria en la que se encuentra el sispositivo --- //
const unsigned long Setenta_Anos = 2208988800UL; // --- 70 anos en segundos son 2208988800

void setup() {
  ac.begin();
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
  Serial.println("Aire Acondicionado " + String(AIR));
  EEPROM.begin(512);
}

void loop() { 
  // --- Startup --- //
  if (Startup == false ){
    if (not WIFI_OK) Conexion_WIFI() ;
    else{
      Recogida_Hora() ;
      Init_MQTT();
    }
  }else{
    // --- ACTUALIZAR HORA --- //
    Actualizar_Hora();
    
    // --- Mantener Activo Weberver --- //
    server.handleClient();
     
    // ---MARCA DE CICLO --- // 
    if ((millis() - Velocidad) > Contador ){ 
      Contador = millis();
      Marca = true;
    }else Marca = false;

    // ---MARCA DE CICLO MQTT --- // 
    if ((millis() - Velocidad_MQTT) > Contador_MQTT ){ 
      Contador_MQTT = millis();
      Marca_MQTT = true;
    }else Marca_MQTT = false;

    // --- Marca Ciclo 1s --- //
    if(Segundo != Segundo_Old){
      Segundo_Old = Segundo ;
    }

    // --- Marca Ciclo 1m --- //
    if(Minuto != Minuto_Old and not Primer_Ciclo){
      Minuto_Old = Minuto ;
    }

    // --- Funcion_Horaria --- //
    Funcion_Horaria();


    // --- Limites --- //
    Limites();

    // --- Control de AC --- //
    Control_AC();

    // --- MQTT Loop -- /
    MQTT_Loop();
    
    // --- Primer Ciclo --- //
    if (Primer_Ciclo) {
      Hora_Old = Hora ;
      Minuto_Old = Minuto ; 
    }else if (Hora != Hora_Old) Hora_Old = Hora ;
    Primer_Ciclo = false ;
  }
  // --- Generar Pagina WEBSERVER --- //
  Generar_WEB() ;
}
