#include <PCF8574.h>
#include <I2CKeyPad.h>
#include <LCD_I2C.h>

PCF8574 fila1(0x20);
PCF8574 keyPad(0x21);
LCD_I2C lcd(0x22);

#define numeroUbicaciones 8
#define buzzer 5
#define DSU1 10
#define ST 11
#define SH 12

int ubicacion[numeroUbicaciones][6]; //primera dimesion indica de que lugar se trata
/*segunda indica:
 * 0 clave del asiento
 * 1 color rojo (1) para si
 * 2 color verde prendido (1) para si
 * 3 indica si el asieto esta libre o ocupado (1) para si
 * 4 guarda si alguien se sento en el (1) para si
 * 5 control para el envio de si se pararon o si se sentaron (1) para sentado
 */
//enviarMensaje();
/*
 * byte 1 
 * {
 *  0 se ingreso la clave del asiento.
 *  1 se sentaron en el asiento
 *  2 se pararon del asiento
 * }
 * byte 2 (en caso de ser 0 el primer byte este es ignorado)
 * {
 *  0 ocupado N°
 *  1 sin asignar N°
 *  2 asignado N°
 * }
 * byte 3 y 4 = numero de asiento
*/
byte auxFun, auxFun2;//funciones usadas para los for en todo el programa
byte nuevaClave[6];//almacena en 0 la ubicacion y en las demas la clave

bool sensorAsientosActivados = false;//esto se activa con la interrupcion

