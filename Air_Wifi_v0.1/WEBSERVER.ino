
// --- Control_WEBSERVER --- //
void Control_WEBSERVER(){
  // -- Server Update -- //
  //MDNS.begin(AP_SSID);
  httpUpdater.setup(&server);
  //MDNS.addService("http", "tcp", LISTEN_PORT);
  Serial.println("HTTPUpdateServer preparado");

  // -- Control Webserver -- //
  server.on("/",[](){
    Capturar_Datos_WEBSERVER();
    server.send(200, "text/html", Pagina) ;
  });
  server.on("/ENCENDER",[](){
    Estado_Interruptor = true ;
    Capturar_Datos_WEBSERVER();
    server.send(200, "text/html", Pagina) ;
  });
  server.on("/APAGAR",[]() {
    Estado_Interruptor = false ;
    Capturar_Datos_WEBSERVER();
    server.send(200, "text/html", Pagina) ;
  });
  server.on("/CONSIGNA/MAS",[](){
    T_Consigna++ ;
    if(T_Consigna >= 30) T_Consigna = 30 ;
    Capturar_Datos_WEBSERVER();
    server.send(200, "text/html", Pagina) ;
  }); 
  server.on("/CONSIGNA/MENOS",[](){
    T_Consigna-- ;
    if(T_Consigna <= 15) T_Consigna = 15 ;
    Capturar_Datos_WEBSERVER();
    server.send(200, "text/html", Pagina) ;
  });
  server.on("/HORARIO/ON",[](){
    Horario = true ;
    Capturar_Datos_WEBSERVER();
    server.send(200, "text/html", Pagina) ;
  }); 
  server.on("/HORARIO/OFF",[](){
    Horario = false ;
    Capturar_Datos_WEBSERVER();
    server.send(200, "text/html", Pagina) ;
  }); 
  server.on("/CARGAR",[](){
    //Lectura_EEPRON();
    server.send(200, "text/html", Pagina) ;
  });
  server.on("/GUARDAR",[](){
    //Escritura_EEPRON();
    server.send(200, "text/html", Pagina) ;
  }); 
  server.on("/REINICIAR",[]() {server.send(200, "text/plain", "Reiniciando...");Reiniciar();});   
  server.on("/TEMPERATURA", []() {server.send(200, "text/plain",S_T_Real);});
  server.on("/online", []() {server.send(200, "text/plain","OK");});
  server.onNotFound([](){server.send(200, "text/html", Pagina);});
  
  // - Iniciar Webserver - //
  server.begin();
  Serial.println("WEBSERVER Iniciado");
}

