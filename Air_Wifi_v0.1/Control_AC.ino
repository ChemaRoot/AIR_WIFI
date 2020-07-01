
// --- Control de Temperatura --- //
void Control_AC(){
  if(MQTT_INPUT) {
    if(Estado_Interruptor){
      if(Estado_Interruptor!= Estado_Interruptor_Old){
        ac.on();
        Estado_Interruptor_Old= Estado_Interruptor ;
      }

      if(T_Consigna!= T_Consigna_Old){
        ac.setTemp(T_Consigna);
        T_Consigna_Old= T_Consigna;
      }     
      
      // -- Control Modo --- /
      if (Modo == 0){
        ac.setMode(AUTO_MODE);
        ac.setFan(FAN_AUTO);   
      }
      else if (Modo  == 1) ac.setMode(COOL_MODE);
      else if (Modo  == 2) ac.setMode(DRY_MODE);
      else if (Modo  == 3) ac.setMode(HEAT_MODE);
      else if (Modo  == 4) ac.setMode(FAN_MODE);

      // -- Control Fan --- //
      if (Fan == 0) ac.setFan(FAN_AUTO);
      else if (Fan == 1) ac.setFan(FAN_MIN);
      else if (Fan == 2) ac.setFan(FAN_MED);
      else if (Fan == 3) ac.setFan(FAN_HI);

      // -- Control Swing --- //
      if (Swing != Swing_Old){
        ac.setSwing();
        Swing_Old = Swing;
      }
      
    }else ac.off();  
    ac.send();
    MQTT_INPUT = false;    
  }  
}