void setup() 
{
  Serial.begin(9600);
//LCD.begin
  lcd.begin();
  lcd.backlight();
//-----------------------------------  
  lcd.setCursor(5, 0);
  lcd.print("Holaa"); 
  lcd.setCursor(5, 1); 
  lcd.print("Mundo!");
  delay(500);
  lcd.clear();
//-----------------------------------  
// 74HC595  
  pinMode(DSU1, OUTPUT);
  pinMode(ST, OUTPUT);
  pinMode(SH, OUTPUT);
// PCF.begin()
  keyPad.begin();
  fila1.begin();
//declara las interrupciones
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), interrupcionAsientos, FALLING);
//setea todos los valores a 0
  for(int auxFun = 0; auxFun < numeroUbicaciones; auxFun++){
    for(int auxFun2 = 0; auxFun2 < 6; auxFun2++)
  
      ubicacion[auxFun][auxFun2] = 0;}

}
//*************************************************************************
void loop() 
{ 
  int ubicacionRecibida;
  ubicacionRecibida = compararClave(claveFuncionIngresada());
  lcd.clear();
  lcd.noBlink();
  if(ubicacionRecibida != 404)
  {
    enviarMensaje('0', '0', ubicacionRecibida);//se ingreso clave
//------------------------------------
    lcd.setCursor(0, 0);
    lcd.print("INGRESE Y TOME");
    lcd.setCursor(0, 1);
    lcd.print("ASIENTO");
    delay(1000);
//------------------------------------
    controlAsientos(ubicacionRecibida);
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("CLAVE");
    lcd.setCursor(0, 1);
    lcd.print("INCORRECTA");
    delay(1000);
  }
}
//*************************************************************************
void controlAsientos(byte asientoVerde)
{
  uint16_t bytesLed;
  lcd.clear();//borra lo anterior puesto
//--------------------------------------------------------------------------
// setea el color verde
   ubicacion[asientoVerde][2] = 1;
// setea el color rojo
   for(auxFun = 0; auxFun < numeroUbicaciones; auxFun++)
   {
    if(!asientoVerde && !ubicacion[asientoVerde][3])
      ubicacion[auxFun][1] = 1;
   }
//--------------------------------------------------------------------------
  byte byteRojo = 255;
  bitSet (bytesLed, asientoVerde);
  bytesLed = bytesLed + ((bitClear(byteRojo, asientoVerde)) << 8); 
  controlLed(bytesLed);
//------------------------------------
  lcd.setCursor(0, 0);
  lcd.print("ESPERE UN");
  lcd.setCursor(0, 1);
  lcd.print("MOMENTO");
//------------------------------------
//--------------------------------------------------------------------------
// espera a que tome asiento, mientras tanto no deja pasar a nadie mas
   while(ubicacion[asientoVerde][2] != ubicacion[asientoVerde][4])
   {
    //la interrupcion lo activa y es para ver el valor actual de los botones y activar el buzzer
    if(sensorAsientosActivados){sensorAsientos();sensorAsientosActivados = 0;}
    delay(10);
   }
   for(auxFun = 0; auxFun < numeroUbicaciones; auxFun++){//setea todas las luces de vuelta a 0
    ubicacion[auxFun][2] = 0;
    ubicacion[auxFun][1] = 0;
   }
   ubicacion[asientoVerde][3] = 1; //indica que ahora este asiento esta ocupado
   controlLed(0);
//--------------------------------------------------------------------------
}
//*************************************************************************
void controlLed(uint16_t valor)
{
  byte valorByte1 = byte(valor);//saca los primeros 8bit
  byte valorByte2 = valor >> 8;//saca los ultimos 8bit
  
  digitalWrite(SH, LOW);
  shiftOut(DSU1, SH, MSBFIRST, valorByte1);
  shiftOut(DSU1, SH, MSBFIRST, valorByte2);
  digitalWrite(ST, HIGH);
  digitalWrite(ST, LOW);
}
//*************************************************************************
int compararClave(int claveFuncion)
{
//compara la clave recibida con cada clave almacenada  
  for(auxFun = 0; auxFun < numeroUbicaciones; auxFun++)
  {
    if(ubicacion[auxFun][0] == claveFuncion && !ubicacion[auxFun][3])
      return auxFun;
  }
  return 404;//si no encuentra ninguna regresa 404
}
//*************************************************************************
int claveFuncionIngresada()
{
//--------------------------------------------------------------------------  
  int teclaPresionada = 16, digitoInverso = 3;
  int claveFuncion = 1;
  int registroClave[4];
  boolean numActivado = 0;
//--------------------------------------------------------------------------
//introduce lo que deberia aparecer en el LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("INGRESE SU");
  lcd.setCursor(0, 1);
  lcd.print("NUMERO:....");
  lcd.setCursor(7, 1);
  lcd.blink();
//--------------------------------------------------------------------------  
  while(digitoInverso > -1)//mientras no tengamos los 4 digitos esto estara em blucle
  {
    while(teclaPresionada == 16)//espera a que se presione una tecla
    {
      teclaPresionada = numTecla();
      delay(10);
      if(sensorAsientosActivados){sensorAsientos();sensorAsientosActivados = 0;}//en caso de que se active el evento
      while(Serial.available())cambiarClave();//si llega un valor al buffer este lo toma
    }
//-----------------------------------------------
//  segun la tecla que se presione tomara distintos resultados
    switch(teclaPresionada)
    {
      case 0 ... 2:
        numActivado = 1;
        teclaPresionada += 1;
        break;
      case 4 ... 6:
        numActivado = 1;
        break;
      case 8 ... 10:
        numActivado = 1;
        teclaPresionada -= 1;
        break;
      case 13:
        numActivado = 1;
        teclaPresionada = 0;
        break;
      case 12:// reinciar
        digitoInverso = 3;
        claveFuncion = 1;

        lcd.setCursor(0, 1);
        lcd.print("NUMERO:....");
        lcd.setCursor(7, 1);
        break;
      case 14:// borrar
        if(digitoInverso < 3)
        {
          digitoInverso += 1;
          claveFuncion = registroClave[digitoInverso] ;

          lcd.setCursor(7+ (3 - digitoInverso), 1);
          lcd.print(".");
          lcd.setCursor(7+ (3 - digitoInverso), 1);
        }
        break;
    }
//-----------------------------------------------
//--------------------------------------------------------------------------    
    if(numActivado)// si es un numero sumarlo segun sus cifras
    {
      lcd.setCursor(7+ (3 - digitoInverso), 1);
      lcd.print(teclaPresionada);
      
      registroClave[digitoInverso] = claveFuncion;
      claveFuncion += (pow(10, digitoInverso) * teclaPresionada);
      digitoInverso--;
    }
//--------------------------------------------------------------------------    
    teclaPresionada = 16;//devuelve el while para esperar la proxima tecla
    numActivado = 0;//lo mismo con el if de si la tecla fue numero
    delay(200);
  }
  return claveFuncion;//devuelve la clave si ya se obtuvo los 4 digitos
//--------------------------------------------------------------------------
}
//*************************************************************************
byte numTecla()
{
  byte cont = 0;
  keyPad.write8(255);
  for(auxFun = 0; auxFun < 4; auxFun++)
  {
    keyPad.write(auxFun, 0);
    for(auxFun2 = 0; auxFun2 < 4; auxFun2++)
    {
      if(!keyPad.read(auxFun2 + 4))
        return cont;
      cont++;
    }
    keyPad.write(auxFun, 1);
  }
  return 16;
}
//*************************************************************************
//como no se puede usar nada relacionado con Wire dentro de una interrupcion
//esta solo activa otra funcion que se llamara de distinta forma
void interrupcionAsientos(){
  sensorAsientosActivados = true;}
