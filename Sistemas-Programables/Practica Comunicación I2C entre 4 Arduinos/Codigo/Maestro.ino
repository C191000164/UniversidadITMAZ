#include <Wire.h>

const uint8_t DIR_LED = 0x08;
const uint8_t DIR_SERVO = 0x09;
const uint8_t DIR_POT = 0x0A;

unsigned long ultimaLectura = 0;

void setup() {
    Serial.begin(9600);
    Wire.begin(); // Inicia como maestro I2C

    Serial.println("Maestro listo.");
    Serial.println("Escribe 1 para encender el LED o 0 para apagarlo.");
}

void loop() {
    // Control del LED desde el Monitor Serie
    if (Serial.available() > 0) {
        char c = Serial.read();

        if (c == '1' || c == '0') {
            byte orden = (c == '1') ? 1 : 0;

            Wire.beginTransmission(DIR_LED);
            Wire.write(orden);

            if (Wire.endTransmission() != 0) {
                Serial.println("Esclavo 1 (LED) no responde");
            }
        }
    }

    // Lee el potenciometro y actualiza el servo cada 500 ms
    if (millis() - ultimaLectura >= 500UL) {
        ultimaLectura = millis();

        if (Wire.requestFrom(DIR_POT, (uint8_t)2) == 2) {
            byte alto = Wire.read();
            byte bajo = Wire.read();

            // Reconstruye el valor: primero byte alto, luego bajo
            int valor = ((unsigned int)alto << 8) | bajo;
            valor = constrain(valor, 0, 1023);

            byte angulo = (byte)map(valor, 0, 1023, 0, 180);

            Wire.beginTransmission(DIR_SERVO);
            Wire.write(angulo);

            if (Wire.endTransmission() != 0) {
                Serial.println("Esclavo 2 (servo) no responde");
            }

            Serial.print("Potenciometro: ");
            Serial.print(valor);
            Serial.print(" -> Angulo enviado: ");
            Serial.print(angulo);
            Serial.println(" grados");
        } else {
            Serial.println("Esclavo 3 (potenciometro) no responde");
        }
    }
}
