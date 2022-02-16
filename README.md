# Gestor-de-espacios-cerrados
Codigo de Gestor de espacios cerrados para las olimpiadas de electrónica 2021
Descripción de funcionamiento:

El operador de taquilla a través de un software elige el asiento para el cliente, la app le brinda un código numérico de cuatro dígitos y el número de asiento, que es entregado al cliente.

Luego el cliente se acerca a la puerta de la sala, y en una terminal ingresa el código, si coincide con otorgado, su asiento se ilumina en verde y el resto en rojo. Cuando se sienta, se deja continuar con el siguiente cliente.

El operador de boletería presiona un botón de la interfaz hecha con python que guarda el nro de botón (nro asiento), y genera un código de forma aleatoria, estos dos datos se los manda al primer Arduino por puerto serie (para la simulación usamos puertos virtuales), y este se lo envía al segundo Arduino a través del módulo RX TX de conexión inalámbrica, una vez recibido por este, guarda esos datos en un array. El segundo arduino al detectar el ingreso de numeros (usuario ingresando clave), empieza a comparar con todos los codigos guardados, si detecta una igualdad, envía al Datasheet registro de desplazamiento 74HC595 los datos para encender los leds rgb con sus correspondientes colores, ilumina el asiento asignado, el programa espera la señal del pulsador del asiento asignado y cuando es verdadero el programa se reinicia
# Como simular
Para probar el proyecto se puede igresar directamente desde el archivo proteus (asegurarse de que los arduinos tengan puestos correctamete su HEX en caso contrario para el arduino SIM2 usar el HEX de la carpeta "Ardino_SIM2" y para el arduino SIM1 usar el HEX de la carpeta "Ardino_SIM1"). primero instalar el simulador de puerto serial que se encontrara en la carpeta con el mismo nombre y activar el puerto COM2 y COM3, despues de eso abrir el arduinoSelector.exe y seguir los pasos del video
En caso de querer volver a exportar el codigo sera necesario incluir las librerias dentro de la carpeta "LIBRERIAS ARDUINO"
# Lectura de tarjeta SD
Para leer la tarjeta SD sera necesario instalar "wima64100" el cual esat en la carpeta "Instalador para lectura de SD", luego de eso con la aplicacion abrir "SD.IMA" el cual es una imagen de una SD donde se encontrara un archivo de texto con la informacion recopilada
# Proteus
El archivo de proteus (.pdsprj) contiene dos hojas, para ver la segunda ir a DESIGN——> Root sheet 2
![ad990d75-7e4f-4cca-b23b-60b7ddb02546](https://user-images.githubusercontent.com/83226453/154174010-e75f638f-b949-4ef0-8492-3be6c6d27267.jpg)
# Ingresar al Master
https://github.com/Dfcerr/Gestor-de-espacios-cerrados/tree/master
