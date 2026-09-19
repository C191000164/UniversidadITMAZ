# Buffer Circular con Arduino

## Descripción

Esta práctica consiste en registrar las pulsaciones de un botón mediante un buffer circular en un Arduino UNO R4 WiFi.

El programa almacena los eventos en un arreglo de 32 posiciones y los muestra en el Monitor Serie. Mientras se procesan, la matriz LED integrada mantiene una animación en movimiento.

## Objetivos

- Comprender el funcionamiento de un buffer circular.
- Registrar pulsaciones mediante interrupciones.
- Utilizar antirrebote para evitar registros repetidos.
- Almacenar y procesar eventos sin detener otras tareas.
- Mostrar los eventos en el Monitor Serie y una animación en la matriz LED.

## Herramientas y material utilizado

- Arduino UNO R4 WiFi.
- Arduino IDE.
- Push button.
- Cables Dupont.
- Cable USB-C.
- Matriz LED integrada del Arduino.

## Diagrama

El diagrama muestra la conexión del pulsador al pin D2 y a GND del Arduino.

![Diagrama de conexiones](diagrama/Diagrama-TinkercadBC.png)

![Armado del circuito](diagrama/ArmadoBC.jpeg)

[Ver carpeta Diagramas](diagrama/)

## Código

El programa registra las pulsaciones mediante una interrupción y guarda sus tiempos en un buffer circular de 32 posiciones.

Utiliza `micros()` para registrar los eventos, un antirrebote de 40 ms y `millis()` para actualizar la animación de la matriz LED cada 100 ms.

[Ver código](codigo/Buffer_Circular1.ino)

## Reporte

El reporte contiene el procedimiento de la práctica, los resultados, las evidencias del circuito y las observaciones sobre el comportamiento del sistema.

[Ver reporte](reporte/Reporte_Practica_Buffer_Circular.pdf)

## Resultados

Durante las pruebas, cada pulsación válida del botón generó un evento que el programa guardó temporalmente en el buffer circular. Después, el evento se procesó desde el `loop()` y apareció en el Monitor Serie con un número consecutivo y el tiempo de registro en microsegundos.

El buffer cuenta con 32 posiciones y utiliza índices de lectura y escritura que regresan al inicio al llegar al final del arreglo. De esta manera, el programa puede seguir almacenando nuevos eventos conforme se liberan posiciones.

Al mismo tiempo, la matriz LED integrada muestra una línea vertical en movimiento que se actualiza cada 100 ms. La animación continúa mientras el Arduino registra y procesa las pulsaciones, sin depender de una espera para cada evento.

El programa también aplica un antirrebote de 40 ms. Si el buffer está lleno, los nuevos eventos se contabilizan como rechazados, sin sobrescribir los que todavía están pendientes de procesar.

## Video

El video muestra el funcionamiento del pulsador, el registro de eventos en el buffer circular y la animación de la matriz LED.

[Ver video](https://youtu.be/Lg2vXyP2IQk)

[Ver carpeta Video](video/)

## Conclusiones

La práctica permitió comprender cómo funciona un buffer circular para almacenar temporalmente eventos producidos por un pulsador. Los índices de lectura y escritura permiten organizar el registro y el procesamiento de las pulsaciones dentro de un arreglo de tamaño fijo.

El uso de una interrupción facilita capturar los eventos cuando ocurren, mientras que el `loop()` los procesa y mantiene activa la animación de la matriz LED. El antirrebote ayuda a evitar registros repetidos de una misma pulsación.

En conjunto, la práctica muestra cómo separar la captura y el procesamiento de información para que el Arduino pueda realizar varias tareas sin detener el funcionamiento general del programa.

