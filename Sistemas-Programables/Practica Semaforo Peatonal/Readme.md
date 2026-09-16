# Semáforo con cruce peatonal

Control de un semáforo vehicular con cruce peatonal mediante un **Arduino UNO R4 WiFi**, cinco LEDs y un botón para solicitar el paso del peatón.

## Descripción

La práctica simula el funcionamiento de un semáforo para vehículos y peatones. El semáforo vehicular cambia entre **verde, amarillo y rojo**, mientras que el semáforo peatonal permanece en rojo hasta que se registra una solicitud mediante el botón.

El programa utiliza `millis()` para controlar los tiempos sin detener la lectura del botón. Cuando el peatón presiona el botón durante el estado verde o amarillo de los vehículos, la solicitud queda guardada y el LED verde peatonal se enciende cuando el semáforo de los vehículos llega al estado rojo.

## Objetivos de aprendizaje

Programar una secuencia de semáforo con Arduino, controlar varios LEDs mediante salidas digitales, utilizar un botón con `INPUT_PULLUP`, implementar antirrebote y manejar tiempos mediante `millis()`.

## Material utilizado

- Arduino UNO R4 WiFi.
- 5 LEDs: 2 rojos, 2 verdes y 1 amarillo.
- Resistencias de **220 Ω**.
- Push button.
- Cables Dupont.
- Cable USB-C.
- Computadora con Arduino IDE.

## Diagrama del circuito

![Diagrama del circuito](diagrama/Captura%20de%20pantalla%202026-09-15%20205356.png)

Las conexiones utilizadas en el programa son:

| Elemento | Pin del Arduino |
|---|---:|
| LED rojo peatonal | D12 |
| LED verde peatonal | D11 |
| LED rojo vehicular | D10 |
| LED amarillo vehicular | D9 |
| LED verde vehicular | D8 |
| Push button | D2 |

El botón está configurado con `INPUT_PULLUP`, por lo que una pulsación se detecta cuando la entrada cambia a nivel **LOW**.

## Código

- [Programa de Arduino](codigo/Semaforo_Peatonal.ino).

El programa utiliza tres estados para el semáforo vehicular:

- **Verde:** 6 segundos.
- **Amarillo:** 2 segundos.
- **Rojo:** 6 segundos.

También se implementa un antirrebote de **40 ms** para evitar que una sola pulsación del botón sea detectada varias veces.

## Video del funcionamiento

[Ver video en YouTube](https://youtu.be/Fw5TZOuojWg?si=uEQRUvMrNa91d_S3)

## Evidencias de armado

![Armado del semáforo](diagrama/WhatsApp%20Image%202026-09-15%20at%201.16.33%20PM.jpeg)

![Prueba del cruce peatonal](diagrama/WhatsApp%20Image%202026-09-15%20at%201.16.33%20PM%20(1).jpeg)

- [Diagrama de conexión](diagrama/Captura%20de%20pantalla%202026-09-15%20205356.png).
- [Fotografía del montaje 1](diagrama/WhatsApp%20Image%202026-09-15%20at%201.16.33%20PM.jpeg).
- [Fotografía del montaje 2](diagrama/WhatsApp%20Image%202026-09-15%20at%201.16.33%20PM%20(1).jpeg).

## Reporte

[Reporte de la práctica.pdf](reporte/Reporte_Practica_Semaforo_Cruce_Peatonal.pdf)

Incluye:

- Objetivo, materiales y procedimiento.
- Resultados de la práctica.
- Tablas y observaciones del comportamiento del sistema.
- Evidencias del montaje y funcionamiento.

## Conclusiones

La práctica permite comprender cómo coordinar diferentes salidas digitales para representar un sistema de semáforo vehicular y peatonal. El uso de `millis()` permite que el Arduino siga leyendo el botón mientras controla los tiempos de los LEDs, evitando bloquear el programa con esperas largas.

También se aplica una lógica de solicitud peatonal: presionar el botón no cambia inmediatamente el semáforo, sino que guarda la petición y permite el cruce cuando los vehículos se encuentran en rojo.

## Resultados

La secuencia programada mantiene el paso vehicular en verde durante 6 segundos, cambia a amarillo durante 2 segundos y posteriormente mantiene el rojo durante 6 segundos.

| Estado vehicular | Duración | Estado peatonal |
|---|---:|---|
| Verde | 6 s | Rojo |
| Amarillo | 2 s | Rojo |
| Rojo sin solicitud | 6 s | Rojo |
| Rojo con solicitud | 6 s | Verde |

Cuando el botón se presiona durante el verde o amarillo vehicular, la solicitud queda almacenada. Al comenzar el rojo vehicular, el paso peatonal cambia a verde durante ese estado y después regresa a rojo cuando comienza nuevamente el verde para los vehículos.

- [Resultados de la práctica.pdf](resultados/ResultadosPSP.pdf).
