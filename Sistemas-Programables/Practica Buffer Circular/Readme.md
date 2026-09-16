# Buffer circular con Arduino

Registro y procesamiento de eventos de un pulsador mediante un **buffer circular** en un **Arduino UNO R4 WiFi**, utilizando interrupciones, antirrebote y la matriz LED integrada.

## Descripción

La práctica demuestra cómo almacenar temporalmente eventos dentro de un **buffer circular**. Cada vez que se presiona el pulsador conectado al pin **D2**, una interrupción registra el instante del evento en microsegundos y lo guarda en un arreglo de tamaño fijo.

El buffer tiene capacidad para **32 eventos**. Cuando el índice llega al final del arreglo, vuelve al inicio para continuar trabajando de forma circular. Los eventos almacenados son procesados posteriormente desde el `loop()` y se muestran en el Monitor Serie.

Al mismo tiempo, la matriz LED integrada del Arduino muestra una línea vertical en movimiento, demostrando que el programa puede continuar realizando otras tareas mientras recibe y procesa las pulsaciones.

## Objetivos de aprendizaje

Comprender el funcionamiento de un buffer circular, registrar eventos mediante interrupciones, aplicar antirrebote a un pulsador y separar la captura de eventos de su procesamiento dentro del programa principal.

## Material utilizado

- Arduino UNO R4 WiFi.
- Push button.
- Cables Dupont.
- Cable USB-C.
- Computadora con Arduino IDE.
- Matriz LED integrada del Arduino UNO R4 WiFi.

## Diagrama del circuito

![Diagrama del circuito](diagrama/Captura%20de%20pantalla%202026-09-16%20123158.png)

El pulsador se conecta entre el pin **D2** y **GND**. El programa configura la entrada con `INPUT_PULLUP`, por lo que no necesita una resistencia pull-up externa.

| Elemento | Conexión | Función |
|---|---|---|
| Push button | D2 y GND | Generar eventos mediante una interrupción |
| Matriz LED | Integrada en el UNO R4 WiFi | Mostrar una animación mientras se procesan eventos |
| Monitor Serie | USB a 9600 baudios | Mostrar los eventos registrados |

## Código

- [Programa de Arduino](codigo/Buffer_Circular1.ino).

El programa utiliza un arreglo de **32 posiciones** para almacenar los tiempos de las pulsaciones. Los índices de lectura y escritura avanzan de forma independiente y regresan a la primera posición cuando llegan al final del buffer.

La función `attachInterrupt()` detecta la pulsación mediante un flanco **FALLING**. Cada evento válido se registra con `micros()` y se aplica un antirrebote de **40 ms**.

El programa también cuenta los eventos que no pueden almacenarse cuando el buffer está lleno y los reporta por el Monitor Serie.

## Video del funcionamiento

[Ver video en YouTube](https://youtu.be/Lg2vXyP2IQk)

## Evidencias de armado

![Armado del circuito](diagrama/WhatsApp%20Image%202026-09-16%20at%2012.22.08%20PM.jpeg)

![Prueba del buffer circular](diagrama/WhatsApp%20Image%202026-09-16%20at%2012.22.08%20PM%20(1).jpeg)

- [Diagrama de conexión](diagrama/Captura%20de%20pantalla%202026-09-16%20123158.png).
- [Fotografía del montaje 1](diagrama/WhatsApp%20Image%202026-09-16%20at%2012.22.08%20PM.jpeg).
- [Fotografía del montaje 2](diagrama/WhatsApp%20Image%202026-09-16%20at%2012.22.08%20PM%20(1).jpeg).

## Reporte

[Reporte de la práctica.pdf](reporte/Reporte_Practica_Buffer_Circular.pdf)

Incluye:

- Objetivo, materiales y procedimiento.
- Resultados de la práctica.
- Tablas y observaciones del comportamiento del sistema.
- Evidencias del montaje y funcionamiento.

## Conclusiones

La práctica permitió comprender cómo un buffer circular puede almacenar eventos de forma temporal sin detener el funcionamiento general del programa. Las pulsaciones se registran rápidamente mediante una interrupción y después se procesan desde el `loop()`.

También se comprobó que el Arduino puede continuar ejecutando una animación en la matriz LED mientras recibe eventos del pulsador. Esto permite observar la utilidad de los buffers para organizar información que llega de manera asíncrona.

## Resultados

Cada pulsación válida genera un evento que se almacena en el buffer y posteriormente aparece en el Monitor Serie con un número consecutivo y el tiempo en microsegundos en que fue registrado.

| Parámetro | Valor |
|---|---:|
| Tamaño del buffer | 32 eventos |
| Pin del pulsador | D2 |
| Tipo de interrupción | FALLING |
| Antirrebote | 40 ms |
| Intervalo de animación | 100 ms |
| Monitor Serie | 9600 baudios |

Cuando existen eventos pendientes, el programa extrae uno por cada vuelta del `loop()`. Si el buffer llega a llenarse, los nuevos eventos no sobrescriben los anteriores y se contabilizan como eventos rechazados.

- [Resultados de la práctica.pdf](resultados/ResultadosBC.pdf).
