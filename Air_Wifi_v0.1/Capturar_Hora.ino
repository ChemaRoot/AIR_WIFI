
// --- Comunicacion y Recogida de Hora --- //
void Recogida_Hora(){
  WiFi.hostByName(ntpServerName, timeServerIP); // --- Recogida de Servidor Aleatorio
  sendNTPpacket(timeServerIP); // --- Envio de Paquete NTP ( Ver Funcion al final del Programa )
  delay(1000) ;
    
  // --- Probar Disponibilidad en Numeros Inpares de Peticion --- //
  int Paquete = udp.parsePacket();
      
  // --- Comprobar Disponibilidad del Paquete --- //
  if (!Paquete) {
    Tiempo_NTP ++ ;
    Serial.println("No hay Paquete NTP Todavia");
    Serial.print("Intento : ");
    Serial.println(Tiempo_NTP);
    if (Tiempo_NTP >= Intentos_NTP){
      Startup = true ;
      Obtencion_Hora_OK = false;
      //Reconectar()() ;
    }
  }
  else {
    Tiempo_NTP = 0 ;
    Serial.print("Paquete NTP Obtenido, Longitud = ");
    Serial.println(Paquete);
    
    Startup = true ;
    Obtencion_Hora_OK = true;
        
    udp.read(packetBuffer, NTP_PACKET_SIZE); // --- Lectura del Paquete

    // --- Combinacion de los 4 Bytes a partir del 40 en una DWORD --- //
    highWord = word(packetBuffer[40], packetBuffer[41]);
    lowWord = word(packetBuffer[42], packetBuffer[43]);
    Segundos_desde_1900 = highWord << 16 | lowWord; // --- Guardado de el tiempo en segundos NTP desde 01/01/1900 --- //
    Serial.print("Segundos desde 01/01/1900 = " );
    Serial.println(Segundos_desde_1900);

    // --- Conversion al Tiempo actual --- //
    // -- El Tiempo en UNIX comienza desde 01/01/1970 -- //
    Tiempo_UNIX = Segundos_desde_1900 - Setenta_Anos;
    Serial.print("Tiempo en UNIX = ");
    Serial.println(Tiempo_UNIX);

    // --- Impresion de Hora y Fecha Actual en Meridiano de Greenwich --- //
    // -- Calculos en funcion del timpo retante en segundos calculando los intervalos de tiempo incompletos -- //
    Hora = ((Tiempo_UNIX  % 86400L) / 3600) + Zona_Horaria ; // --- 1 Dia son 86400 segundos --- //
    Minuto = (Tiempo_UNIX  % 3600) / 60 ;
    Segundo = (Tiempo_UNIX  % 3600) / 60 ;
    Dia = (day(Tiempo_UNIX + Zona_Horaria * 3600)) ;
    Dia_Semana = (weekday(Tiempo_UNIX + Zona_Horaria * 3600)) ;
    Mes = (month(Tiempo_UNIX + Zona_Horaria * 3600)) ;
    Anyo = (year(Tiempo_UNIX + Zona_Horaria * 3600));
    Serial.print("La Hora en GMT "); 
    Serial.print(Zona_Horaria) ;
    Serial.print(" es:") ;
    Serial.print(Hora);
    Serial.print(":") ;
    Serial.print(Minuto) ;
    Serial.print(":") ;
    Serial.print(Segundo) ;
    Serial.println(' ');
    setTime(Hora,Minuto,Segundo,Dia,Mes,Anyo);
    Serial.println("Esperando Clientes...");
   }      
}

// --- Envio de Requerimiento al servidor NTP --- //
unsigned long sendNTPpacket(IPAddress& address){

  Serial.println("Enviando Paquete NTP...");
  memset(packetBuffer, 0, NTP_PACKET_SIZE); // --- Borrado de Buffer --- //
  // --- Inicializacion de los Valores necesarios para la peticion --- //
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

// --- Actualizar Hora --- //
void Actualizar_Hora(){
  if (Hora != hour()) Hora = hour() ;
  if (Minuto != minute()) Minuto = minute() ;
  if (Segundo != second()) Segundo = second() ;
  if (Dia != day()) Dia = day() ;
  if (Dia_Semana != weekday()) Dia_Semana = weekday() ;
  if (Mes != month()) Mes = month() ;
  if (Anyo != year()) Anyo = year() ;
}
