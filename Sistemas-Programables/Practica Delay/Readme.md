# Secuencia de LEDs con delay()

Control secuencial de tres LEDs utilizando la función **`delay()`** en un **Arduino UNO R4 WiFi**.

## Descripción

La práctica demuestra el funcionamiento de `delay()` para controlar una secuencia de encendido de LEDs. Se utilizan tres LEDs conectados al Arduino y cada uno permanece encendido durante un tiempo diferente antes de apagarse y dar paso al siguiente.

De acuerdo con el programa cargado en el repositorio, el LED verde se enciende durante **500 ms**, el LED amarillo durante **1000 ms** y el LED rojo durante **1500 ms**. Al finalizar la secuencia, el `loop()` vuelve a comenzar.

## Objetivos de aprendizaje

Comprender el funcionamiento de la función `delay()`, controlar salidas digitales mediante Arduino y observar cómo una espera bloqueante ejecuta las instrucciones de forma secuencial.

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

| LED | Pin del Arduino | Tiempo encendido |
|---|---:|---:|
| Verde | D9 | 500 ms |
| Amarillo | D10 | 1000 ms |
| Rojo | D11 | 1500 ms |

## Código

- [Programa de Arduino](codigo/Delay.ino).

El programa enciende un LED, espera el tiempo definido mediante `delay()`, lo apaga y continúa con el siguiente. Por esta razón, las acciones se realizan una después de otra.

## Video del funcionamiento

[Ver video en YouTube](https://youtu.be/zXMxCz9uoMA?si=Up6rGS999ETC38SV)

## Evidencias de armado

![Evidencia del montaje 1](diagrama/WhatsApp%20Image%202026-09-15%20at%202.00.36%20PM.jpeg)

![Evidencia del montaje 2](diagrama/WhatsApp%20Image%202026-09-15%20at%202.00.37%20PM.jpeg)

![Evidencia del montaje 3](diagrama/WhatsApp%20Image%202026-09-15%20at%202.00.37%20PM%20(1).jpeg)

- [Diagrama de conexión](diagrama/Captura%20de%20pantalla%202026-09-15%20210229.png).
- [Fotografía del montaje 1](diagrama/WhatsApp%20Image%202026-09-15%20at%202.00.36%20PM.jpeg).
- [Fotografía del montaje 2](diagrama/WhatsApp%20Image%202026-09-15%20at%202.00.37%20PM.jpeg).
- [Fotografía del montaje 3](diagrama/WhatsApp%20Image%202026-09-15%20at%202.00.37%20PM%20(1).jpeg).

## Reporte

[Reporte de la práctica.pdf](reporte/Reporte_Practica_LED_Delay.pdf)

Incluye:

- Objetivo, materiales y procedimiento.
- Resultados de la práctica.
- Tablas de datos.
- Observaciones sobre el comportamiento del sistema.

## Conclusiones

La práctica permitió observar de manera sencilla cómo funciona `delay()` en Arduino. Cada llamada a esta función mantiene detenido el flujo normal del programa durante el tiempo especificado, por lo que los LEDs se controlan de forma secuencial.

Esto permite comprender la diferencia entre una temporización bloqueante con `delay()` y otras técnicas de temporización que permiten ejecutar varias tareas sin detener el programa.

## Resultados

La secuencia programada encendió los tres LEDs uno después de otro. Cada LED permaneció activo durante el tiempo definido en el código y después se apagó antes de continuar con el siguiente.

| Orden | LED | Pin | Duración |
|---:|---|---:|---:|
| 1 | Verde | D9 | 500 ms |
| 2 | Amarillo | D10 | 1000 ms |
| 3 | Rojo | D11 | 1500 ms |

El ciclo completo de esperas programadas suma **3000 ms (3 segundos)** antes de iniciar nuevamente la secuencia.

- [Resultados de la práctica.pdf](resultados/ResultadosPD.pdf).
