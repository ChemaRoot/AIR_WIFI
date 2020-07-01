
// --- Init_MQTT --- //
void Init_MQTT(){
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial.println("MQTT Inicializado");
}

void callback(char* topic, byte* payload, unsigned int length) {
  MQTT_INPUT = true;
  Serial.print("MQTT in: [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) Serial.print((char)payload[i]);
  Serial.println();

  String S_Topic = String(topic);

  // --- Control Interruptor --- //
  if(S_Topic.indexOf("POWER") > 0){
    if ((char)payload[0] == 'O' and (char)payload[1] == 'N')Estado_Interruptor = true;
    else if((char)payload[0] == 'O' and (char)payload[1] == 'F' and (char)payload[2] == 'F')Estado_Interruptor = false;
  }

  // --- Consigna --- //
  if(S_Topic.indexOf("SP") > 0) T_Consigna = (payload[0] - 48)* 10 + (payload[1] - 48) ;

  // --- Control Modo --- //
  if(S_Topic.indexOf("MODE") > 0){
    if((char)payload[0] == 'a' and (char)payload[1] == 'u' and (char)payload[2] == 't' and (char)payload[3] == 'o')Modo = 0;
    else if((char)payload[0] == 'c' and (char)payload[1] == 'o' and (char)payload[2] == 'o' and (char)payload[3] == 'l')Modo = 1;
    else if((char)payload[0] == 'd' and (char)payload[1] == 'r' and (char)payload[2] == 'y')Modo = 2;
    else if((char)payload[0] == 'h' and (char)payload[1] == 'e' and (char)payload[2] == 'a' and (char)payload[3] == 't')Modo = 3;
    else if((char)payload[0] == 'f' and (char)payload[1] == 'a' and (char)payload[2] == 'n' and (char)payload[3] == '_')Modo = 4;
  }

  // --- Control Fan --- //
  if(S_Topic.indexOf("FAN") > 0){
    if((char)payload[0] == 'a' and (char)payload[1] == 'u' and (char)payload[2] == 't' and (char)payload[3] == 'o')Fan = 0;
    else if((char)payload[0] == 'l' and (char)payload[1] == 'o' and (char)payload[2] == 'w')Fan = 1;
    else if((char)payload[0] == 'm' and (char)payload[1] == 'e' and (char)payload[2] == 'd' and (char)payload[3] == 'i')Fan = 2;
    else if((char)payload[0] == 'h' and (char)payload[1] == 'i' and (char)payload[2] == 'g' and (char)payload[3] == 'h')Fan = 3;
  }
  
  // --- Control Fan --- //
  if(S_Topic.indexOf("SWING") > 0){
    if((char)payload[0] == 'o' and (char)payload[1] == 'n')Swing = 1;
    else if((char)payload[0] == 'o' and (char)payload[1] == 'f' and (char)payload[2] == 'f')Swing = 0;
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando Conexion MQTT ...");
    // Attempt to connect
    if (client.connect(Nombre_Dispositivo, mqttUser, mqttPassword )) {
      Serial.println("connected");
      client.publish(Out_Topic, Nombre_Dispositivo);
      client.subscribe(In_Topic);
      client.subscribe(In_SP_Temp_Topic);
      client.subscribe(In_MODE_Topic);
      client.subscribe(In_FAN_Topic);
      client.subscribe(In_SWING_Topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" Reconectando en 5 segundos");
      delay(5000);
    }
  }
}

// --- MQTT LOOP --- //
void MQTT_Loop(){
  // -- Control Conexion -- //
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // -- Publicar Mensajes -- //
  if(Marca_MQTT){
    String string_msg;
    char char_msg[20];

    // - Estado Interruptor - //
    if(Estado_Interruptor) snprintf (msg, 75, "ON");
    else snprintf (msg, 75, "OFF");
    if(Debug){
      Serial.print("MQTT Send: [");
      Serial.print(Out_Topic);
      Serial.print("] ") ;
      Serial.println(msg);
    }  
    client.publish(Out_Topic, msg);

    // - Temperatura Consigna - //
    string_msg = String(T_Consigna);
    string_msg.toCharArray(char_msg,20);
    snprintf (msg, 75, char_msg);
    if(Debug){
      Serial.print("MQTT Send: [");
      Serial.print(Out_SP_Temp_Topic);
      Serial.print("] ") ;
      Serial.println(msg);
    }  
    client.publish(Out_SP_Temp_Topic, msg);

    // - Mode - //
    if (Estado_Interruptor){
      if(Modo == 0) string_msg = "auto" ;
      else if(Modo == 1) string_msg = "cool" ;
      else if(Modo == 2) string_msg = "dry" ;
      else if(Modo == 3) string_msg = "heat" ;
      else if(Modo == 4) string_msg = "fan_only" ;
    } else string_msg = "off" ;
    string_msg.toCharArray(char_msg,20);
    snprintf (msg, 75, char_msg);
    if(Debug){
      Serial.print("MQTT Send: [");
      Serial.print(Out_MODE_Topic);
      Serial.print("] ") ;
      Serial.println(msg);
    }  
    client.publish(Out_MODE_Topic, msg);

    // - Fan - //
    if(Fan == 1) string_msg = "low" ;
    else if(Fan == 2) string_msg = "medium" ;
    else if(Fan == 3) string_msg = "high" ;
    else string_msg = "auto" ;
    string_msg.toCharArray(char_msg,20);
    snprintf (msg, 75, char_msg);
    if(Debug){
      Serial.print("MQTT Send: [");
      Serial.print(Out_FAN_Topic);
      Serial.print("] ") ;
      Serial.println(msg);
    }  
    client.publish(Out_FAN_Topic, msg);

    // - Swing - //
    if(Swing == 1) string_msg = "on" ;
    else string_msg = "off" ;
    string_msg.toCharArray(char_msg,20);
    snprintf (msg, 75, char_msg);
    if(Debug){
      Serial.print("MQTT Send: [");
      Serial.print(Out_SWING_Topic);
      Serial.print("] ") ;
      Serial.println(msg);
    }  
    client.publish(Out_SWING_Topic, msg);
  }
}
