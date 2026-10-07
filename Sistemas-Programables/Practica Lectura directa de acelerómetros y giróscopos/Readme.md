# Lectura directa de acelerómetros y giróscopos con MPU-6050

## Descripción

Esta práctica consiste en utilizar un sensor **MPU-6050** con un Arduino UNO R4 WiFi para obtener datos del acelerómetro y del giroscopio mediante comunicación I2C.

A partir de estas mediciones, el programa calcula el ángulo de inclinación del sensor y utiliza ese valor para controlar un motorreductor mediante un puente H L298N. El sistema también muestra información del estado en el Monitor Serie y utiliza la matriz LED integrada del Arduino como indicador visual.

## Objetivos

- Comprender la lectura del acelerómetro y del giroscopio del MPU-6050.
- Utilizar comunicación I2C entre el sensor y el Arduino UNO R4 WiFi.
- Calcular el ángulo de inclinación a partir de las mediciones del sensor.
- Aplicar un filtro para obtener una lectura de inclinación más estable.
- Controlar la dirección y potencia de un motorreductor mediante un puente H.
- Implementar medidas de seguridad ante fallas de lectura o pérdida del sensor.
- Mostrar el estado del sistema mediante el Monitor Serie y la matriz LED.

## Herramientas y material utilizado

- Arduino UNO R4 WiFi.
- Sensor MPU-6050.
- Puente H L298N.
- Motorreductor.
- Protoboard.
- Cables de conexión.
- Alimentación para el motor.
- Cable USB.
- Arduino IDE.
- Matriz LED integrada del Arduino UNO R4 WiFi.

## Diagrama

El MPU-6050 se comunica con el Arduino mediante I2C utilizando **SDA = A4**, **SCL = A5** y la dirección **0x68**. El puente H controla el motor mediante **ENA = D9**, **IN1 = D8** e **IN2 = D7**.

Las evidencias se muestran de forma separada y con un tamaño reducido para mantener una presentación similar a un reporte.

### Diagrama de conexiones

<p align="center">
  <img src="Diagrama/Captura%20de%20pantalla%202026-10-06%20134638.png" alt="Diagrama de conexiones de la práctica" width="430">
</p>

### Montaje físico

<p align="center">
  <img src="Diagrama/WhatsApp%20Image%202026-10-06%20at%201.46.44%20PM.jpeg" alt="Montaje físico de la práctica" width="340">
</p>

<p align="center">
  <img src="Diagrama/WhatsApp%20Image%202026-10-06%20at%201.46.45%20PM.jpeg" alt="Segunda evidencia del montaje físico" width="340">
</p>

[Ver carpeta Diagramas](Diagrama/)

## Código

El programa configura el MPU-6050 con el acelerómetro en un rango de **±2 g** y el giroscopio en **±250 °/s**. El sensor trabaja a una frecuencia aproximada de 100 muestras por segundo.

Al iniciar, el Arduino comprueba que el MPU-6050 responda en la dirección `0x68` y realiza una calibración de 500 muestras con el sensor inmóvil. Después combina la inclinación obtenida del acelerómetro con la velocidad angular del giroscopio para calcular el ángulo `pitch`.

El control del motor utiliza una zona muerta de **5°**. Cuando la inclinación supera ese valor, el programa genera un PWM proporcional entre **90 y 255** y cambia el sentido del motor según la dirección de la inclinación. Para evitar cambios bruscos, la potencia se modifica mediante una rampa gradual.

También se incluye un sistema de seguridad: si falla la lectura del MPU-6050 o el intervalo de control es demasiado largo, el motor se detiene inmediatamente. El programa intenta recuperar la comunicación con el sensor antes de volver a habilitar el movimiento.

[Ver código](C%C3%B3digo/Inclinometro_Motor_R4.ino)

## Reporte

El reporte reúne el procedimiento de la práctica, las conexiones del MPU-6050 y del puente H, la lectura de los sensores, el control del motorreductor y las evidencias del funcionamiento del sistema.

[Ver reporte](Reporte/Reporte_Unidad_Medicion_Inercial_MPU6050.pdf)

## Resultados

Durante las pruebas, el Arduino pudo identificar y configurar el MPU-6050 mediante comunicación I2C. Antes de iniciar el control, el sistema realiza una calibración del giroscopio mientras el sensor permanece inmóvil, lo que ayuda a reducir el error acumulado en las mediciones.

Una vez activo, el programa obtiene continuamente los datos del acelerómetro y del giroscopio y calcula el ángulo de inclinación. El Monitor Serie informa si el sensor está centrado, inclinado hacia adelante o hacia atrás, además de mostrar el ángulo aproximado, la intensidad de la inclinación, el sentido del motor y el porcentaje de PWM aplicado.

Cuando la inclinación se mantiene cerca del centro, el motor permanece detenido. Al superar la zona muerta de 5°, el motor comienza a recibir potencia de manera proporcional a la inclinación. A mayor ángulo, mayor es el PWM aplicado hasta alcanzar el límite programado. Si cambia el sentido de la inclinación, el programa reduce primero la potencia hasta cero antes de invertir la dirección del motor.

La matriz LED integrada también funciona como indicador del estado. Cuando el sensor se encuentra centrado se muestra un marco; cuando existe inclinación se representa su posición en la matriz y, ante una falla o paro del sistema, aparece una figura en forma de X.

Además, el sistema incorpora un paro de seguridad ante errores de lectura del MPU-6050. En ese caso se apaga el motor y el programa intenta recuperar la comunicación antes de volver a activar el control. Esto permite que el funcionamiento sea más seguro ante una desconexión temporal o un problema en el bus I2C.

### Evidencia del funcionamiento

<p align="center">
  <img src="Diagrama/Captura%20de%20pantalla%202026-10-06%20192512.png" alt="Evidencia del funcionamiento de la práctica" width="400">
</p>

## Video

El video muestra el montaje del sistema, la lectura de la inclinación con el MPU-6050 y la respuesta del motorreductor de acuerdo con el movimiento del sensor.

[Ver video](https://youtu.be/lv86CP9bxlI?si=P1Oyk19WsESLipIz)

[Ver carpeta Video](Video/)

## Conclusiones

La práctica permitió comprender cómo un sensor MPU-6050 puede proporcionar información de aceleración y velocidad angular mediante comunicación I2C. Estas mediciones se utilizaron para calcular la inclinación del sensor y relacionar directamente el movimiento físico con la respuesta de un motorreductor.

También se observó la importancia de calibrar el giroscopio y combinar sus datos con los del acelerómetro para obtener una medición más estable. El uso de una zona muerta evita que pequeñas variaciones alrededor de la posición central provoquen movimientos innecesarios del motor.

El control mediante PWM permitió modificar la potencia del motor de acuerdo con el ángulo detectado, mientras que la rampa de aceleración redujo los cambios bruscos al aumentar la potencia o invertir el sentido. El Monitor Serie y la matriz LED facilitaron la observación del comportamiento del sistema durante las pruebas.

Finalmente, la práctica permitió aplicar medidas de seguridad dentro del programa. Ante una falla de comunicación con el sensor, el motor se detiene y el sistema intenta recuperar el MPU-6050 antes de continuar. En conjunto, se integraron lectura de sensores inerciales, procesamiento de datos, control de motor y manejo de errores dentro de un mismo sistema programable.