void sensorAsientos()
{
//--------------------------------------------------------------------------
  for(auxFun = 0; auxFun < numeroUbicaciones; auxFun++)
    ubicacion[auxFun][4] = !fila1.read(auxFun);
  //como solo hay una fila lo hago una vez, en caso de ser mas esto se repite con cada fila
//--------------------------------------------------------------------------
  for(auxFun = 0; auxFun < numeroUbicaciones; auxFun++)//pasa por todas las ubicaciones
  {
    if(!ubicacion[auxFun][3] && ubicacion[auxFun][4] && !ubicacion[auxFun][2])
   {
      digitalWrite(buzzer, HIGH);
      auxFun = numeroUbicaciones;
    }
    else if (auxFun == (numeroUbicaciones - 1) && digitalRead(buzzer))//apaga el buzzer si ya salio del asiento
      digitalWrite(buzzer, LOW);
  }
//--------------------------------------------------------------------------
  for(auxFun = 0; auxFun < numeroUbicaciones; auxFun++)//pasa por todas las ubicaciones y envia los cambios
  {
    if(ubicacion[auxFun][3] && ubicacion[auxFun][4] && !ubicacion[auxFun][5]){
      enviarMensaje('1', '0', auxFun);//indica que se sentaron en el asiento ocupado
      ubicacion[auxFun][5] = 1;
    }
    else if(ubicacion[auxFun][3] && !ubicacion[auxFun][4] && ubicacion[auxFun][5]){
      enviarMensaje('2', '0', auxFun);//se pararon del asiento ocupado
      ubicacion[auxFun][5] = 0;
    }
    if(!ubicacion[auxFun][3] && ubicacion[auxFun][4] && !ubicacion[auxFun][2]){
      enviarMensaje('1', '1', auxFun);//inidica que se sentaron en el asiento sin asignar
      ubicacion[auxFun][5] = 1;
    }
    else if(!ubicacion[auxFun][3] && ubicacion[auxFun][4] && ubicacion[auxFun][5]){
      enviarMensaje('2', '1', auxFun);//se pararon del asiento sin asignar
      ubicacion[auxFun][5] = 0;
    }
    if(ubicacion[auxFun][4] && ubicacion[auxFun][2]){
      enviarMensaje('1', '2', auxFun);//inidica que se sentaron en el asiento asignado
      ubicacion[auxFun][5] = 1;
    }
  }
//--------------------------------------------------------------------------
}
//*************************************************************************
void enviarMensaje(char clave1, char clave2, byte ubicacionFuncion)
{
  byte decimal, unidad;
  Serial.print('_');
  Serial.print(clave1);
  Serial.print(clave2);
  ubicacionFuncion++;
  decimal = ubicacionFuncion / 10;
  unidad = ubicacionFuncion - (decimal * 10);
  Serial.print(char(decimal + 48));
  Serial.print(char(unidad + 48));
}
//*************************************************************************
void cambiarClave()//ya no se como explicarlo, pero funciona cuisde
{
  if(Serial.read() == '^')//si el mensaje recibido es una clave procede a llenar la nueva clave
  {
    Serial.readBytes(nuevaClave, 5); 
    ubicacion[int(nuevaClave[0])-48][0] = 1;
    ubicacion[int(nuevaClave[0])-48][3] = 0;
    for(auxFun = 0; auxFun < 4; auxFun++)
      ubicacion[int(nuevaClave[0])-48][0] += pow(10, 3 - auxFun)*(int(nuevaClave[auxFun+1])-48);
  }
  else{//si no borra el buffer
    while(Serial.available())
      auxFun = Serial.read();
  }
}
//*************************************************************************
