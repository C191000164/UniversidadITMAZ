# Sensor ambiental de precisión con BMP280

## Descripción

Esta práctica consiste en utilizar un sensor BMP280 conectado a un Arduino UNO R4 WiFi mediante comunicación I2C para obtener mediciones de temperatura, presión atmosférica y altitud aproximada.

El programa detecta automáticamente el sensor en las direcciones I2C `0x76` o `0x77`, realiza lecturas periódicas y muestra los datos en el Monitor Serie. Además, utiliza la matriz LED integrada del Arduino para alternar la visualización de la temperatura y la altitud.

## Objetivos

- Comprender el funcionamiento del sensor ambiental BMP280.
- Utilizar comunicación I2C entre el sensor y el Arduino UNO R4 WiFi.
- Obtener mediciones de temperatura, presión y altitud.
- Mostrar las lecturas en el Monitor Serie.
- Utilizar la matriz LED integrada para mostrar información del sensor.
- Detectar errores de lectura o desconexión del dispositivo.

## Herramientas y material utilizado

- Arduino UNO R4 WiFi.
- Sensor BMP280.
- Arduino IDE.
- Cables de conexión.
- Cable USB.
- Matriz LED integrada del Arduino UNO R4 WiFi.
- Librerías `Wire`, `Adafruit_Sensor`, `Adafruit_BMP280`, `ArduinoGraphics` y `Arduino_LED_Matrix`.

## Diagrama

Las siguientes imágenes muestran el montaje y las evidencias utilizadas durante la práctica. Se presentan de forma separada y con un tamaño reducido para conservar una apariencia similar a un reporte.

### Montaje principal

<p align="center">
  <img src="Diagrama/WhatsApp%20Image%202026-10-05%20at%201.17.46%20PM.jpeg" alt="Montaje principal del sensor ambiental" width="400">
</p>

### Evidencias del funcionamiento

<p align="center">
  <img src="Diagrama/WhatsApp%20Image%202026-10-05%20at%208.32.49%20PM.jpeg" alt="Evidencia de funcionamiento del sensor ambiental" width="340">
</p>

<p align="center">
  <img src="Diagrama/WhatsApp%20Image%202026-10-05%20at%208.32.49%20PM%20(1).jpeg" alt="Segunda evidencia de funcionamiento del sensor ambiental" width="340">
</p>

[Ver carpeta Diagramas](Diagrama/)

## Código

El programa inicia la comunicación I2C y busca un BMP280 en las direcciones `0x76` y `0x77`. También comprueba el identificador del sensor para evitar confundirlo con un BME280.

Una vez detectado, el Arduino obtiene la temperatura en grados Celsius, la presión en hPa y una altitud aproximada calculada a partir de una presión de referencia a nivel del mar de `1005 hPa`.

Las lecturas se realizan cada segundo y se envían al Monitor Serie a `115200` baudios. La matriz LED cambia cada 2.5 segundos entre la temperatura y la altitud. Si el sensor se desconecta o devuelve valores inválidos, la matriz muestra `ERR` y el programa intenta detectar nuevamente el dispositivo cada 5 segundos.

[Ver código](C%C3%B3digo/Sensorambiental.ino/Sensorambiental.ino.ino)

## Reporte

El reporte reúne el procedimiento de la práctica, la configuración del sensor BMP280, las conexiones utilizadas, las pruebas de lectura y las evidencias del funcionamiento del sistema.

[Ver reporte](Reporte/Reporte_Sensor_Ambiental_BMP280.pdf)

## Resultados

Durante las pruebas, el Arduino UNO R4 WiFi pudo comunicarse con el sensor BMP280 mediante el bus I2C. Al iniciar el programa, se realiza una búsqueda en las direcciones `0x76` y `0x77` y, cuando el dispositivo es identificado correctamente, el Monitor Serie confirma que el sensor está listo para realizar mediciones.

El sistema obtiene tres datos principales: temperatura en grados Celsius, presión atmosférica en hectopascales y una estimación de altitud en metros. Estas lecturas se actualizan aproximadamente cada segundo y se muestran en una sola línea en el Monitor Serie, lo que también permite utilizarlas en el Serial Plotter.

La matriz LED integrada del Arduino muestra de forma alternada la temperatura y la altitud. Cada dato permanece visible durante aproximadamente 2.5 segundos antes de cambiar al siguiente, permitiendo consultar información básica sin depender únicamente de la computadora.

El programa también incluye una comprobación de lecturas inválidas. Si la presión se encuentra fuera del rango esperado, aparece un error de lectura, se muestra `ERR` en la matriz y el sistema intenta volver a establecer comunicación con el sensor cada 5 segundos. Esto permite que la práctica continúe funcionando de forma más estable ante una desconexión temporal o un problema de cableado.

En conjunto, las pruebas permiten observar cómo un único sensor puede proporcionar distintas variables ambientales y cómo esas mediciones pueden procesarse y mostrarse simultáneamente mediante el Monitor Serie y la matriz LED del Arduino.

## Video

El video muestra el montaje de la práctica y el funcionamiento del sensor BMP280 mientras el Arduino obtiene y presenta las mediciones ambientales.

[Ver video](https://youtu.be/ox5LbVQqwLA?si=oZzK3-JBsN90VUjZ)

[Ver carpeta Video](Video/)

## Conclusiones

La práctica permitió comprender cómo utilizar un sensor BMP280 con un Arduino UNO R4 WiFi para medir diferentes variables ambientales mediante comunicación I2C. A través de un mismo dispositivo fue posible obtener temperatura, presión atmosférica y una estimación de altitud, relacionando las conexiones físicas con la lectura de datos desde el programa.

También se comprobó la utilidad de trabajar con tareas controladas mediante `millis()`, ya que las lecturas del sensor, la actualización de la matriz LED y los intentos de reconexión se realizan en diferentes intervalos sin depender de pausas largas que detengan todo el programa.

La detección automática de la dirección I2C y la validación del identificador del sensor ayudan a reconocer si el dispositivo conectado corresponde realmente a un BMP280. Además, el manejo de errores permite detectar lecturas incorrectas y volver a intentar la conexión cuando existe algún problema.

En general, esta práctica permitió integrar adquisición de datos ambientales, comunicación I2C, visualización en la matriz LED y monitoreo por puerto serie dentro de un mismo sistema, mostrando una forma práctica de desarrollar una pequeña estación de medición ambiental con Arduino.
