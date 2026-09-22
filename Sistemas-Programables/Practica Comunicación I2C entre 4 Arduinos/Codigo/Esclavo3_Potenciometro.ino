#include <Wire.h>

void setup() {
    Wire.begin(0x0A);
    Wire.onRequest(enviar);
}

void loop() {
}

void enviar() {
    int valor = analogRead(A0); // 0 a 1023

    // Envia primero el byte alto y despues el bajo
    Wire.write(highByte(valor));
    Wire.write(lowByte(valor));
}