// --- Generar WEBSERVER --- //
void Generar_WEB(){
// -- Conversiones A Caracteres -- //
char* C_Estado_Wifi = "NOK" ;
if (WIFI_OK) C_Estado_Wifi = "OK" ;
char* C_Control_Temperatura = "Deshabilitado" ;
if (Estado_Interruptor) C_Control_Temperatura = "Habilitado" ;
char* C_Modo = "Off" ;
if (Modo == 1) C_Modo = "Frio" ;
if (Modo == 2) C_Modo = "Secar" ;
if (Modo == 3) C_Modo = "Calor" ;
if (Modo == 4) C_Modo = "Vent" ;
char* C_Fan = "Auto" ;
if (Fan == 1) C_Fan = "Bajo" ;
if (Fan == 2) C_Fan = "Medio" ;
if (Fan == 3) C_Fan = "Alto" ;
char* C_Swing = "Off" ;
if (Swing == 1) C_Swing = "On" ;
char* C_Estado_Interruptor = "Desactivado" ;
if (Estado_Interruptor) C_Estado_Interruptor = "Activado" ;
char* C_Horario = "Deshabilitado" ;
if (Horario) C_Horario = "Habilitado" ;
byte longitud = IP.length() + 1 ;
char char_IP[longitud];
IP.toCharArray(char_IP,longitud);
S_T_Real = String(T_Real);

snprintf(Pagina,3000,
"<!doctype html>\
<html>\
<head>\
<meta charset='utf-8'>\
<title>AC</title>\
</head>\
  <h1 class='titulo_principal' style='background-color: antiquewhite' align='center'> WEBSERVER Air %01d </h1>\
<body bgcolor='#7F18A0'>\
<table width='1500' border='5' align='center' bgcolor='#A6A6A6'>\
  <tbody>\
    <tr>\
      <th>Variables</th>\
      <th>Estado</th>\
    </tr>\
    <tr>\
      <td align='center'>Estado del WIFI</td>\
      <td align='center'>%s</td>\
    </tr>\
    <tr>\
      <td align='center'>Hora de Dispositivo</td>\
      <td align='center'>%02d:%02d</td>\
    </tr>\
    <tr>\
      <td align='center'>Nobre de la Red</td>\
      <td align='center'>%s</td>\
    </tr>\
    <tr>\
      <td align='center'>IP</td>\
      <td align='center'>%s</td>\
    </tr>\
    <tr>\
      <td align='center'>Control de Temperatura</td>\
      <td align='center'>%s</td>\
    </tr>\
    <tr>\
      <td align='center'>Estado Interruptor</td>\
      <td align='center'>%s</td>\
    </tr>\
    <tr>\
      <td align='center'>Tem.Consigna: %02d ºC</td>\
      <td align='center'>Modo: %s</td>\
    </tr>\
    <tr>\
      <td align='center'>Vel.Ventilador: %s</td>\
      <td align='center'>Lamas: %s</td>\
    </tr>\
    <tr>\
      <td align='center'>Horario</td>\
      <td align='center'>%s</td>\
    </tr>\
    <tr>\
      <td align='center'>Hora Encendido</td>\
      <td align='center'>Hora Apagado</td>\
    </tr>\
    <tr>\
      <td align='center'> <form  name='frm1'  method='post'><input type='text' maxlength='5' size='5' value=%02d:%02d name='Encendido_1'></form></td>\
      <td align='center'> <form  name='frm2'  method='post'><input type='text' maxlength='5' size='5' value=%02d:%02d name='Apagado_1'></form></td>\
    </tr>\
    <tr>\
      <td align='center'> <form  name='frm3'  method='post'><input type='text' maxlength='5' size='5' value=%02d:%02d name='Encendido_2'></form></td>\
      <td align='center'> <form  name='frm4'  method='post'><input type='text' maxlength='5' size='5' value=%02d:%02d name='Apagado_2'></form></td>\
    </tr>\
    <tr>\
      <td align='center'> <form  name='frm5'  method='post'><input type='text' maxlength='5' size='5' value=%02d:%02d name='Encendido_3'></form></td>\
      <td align='center'> <form  name='frm6'  method='post'><input type='text' maxlength='5' size='5' value=%02d:%02d name='Apagado_3'></form></td>\
    </tr>\
    <tr>\
      <td align='center'> <button type='button' onClick=location.href='/ENCENDER'> ENCENDER </button></td>\
      <td align='center'> <button type='button' onClick=location.href='/APAGAR'> APAGAR </button></td>\
    </tr>\
    <tr>\
      <td align='center'> <button type='button' onClick=location.href='/CONSIGNA/MENOS'> BAJAR CONSIGNA </button></td>\
      <td align='center'> <button type='button' onClick=location.href='/CONSIGNA/MAS'> AUMENTAR CONSIGNA </button></td>\
    </tr>\
    <tr>\
      <td align='center'> <button type='button' onClick=location.href='/HORARIO/ON'> HABILITAR HORARIO </button></td>\
      <td align='center'> <button type='submit' onClick=location.href='/HORARIO/OFF'> DESHABILITAR HORARIO </button></td>\
    </tr>\   
    <tr>\
      <td align='center'> <button type='button' onClick=location.href='/CARGAR'> CARGAR </button></td>\
      <td align='center'> <button type='submit' onClick=location.href='/GUARDAR'> GUARDAR </button></td>\
    </tr>\  
  </tbody>\    
</table>\
</body>\
</html>",AIR,C_Estado_Wifi,Hora,Minuto,WIFI_SSID,char_IP,C_Control_Temperatura,C_Estado_Interruptor,T_Consigna,C_Modo,C_Fan,C_Swing,C_Horario,Hora_Encender_1[0],Hora_Encender_1[1],Hora_Apagar_1[0],Hora_Apagar_1[1],Hora_Encender_2[0],Hora_Encender_2[1],Hora_Apagar_2[0],Hora_Apagar_2[1],Hora_Encender_3[0],Hora_Encender_3[1],Hora_Apagar_3[0],Hora_Apagar_3[1]);
}



void Capturar_Datos_WEBSERVER(){ 
  String S_Encender = "00:00";
  char C_Encender[5] ;
    
  if(server.hasArg("Encendido_1") && (server.arg("Encendido_1").length()>0)){
    S_Encender = server.arg("Encendido_1");
    S_Encender.toCharArray(C_Encender, S_Encender.length() + 1);  
    if(isDigit(C_Encender[0]) and isDigit(C_Encender[1]) and isDigit(C_Encender[3]) and isDigit(C_Encender[4])){
      Hora_Encender_1[0] = (C_Encender[0] - 48)*10 + C_Encender[1] - 48 ;
      Hora_Encender_1[1] = (C_Encender[3]-  48)*10 + C_Encender[4] - 48 ;      
    }
  }
    if(server.hasArg("Encendido_2") && (server.arg("Encendido_2").length()>0)){
    S_Encender = server.arg("Encendido_2");  
    S_Encender.toCharArray(C_Encender, S_Encender.length() + 1);   
    if(isDigit(C_Encender[0]) and isDigit(C_Encender[1]) and isDigit(C_Encender[3]) and isDigit(C_Encender[4])){
      Hora_Encender_2[0] = (C_Encender[0] - 48)*10 + C_Encender[1] - 48 ;
      Hora_Encender_2[1] = (C_Encender[3]-  48)*10 + C_Encender[4] - 48 ;      
    }
  }
  if(server.hasArg("Encendido_3") && (server.arg("Encendido_3").length()>0)){
    S_Encender = server.arg("Encendido_3");
    S_Encender.toCharArray(C_Encender, S_Encender.length() + 1);  
    if(isDigit(C_Encender[0]) and isDigit(C_Encender[1]) and isDigit(C_Encender[3]) and isDigit(C_Encender[4])){
      Hora_Encender_3[0] = (C_Encender[0] - 48)*10 + C_Encender[1] - 48 ;
      Hora_Encender_3[1] = (C_Encender[3]-  48)*10 + C_Encender[4] - 48 ;      
    }
  }
   
  String S_Apagar = "00:00";
  char C_Apagar[5] ;  
  if(server.hasArg("Apagado_1") && (server.arg("Apagado_1").length()>0)){
    S_Apagar = server.arg("Apagado_1");
    S_Apagar.toCharArray(C_Apagar, S_Apagar.length() + 1);
    if(isDigit(C_Apagar[0]) and isDigit(C_Apagar[1]) and isDigit(C_Apagar[3]) and isDigit(C_Apagar[4])){
      Hora_Apagar_1[0] = (C_Apagar[0] - 48)*10 + C_Apagar[1] - 48 ;
      Hora_Apagar_1[1] = (C_Apagar[3]-  48)*10 + C_Apagar[4] - 48 ;
    }
  }
  if(server.hasArg("Apagado_2") && (server.arg("Apagado_2").length()>0)){
    S_Apagar = server.arg("Apagado_2");
    S_Apagar.toCharArray(C_Apagar, S_Apagar.length() + 1);
    if(isDigit(C_Apagar[0]) and isDigit(C_Apagar[1]) and isDigit(C_Apagar[3]) and isDigit(C_Apagar[4])){
      Hora_Apagar_2[0] = (C_Apagar[0] - 48)*10 + C_Apagar[1] - 48 ;
      Hora_Apagar_2[1] = (C_Apagar[3]-  48)*10 + C_Apagar[4] - 48 ;
    }
  }
  if(server.hasArg("Apagado_3") && (server.arg("Apagado_3").length()>0)){
    S_Apagar = server.arg("Apagado_3");
    S_Apagar.toCharArray(C_Apagar, S_Apagar.length() + 1);
    if(isDigit(C_Apagar[0]) and isDigit(C_Apagar[1]) and isDigit(C_Apagar[3]) and isDigit(C_Apagar[4])){
      Hora_Apagar_3[0] = (C_Apagar[0] - 48)*10 + C_Apagar[1] - 48 ;
      Hora_Apagar_3[1] = (C_Apagar[3]-  48)*10 + C_Apagar[4] - 48 ;
    }
  }
  
  // -- Limites Horarios -- //
  if(Hora_Encender_1[0] >= 24 || Hora_Encender_1[0] <= 0) Hora_Encender_1[0] = 0 ;
  if(Hora_Encender_1[1] >= 60 || Hora_Encender_1[1] <= 0) Hora_Encender_1[1] = 0 ;
  if(Hora_Encender_2[0] >= 24 || Hora_Encender_2[0] <= 0) Hora_Encender_2[0] = 0 ;
  if(Hora_Encender_2[1] >= 60 || Hora_Encender_2[1] <= 0) Hora_Encender_2[1] = 0 ;
  if(Hora_Encender_3[0] >= 24 || Hora_Encender_3[0] <= 0) Hora_Encender_3[0] = 0 ;
  if(Hora_Encender_3[1] >= 60 || Hora_Encender_3[1] <= 0) Hora_Encender_3[1] = 0 ;

  if(Hora_Apagar_1[0] >= 24 || Hora_Apagar_1[0] <= 0) Hora_Apagar_1[0] = 0 ;
  if(Hora_Apagar_1[1] >= 60 || Hora_Apagar_1[1] <= 0) Hora_Apagar_1[1] = 0 ;
  if(Hora_Apagar_2[0] >= 24 || Hora_Apagar_2[0] <= 0) Hora_Apagar_2[0] = 0 ;
  if(Hora_Apagar_2[1] >= 60 || Hora_Apagar_2[1] <= 0) Hora_Apagar_2[1] = 0 ;
  if(Hora_Apagar_3[0] >= 24 || Hora_Apagar_3[0] <= 0) Hora_Apagar_3[0] = 0 ;
  if(Hora_Apagar_3[1] >= 60 || Hora_Apagar_3[1] <= 0) Hora_Apagar_3[1] = 0 ;
  
  // --- Generar Pagina WEBSERVER --- //
  Generar_WEB();
}
