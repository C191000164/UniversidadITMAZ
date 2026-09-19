# Semáforo Vehicular y Peatonal con Arduino

## Descripción

Esta práctica consiste en controlar un semáforo vehicular y peatonal mediante un Arduino UNO R4 WiFi, cinco LEDs y un botón para solicitar el cruce.

El semáforo cambia de verde a amarillo y rojo. La solicitud del peatón se guarda y se atiende cuando los vehículos tienen la luz roja. Los tiempos se controlan con `millis()`, sin detener la lectura del botón.

## Objetivos

- Comprender el funcionamiento de un semáforo vehicular y peatonal.
- Controlar cinco LEDs mediante salidas digitales.
- Utilizar un botón para solicitar el cruce peatonal.
- Programar los tiempos del semáforo con `millis()`.
- Evitar lecturas repetidas del botón mediante antirrebote.
- Permitir el cruce peatonal cuando el semáforo vehicular está en rojo.

## Herramientas y material utilizado

- Arduino UNO R4 WiFi.
- Arduino IDE.
- Protoboard.
- 2 LEDs rojos, 2 verdes y 1 amarillo.
- Resistencias de 220 Ω.
- Push button.
- Cables Dupont.
- Cable USB-C.

## Diagrama

El diagrama muestra las conexiones del Arduino con los LEDs y el botón utilizados en la práctica.

![Diagrama de conexiones](Diagrama/Diagrama-TinkercadSP.png)

![Armado del circuito](Diagrama/ArmadoSP.jpeg)

[Ver carpeta Diagramas](Diagrama/)

## Código

El programa controla la secuencia de luces del semáforo y guarda las solicitudes de cruce realizadas con el botón.

Utiliza `millis()` para controlar los tiempos: verde durante 6 segundos, amarillo durante 2 segundos y rojo durante 6 segundos.

[Ver código](Codigo/Semaforo_Peatonal.ino)

## Reporte

El reporte contiene el procedimiento de la práctica, las evidencias del circuito, los resultados y las observaciones sobre el funcionamiento del sistema.

[Ver reporte](Reporte/Reporte_Practica_Semaforo_Cruce_Peatonal.pdf)

## Resultados

Durante las pruebas, el semáforo vehicular siguió la secuencia programada: luz verde durante 6 segundos, amarilla durante 2 segundos y roja durante 6 segundos. La temporización se realiza con `millis()`, por lo que el Arduino puede seguir leyendo el botón mientras cambian las luces.

Al presionar el botón durante el verde o amarillo vehicular, la solicitud de cruce queda guardada. Cuando los vehículos llegan al estado rojo, se enciende el LED verde peatonal para permitir el paso. Al terminar ese estado, el peatón vuelve a tener luz roja y comienza un nuevo ciclo vehicular.

Cuando no existe una solicitud de cruce, la luz peatonal permanece en rojo y el semáforo continúa su secuencia normal. El programa también incorpora un antirrebote de 40 ms para evitar que una sola pulsación se registre varias veces.

Estos resultados muestran cómo se coordinan las luces vehiculares y peatonales mediante el código y el circuito montado en Arduino.

## Video

El video muestra el funcionamiento del semáforo vehicular y peatonal, incluyendo la solicitud de cruce mediante el botón.

[Ver video](https://youtu.be/Fw5TZOuojWg?si=uEQRUvMrNa91d_S3)

[Ver carpeta Video](Video/)

## Conclusiones

La práctica permitió aplicar la programación de una secuencia de estados al control de un semáforo vehicular y peatonal. Mediante las salidas digitales del Arduino se coordinan las luces para representar el paso de vehículos y la solicitud de cruce.

El uso de `millis()` permite controlar las duraciones de cada luz sin interrumpir la lectura del botón. Además, el antirrebote ayuda a registrar correctamente una pulsación y la solicitud se conserva hasta que el semáforo vehicular llega al rojo.

En conjunto, la práctica relaciona el programa con el funcionamiento del circuito y muestra la importancia de atender el cruce peatonal en el momento adecuado.

