# Driver de software para un relevador

## Descripción

Esta práctica consiste en desarrollar un driver de software para controlar salidas digitales desde un Arduino UNO R4 WiFi. Para organizar el control se utiliza una clase llamada `Relevador`, que concentra las funciones necesarias para encender y apagar un actuador sin repetir la lógica en el programa principal.

El ejemplo controla un motor conectado mediante un puente H L298N y un LED. Cada salida utiliza una lógica diferente: el motor se activa con nivel alto y el LED está configurado para activarse con nivel bajo. El programa recibe órdenes desde el Monitor Serie y permite consultar el estado de ambos elementos.

## Objetivos

- Comprender cómo se puede organizar el control de una salida digital mediante una clase.
- Encender y apagar los actuadores desde el Monitor Serie.
- Manejar salidas con lógica activa en alto y activa en bajo.
- Evitar cambios demasiado seguidos mediante un intervalo mínimo entre conmutaciones.
- Mantener la lectura de comandos sin utilizar pausas bloqueantes con `delay()`.
- Consultar el estado de los actuadores y observar la respuesta del programa.

## Herramientas y material utilizado

- Arduino UNO R4 WiFi.
- Arduino IDE.
- Puente H L298N.
- Motorreductor.
- LED y resistencia.
- Protoboard y cables de conexión.
- Cable USB para programar la tarjeta y abrir el Monitor Serie.

## Diagrama

El montaje permite observar las salidas utilizadas por el programa y la conexión de los elementos controlados. El pin **D7** se utiliza para la señal de control del L298N y el pin **D6** para el LED. La lógica de encendido y apagado de cada elemento se define dentro de la clase `Relevador`.

### Diagrama de conexiones

<p align="center">
  <img src="Diagrama/Captura%20de%20pantalla%202026-10-20%20133405.png" alt="Diagrama de conexiones de la práctica de driver de software para un relevador" width="390">
</p>

### Montaje físico

<p align="center">
  <img src="Diagrama/WhatsApp%20Image%202026-10-20%20at%202.15.01%20PM.jpeg" alt="Montaje físico del circuito con Arduino y sus actuadores" width="300">
</p>

[Ver carpeta Diagrama](Diagrama/)

## Código

El programa se encuentra organizado en la clase `Relevador`. Cada objeto guarda el pin que controla, el tipo de lógica de activación, su estado actual y el momento del último cambio. De esta manera, las funciones `encender()` y `apagar()` pueden usarse para controlar distintos elementos con la misma estructura.

El objeto del motor utiliza el pin **D7** con lógica activa en alto, mientras que el LED utiliza el pin **D6** con lógica activa en bajo. Al iniciar, el programa establece primero el nivel de apagado para evitar una activación involuntaria y después configura el pin como salida.

Los comandos se escriben en el Monitor Serie a **9600 baudios** y se envían con nueva línea:

- `M1`: encender el motor.
- `M0`: apagar el motor.
- `L1`: encender el LED.
- `L0`: apagar el LED.
- `E`: mostrar el estado actual de ambos elementos.

Además, el código aplica un intervalo mínimo de **1000 milisegundos** entre cambios de estado. Si se intenta cambiar demasiado pronto, la orden se rechaza y el Monitor Serie indica cuánto tiempo falta. La lectura de comandos se realiza sin usar `delay()`, por lo que el programa puede seguir atendiendo la entrada serial.

[Ver código](C%C3%B3digo/Driver_software_relevador.ino)

## Reporte

El reporte contiene el procedimiento de la práctica, la explicación del driver de software, las conexiones utilizadas y las evidencias del funcionamiento. También permite consultar de manera ordenada la forma en que se reciben los comandos y se administra el estado de las salidas.

[Ver reporte](Reporte/Reporte_Practica_4_1_1-A.pdf)

## Resultados

Durante las pruebas, el Arduino UNO R4 WiFi inicia el programa y muestra en el Monitor Serie el mensaje **“Driver de relevador listo”**, junto con la lista de comandos disponibles. Esto permite comprobar que la comunicación serial está preparada para recibir instrucciones del usuario.

Al enviar `M1` o `M0`, el programa intenta encender o apagar la salida asociada al motor mediante el pin D7. De forma similar, los comandos `L1` y `L0` controlan el LED conectado al pin D6. La respuesta se muestra en el Monitor Serie para indicar si el elemento quedó encendido o apagado. Con el comando `E` se presenta un resumen del estado registrado para ambos actuadores.

También se contempla el intervalo mínimo entre conmutaciones. Cuando se envía una orden que intenta cambiar el estado antes de que transcurra el tiempo establecido, el programa no realiza el cambio y muestra un mensaje que indica cuántos milisegundos se deben esperar. Con esto se puede observar cómo el driver administra las solicitudes en lugar de cambiar la salida cada vez que recibe una orden.

Otro resultado importante es que la clase contempla la diferencia entre las salidas activas en alto y activas en bajo. El programa aplica el nivel eléctrico correspondiente según el elemento que controla, mientras que el resto del código utiliza las mismas funciones de encendido y apagado. Además, al no depender de `delay()`, la lectura de comandos permanece disponible durante la operación.

### Evidencia del funcionamiento en el Monitor Serie

<p align="center">
  <img src="Diagrama/Captura%20de%20pantalla%202026-10-20%20193330.png" alt="Evidencia de comandos y estados mostrados en el Monitor Serie" width="390">
</p>

## Video

El video muestra el montaje de la práctica y el funcionamiento del programa al enviar comandos para controlar las salidas y observar sus estados.

[Ver video de la práctica](https://youtube.com/shorts/CW2_RSHPhMA)

[Ver carpeta Video](Video/)

## Conclusiones

La práctica permitió comprender cómo se puede desarrollar un driver de software para organizar el control de diferentes salidas digitales desde un Arduino UNO R4 WiFi. Al utilizar la clase `Relevador`, las acciones de encendido y apagado quedan reunidas en un solo lugar, lo que hace que el programa principal sea más claro y facilita reutilizar la misma lógica con otros actuadores.

También se observó la importancia de considerar la forma en que cada elemento se activa eléctricamente. En este ejercicio, el motor conectado al L298N trabaja con lógica activa en alto, mientras que el LED se controla con lógica activa en bajo. La clase se encarga de convertir la orden solicitada en el nivel correcto, evitando que esa diferencia tenga que manejarse por separado en cada comando.

El intervalo mínimo entre cambios ayuda a rechazar órdenes demasiado seguidas y demuestra cómo se pueden incluir condiciones de control dentro del driver. Asimismo, establecer el estado de apagado durante el arranque contribuye a prevenir activaciones involuntarias. El uso de `millis()` y la lectura serial sin `delay()` permiten realizar estas comprobaciones sin detener la ejecución del programa.

En general, esta práctica reforzó el uso de clases, el control de pines digitales, la comunicación por el Monitor Serie y la administración de estados. Estos conceptos son útiles para crear programas más ordenados y ampliar el control a otros dispositivos en futuros proyectos con Arduino.
