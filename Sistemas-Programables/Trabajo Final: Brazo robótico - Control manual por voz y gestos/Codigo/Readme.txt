Codigo de la practica:

brazo_voz_ble/brazo_voz_ble.ino -> controla los 5 servos del brazo con los 3 joysticks y con la app por Bluetooth LE
                                   (nombre del dispositivo: BrazoKUKA). Recibe ordenes de voz, flechas y botones,
                                   mantiene cada servo dentro de sus limites (hombro 55-125) y manda los angulos
                                   a la app y al Monitor Serie. Si la app deja de mandar mensajes, el brazo se
                                   detiene a los 400 ms.

   Joystick 1 -> hombro (A3) y base (A4)
   Joystick 2 -> codo (A1), muneca (A2) y boton HOME (A0)
   Joystick 3 -> pinza (A5)

AppInventor/Brazo_KUKA_Voz7.aia -> proyecto de MIT App Inventor. Se importa con Projects -> Import project (.aia).
AppInventor/Diseno_App_Brazo.png -> pantalla de la app y arbol de componentes.
AppInventor/Brazo_Bloques1.png  -> conexion Bluetooth LE y envio de comandos.
AppInventor/Brazo_Bloques2.png  -> voz continua (varias ordenes seguidas).
AppInventor/Brazo_Bloques3.png  -> flechas (mantener presionado para mover).
AppInventor/Brazo_Bloques4.png  -> palabras de voz, garra y posicion inicial.

Librerias: "ArduinoBLE", "Servo" y "EEPROM". Placa: Arduino UNO R4 WiFi. Monitor Serie a 115200 baudios.
La app necesita la extension BluetoothLE de MIT App Inventor.

Nota: al encender, no toquen los joysticks, porque el programa calibra su centro. Los servos se conectan uno por
uno (400 ms entre cada uno) para evitar un pico de corriente en la fuente.
