# Control de LEDs con millis()

Control independiente de tres LEDs utilizando la función **`millis()`** en un **Arduino UNO R4 WiFi**, sin detener la ejecución del programa con `delay()`.

## Descripción

La práctica demuestra el uso de `millis()` para controlar diferentes tiempos dentro de un mismo programa. Se utilizan tres LEDs conectados al Arduino, cada uno con un intervalo diferente para cambiar de estado.

El LED verde cambia cada **500 ms**, el LED amarillo cada **1000 ms** y el LED rojo cada **1500 ms**. Como cada LED utiliza su propio registro de tiempo, los tres pueden funcionar de manera independiente dentro del mismo `loop()`.

Además, cada vez que el LED rojo cambia al estado encendido, el Arduino envía el mensaje **"Ximena la mas chambeadora"** al Monitor Serie configurado a **9600 baudios**.

## Objetivos de aprendizaje

Comprender el funcionamiento de la función `millis()`, controlar varias tareas con diferentes intervalos de tiempo y evitar el bloqueo del programa que puede producir el uso de `delay()`.

## Material utilizado

- Arduino UNO R4 WiFi.
- LED verde.
- LED amarillo.
- LED rojo.
- Resistencias limitadoras para los LEDs.
- Cables de conexión.
- Cable USB-C.
- Computadora con Arduino IDE.

## Diagrama del circuito

![Diagrama del circuito](diagrama/Captura%20de%20pantalla%202026-09-15%20210229.png)

Las conexiones definidas en el programa son:

| LED | Pin del Arduino | Intervalo |
|---|---:|---:|
| Verde | D9 | 500 ms |
| Amarillo | D10 | 1000 ms |
| Rojo | D11 | 1500 ms |

## Código

- [Programa de Arduino](codigo/millis.ino).

El programa guarda el tiempo anterior de cada LED en variables independientes y compara esos valores con `millis()`. Cuando se cumple el intervalo correspondiente, el estado del LED cambia entre encendido y apagado.

Esta lógica permite ejecutar las tres temporizaciones al mismo tiempo sin detener el programa.

## Video del funcionamiento

[Ver video en YouTube](https://youtu.be/gIHIuMy8Fag?si=nVLI3jlfM2oAtlDP)

## Evidencias de armado

![Evidencia del montaje 1](diagrama/WhatsApp%20Image%202026-09-15%20at%202.00.36%20PM.jpeg)

![Evidencia del montaje 2](diagrama/WhatsApp%20Image%202026-09-15%20at%202.00.37%20PM.jpeg)

![Evidencia del montaje 3](diagrama/WhatsApp%20Image%202026-09-15%20at%202.00.37%20PM%20(1).jpeg)

- [Diagrama de conexión](diagrama/Captura%20de%20pantalla%202026-09-15%20210229.png).
- [Fotografía del montaje 1](diagrama/WhatsApp%20Image%202026-09-15%20at%202.00.36%20PM.jpeg).
- [Fotografía del montaje 2](diagrama/WhatsApp%20Image%202026-09-15%20at%202.00.37%20PM.jpeg).
- [Fotografía del montaje 3](diagrama/WhatsApp%20Image%202026-09-15%20at%202.00.37%20PM%20(1).jpeg).

## Reporte

[Reporte de la práctica.pdf](reporte/Reporte_Practica_Millis_Arduino_R4_WiFi.pdf)

Incluye:

- Objetivo, materiales y procedimiento.
- Gráficas y tablas de datos.
- Evidencias del funcionamiento.
- Observaciones sobre el comportamiento del sistema.

## Conclusiones

La práctica permitió comprobar que `millis()` puede utilizarse para manejar diferentes tareas de manera independiente dentro de un mismo programa. A diferencia de `delay()`, esta función permite que el Arduino continúe ejecutando el resto del código mientras controla los tiempos de cada LED.

También se verificó el uso del Monitor Serie como forma de registrar un evento específico, enviando un mensaje cada vez que el LED rojo cambia al estado encendido.

## Resultados

Los tres LEDs cambiaron de estado de acuerdo con los intervalos programados. El LED verde presentó el cambio más frecuente, seguido del amarillo y finalmente el rojo.

| Elemento | Pin | Tiempo entre cambios | Acción |
|---|---:|---:|---|
| LED verde | D9 | 500 ms | Cambia entre encendido y apagado |
| LED amarillo | D10 | 1000 ms | Cambia entre encendido y apagado |
| LED rojo | D11 | 1500 ms | Cambia entre encendido y apagado |
| Monitor Serie | — | Al encender LED rojo | Muestra `Ximena la mas chambeadora` |

### Evidencia de resultados

![Resultados de la práctica](resultados/Captura%20de%20pantalla%202026-09-15%20204219.png)

- [Captura de resultados](resultados/Captura%20de%20pantalla%202026-09-15%20204219.png).
- [Resultados de la práctica.pdf](resultados/ResultadosPM.pdf).
