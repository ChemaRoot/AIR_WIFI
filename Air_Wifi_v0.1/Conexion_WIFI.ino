
// --- Conexion WIFI --- //
void Conexion_WIFI(){
  // -- IP Estatica -- //
  IPAddress wifiIp(192, 168, 1, 160 + AIR);
  IPAddress wifiNet(255, 255, 255, 0);
  IPAddress wifiGW(192, 168, 1, 1);
  IPAddress wifiDNS1(8, 8, 8, 8);
  IPAddress wifiDNS2(8, 8, 4, 4);
  WiFi.config(wifiIp,wifiDNS1,wifiGW,wifiNet); 

  // -- Conectando Wifi -- //
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  // Nombre Red Wifi y clave Deconexion server//
  Serial.print("Conectando...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    Tiempo_Wifi ++ ;
    if(Tiempo_Wifi >= 30) Startup == true ;
  }
  if(not Startup){
    Tiempo_Wifi = 0 ;
    WIFI_OK = true ;
    Serial.println();
    Serial.print("Conectado");
    IP = WiFi.localIP().toString() ;
    Serial.println(IP);
  }

  // -- Wifi AP -- //
  if (not WIFI_OK) WiFi.softAP(AP_SSID,CLAVE_AP); // Nombre Red Wifi y clave AP//
  else WiFi.softAPdisconnect (true);  // Para Wifi AP //  

  // --- Control Webserver --- //
  Control_WEBSERVER();

  // --- Conexion UDP --- //
  Serial.println("Iniciando UDP");
  udp.begin(localPort);
  Serial.print("Puerto Local: ");
  Serial.println(udp.localPort());
}
