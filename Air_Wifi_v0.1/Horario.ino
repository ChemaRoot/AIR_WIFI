
// --- Funcion_Horaria --- //
void Funcion_Horaria(){
  if(Horario){
    if (Hora_Encender_1[0] == Hora and Hora_Encender_1[1] == Minuto) Estado_Interruptor = true ;
    if (Hora_Apagar_1[0] == Hora and Hora_Apagar_1[1] == Minuto) Estado_Interruptor = false ;
    if (Hora_Encender_2[0] == Hora and Hora_Encender_2[1] == Minuto) Estado_Interruptor = true ;
    if (Hora_Apagar_2[0] == Hora and Hora_Apagar_2[1] == Minuto) Estado_Interruptor = false ;
    if (Hora_Encender_3[0] == Hora and Hora_Encender_3[1] == Minuto) Estado_Interruptor = true ;
    if (Hora_Apagar_3[0] == Hora and Hora_Apagar_3[1] == Minuto) Estado_Interruptor = false ;
  }
}
