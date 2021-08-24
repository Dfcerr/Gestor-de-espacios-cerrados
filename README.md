# Gestor-de-espacios-cerrados
Codigo de Gestor de espacios cerrados para las olimpiadas de electrónica 2021
Descripción de funcionamiento:

El operador de taquilla a través de un software elige el asiento para el cliente, la app le brinda un código numérico de cuatro dígitos y el número de asiento, que es entregado al cliente.

Luego el cliente se acerca a la puerta de la sala, y en una terminal ingresa el código, si coincide con otorgado, su asiento se ilumina en verde y el resto en rojo. Cuando se sienta, se deja continuar con el siguiente cliente.

El operador de boletería presiona un botón de la interfaz hecha con python que guarda el nro de botón (nro asiento), y genera un código de forma aleatoria, estos dos datos se los manda al primer Arduino por puerto serie (para la simulación usamos puertos virtuales), y este se lo envía al segundo Arduino a través del módulo RX TX de conexión inalámbrica, una vez recibido por este, guarda esos datos en un array. El segundo arduino al detectar el ingreso de numeros (usuario ingresando clave), empieza a comparar con todos los codigos guardados, si detecta una igualdad, envía al Datasheet registro de desplazamiento 74HC595 los datos para encender los leds rgb con sus correspondientes colores, ilumina el asiento asignado, el programa espera la señal del pulsador del asiento asignado y cuando es verdadero el programa se reinicia

# Ingresar al Master
https://github.com/Dfcerr/Gestor-de-espacios-cerrados/tree/master
