//------------------------------------
#include <SD.h>
#include <SPI.h>
File TXT;
//------------------------------------

byte mensajeCodificado[4] = {0, 0, 0, 0};
byte recepcionClave[6] = {0, 0, 0, 0, 0, 0};
char mensaje[45];
byte auxFun = 0;

void setup() {
//inicia el SD
  SD.begin(10);
//------------------------------------
 
//------------------------------------  
  Serial.begin(9600);
}
void loop() 
{

  delay(100);

}
void serialEvent()
{
  if (Serial.available() > 0)
  {
    char cual = Serial.read();
    if(cual == '_'){
      Serial.readBytes(mensajeCodificado, 4); 
      decodificarMensaje();
    }
    else if(cual == '`')
    {
      Serial.readBytes(recepcionClave, 5);
      cambiarClave();
    }
    else{
      while(Serial.available())
        auxFun = Serial.read();
    }
  }
}
void decodificarMensaje()
{
  switch(mensajeCodificado[0])
  {
    case '0':
      strcpy(mensaje, "Se ingreso la clave del asiento N ..");
      mensaje[strlen(mensaje)-2] = char(mensajeCodificado[2]);
      mensaje[strlen(mensaje)-1] = char(mensajeCodificado[3]);
      TXT = SD.open("Registro.txt", FILE_WRITE);
      TXT.println(mensaje);
      TXT.close();
      return;
      break;
    case '1':
      strcpy(mensaje, "Se sentaron en el asiento ");
      break;
    case '2':
      strcpy(mensaje, "Se pararon del asiento ");
      break;
  }
  switch(mensajeCodificado[1])
  {
    case '0':
      strcat(mensaje, "ocupado N ..");
      break;
    case '1':
      strcat(mensaje, "sin asignar N ..");
      break;
    case '2':
      strcat(mensaje, "asignado N ..");
      break;
  }
  mensaje[strlen(mensaje)-2] = char(mensajeCodificado[2]);
  mensaje[strlen(mensaje)-1] = char(mensajeCodificado[3]);

  TXT = SD.open("Registro.txt", FILE_WRITE);
  TXT.println(mensaje);
  TXT.close();
}
//*************************************************************************
void cambiarClave()//ya no se como explicarlo, pero funciona cuisde
{
  int claveFuncion = 1, ubicacionFuncion;
  ubicacionFuncion = int(recepcionClave[0]) - 48;
  for(auxFun = 0; auxFun < 4; auxFun++)
     claveFuncion += pow(10, 3 - auxFun)*(int(recepcionClave[auxFun+1])-48);
  
  Serial.print('^');
  Serial.print(ubicacionFuncion);
  Serial.print(claveFuncion);
}
//*************************************************************************
