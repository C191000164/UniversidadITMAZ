# Motor controlado por voz con Arduino

## Descripción

Esta práctica consiste en controlar un LED, la matriz LED integrada y un motorreductor mediante comandos de voz enviados desde una aplicación de MIT App Inventor a un Arduino UNO R4 WiFi.

El programa recibe las órdenes por Wi-Fi y permite detener el motor o seleccionar tres niveles de velocidad mediante PWM.

## Objetivos

- Controlar dispositivos mediante comandos de voz.
- Comunicar una aplicación con Arduino por Wi-Fi.
- Encender y apagar un LED y la matriz LED.
- Programar tres niveles de velocidad para un motorreductor.
- Utilizar un puente H y señales PWM para controlar el motor.

## Herramientas y material utilizado

- Arduino UNO R4 WiFi y cable USB.
- Arduino IDE.
- MIT App Inventor y teléfono Android.
- Puente H L298N.
- Motorreductor con rueda.
- Batería de 9 V.
- LED externo.
- Cables Dupont.
- Conexión Wi-Fi.

## Diagrama

El diagrama muestra una representación de las conexiones del circuito. La imagen de Tinkercad utiliza Arduino UNO y L293D como referencia del montaje.

![Diagrama del circuito](Diagrama/Tinkercad_CPV.png)

![Armado del circuito](Diagrama/Armado%20del%20circuito.jpg)

[Ver carpeta Diagramas](Diagrama/)

## Código

El programa recibe las órdenes enviadas por la aplicación para controlar las luces y los niveles de velocidad del motor.

El control del motor utiliza PWM con valores de 0 (detenido), 110 (baja), 180 (media) y 255 (máxima).

[Ver código Arduino](Codigo/ControlPorVoz.ino)

[Ver archivos de App Inventor](Codigo/AppInventor/)

## Reporte

El reporte contiene el procedimiento de la práctica, las evidencias del circuito, las tablas de datos y las observaciones del sistema.

[Ver reporte](Reporte/Reporte_Practica_Control_por_Voz.pdf)

## Resultados

Durante las pruebas se confirmó el encendido del LED externo y de la matriz LED mediante los comandos enviados por Wi-Fi.

El Monitor Serie mostró la conexión del Arduino y la recepción de las órdenes de velocidad y paro. El movimiento físico del motor quedó pendiente de confirmar.

![Pruebas en el Monitor Serie](Diagrama/Monitor_Serial.png)

## Video

El video muestra la práctica de control por voz y el envío de órdenes desde la aplicación al Arduino.

[Ver video](https://youtu.be/HVi7jjqaD8g)

[Ver carpeta Video](Video/)

## Conclusiones

La práctica permitió comprender cómo comunicar una aplicación con Arduino para controlar luces y programar velocidades mediante PWM y un puente H.

Las pruebas confirmaron la recepción de comandos y el funcionamiento de las luces. Para comprobar el movimiento del motor, es necesario revisar sus conexiones y alimentación.
