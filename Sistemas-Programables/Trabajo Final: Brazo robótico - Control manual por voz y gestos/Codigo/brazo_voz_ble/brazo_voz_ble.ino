/*
  =====================================================================
   Brazo robótico KUKA (Lozada Dynamics LDArmV2)
   Control con 3 JOYSTICKS + APP (voz y botones) por BLUETOOTH LOW ENERGY
   Placa: Arduino UNO R4 WiFi        Librerías: Servo, ArduinoBLE, EEPROM
  ---------------------------------------------------------------------
   Este sketch es una versión NUEVA basada en brazo_joysticks.ino
   (ese archivo no se modificó). Las conexiones son exactamente las mismas:

   Servos (numeración del kit):
     S1 Pinza   MG90S   -> D3
     S2 Muñeca  MG90S   -> D5
     S3 Codo    MG90S   -> D6
     S4 Hombro  MG996R  -> D9
     S5 Base    MG996R  -> D10

   Joysticks HW-504 (alimentados con 5V/GND del Arduino):
     J1 (S4 y S5):  VRx -> A4 (base)     VRy -> A3 (hombro)
     J2 (S2 y S3):  VRx -> A2 (muñeca)   VRy -> A1 (codo)   SW -> A0 (HOME)
     J3 (S1):       VRx -> A5 (pinza)

  ---------------------------------------------------------------------
   FUNCIONAMIENTO
   - La app trae 2 joysticks táctiles (izquierdo: hombro S4 + base S5;
     derecho: codo S3 + muñeca S2) que funcionan igual que los físicos.
   - Joysticks y app funcionan AL MISMO TIEMPO (sin modo exclusivo):
       * Joystick = control en tiempo real (por velocidad, con zona muerta).
       * Comando de la app (voz o botón) = el brazo va SUAVE a una posición
         guardada.
       * Si mueves un joystick mientras el brazo va a una posición, el
         movimiento automático se cancela y el control manual sigue desde
         donde esté el brazo (sin saltos).
   - Movimientos progresivos: cada articulación acelera y frena suavemente
     (perfil trapezoidal) y en las posiciones de voz todas las articulaciones
     llegan al mismo tiempo.
   - HOME de emergencia: botón de la app, botón de J2, o 'h' por el monitor
     serie. Funciona en cualquier estado.
   - Memorias M1, M2, M3: la app puede guardar la posición actual del brazo
     y luego regresar a ella por voz ("memoria uno"). Se guardan en la
     EEPROM, así que no se borran al apagar.

   BLE (igual que el carro: servicio tipo UART)
     Nombre del dispositivo: BrazoKUKA
     Servicio  6E400001-B5A3-F393-E0A9-E50E24DCCA9E
     RX (app -> brazo, escribir)   6E400002-B5A3-F393-E0A9-E50E24DCCA9E
     TX (brazo -> app, notificar)  6E400003-B5A3-F393-E0A9-E50E24DCCA9E

   Comandos de texto (por BLE o por el monitor serie a 115200):
     HOME  ALTO  ABRIR  CERRAR  RECOGER  ENTREGAR  SALUDO
     IZQUIERDA  DERECHA  CENTRO  MEM1 MEM2 MEM3  GUARDAR1 GUARDAR2 GUARDAR3
     HOLA   (sube y baja el brazo 3 veces y regresa a donde estaba)
     SUBIR  BAJAR   (sube / baja el hombro un paso desde donde esté)
     CODOARRIBA  CODOABAJO  MUNECAARRIBA  MUNECAABAJO
                    (mueven SOLO el codo S3 o SOLO la muñeca S2 un paso;
                     por voz: «codo arriba», «muñeca abajo», ...)
     BESO   (tira un beso en un solo movimiento fluido: el brazo va hacia
             enfrente y hacia arriba cerrando la pinza, la abre y la cierra
             arriba, y regresa a donde estaba)
     ?  (pide el estado)
     Jix,iy,dx,dy   (joysticks táctiles de la app, -100..100; ver
                     "JOYSTICKS DE LA APP" más abajo)
   Ningún movimiento bloquea el programa: el BLE se lee en cada vuelta del
   loop, así que un comando nuevo (sobre todo ALTO) interrumpe al instante
   cualquier movimiento en curso, incluido HOLA.
   Respuestas que manda el brazo a la app:
     Ppinza,muneca,codo,hombro,base    (ángulos actuales, ej. P70,90,90,90,90)
     E:MOV <pos> | E:LLEGO <pos> | E:MANUAL | E:ALTO | E:GUARDADA Mx | ...
  =====================================================================
*/

#include <Servo.h>
#include <ArduinoBLE.h>
#include <EEPROM.h>

// ------------------------- AJUSTES GENERALES -------------------------
const unsigned long PERIODO_MS = 20;     // ciclo de control (50 Hz)
const int   ZONA_MUERTA     = 60;        // de 0..512 (sube si el brazo "tiembla")
const int   PULSO_MIN_US    = 544;       // pulso para 0°
const int   PULSO_MAX_US    = 2400;      // pulso para 180°

// Suavidad de los movimientos (grados/s²). Más bajo = arranques y frenados más suaves.
const float ACEL_MANUAL     = 400.0f;    // con joystick (respuesta rápida pero sin saltos)
// Movimientos automáticos RÁPIDOS: arrancan casi al instante (antes 90 y 0.70,
// tardaban ~2 s en hacer IZQUIERDA). Si el brazo da tirones, baja estos valores.
const float ACEL_AUTO       = 450.0f;    // yendo a una posición (voz / botones / HOME)
const float FACTOR_VEL_AUTO = 1.50f;     // velocidad de las posiciones = 150 % de velMax
const float VEL_MIN_AUTO    = 6.0f;      // grados/s mínimos en un movimiento automático

// Ciclos seguidos con joystick fuera de la zona muerta para cancelar un
// movimiento automático (3 x 20 ms = 60 ms). Evita que un pico de ruido
// cancele un comando de voz.
const uint8_t CICLOS_TOMA_MANUAL = 3;

// Botón HOME (SW del joystick 2)
const uint8_t PIN_BOTON_HOME = A0;

// ------------------------- BLE ---------------------------------------
const char* NOMBRE_BLE = "BrazoKUKA";
BLEService        servicioBrazo("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
BLECharacteristic chComandos   ("6E400002-B5A3-F393-E0A9-E50E24DCCA9E",
                                BLEWrite | BLEWriteWithoutResponse, 20);
BLECharacteristic chEstado     ("6E400003-B5A3-F393-E0A9-E50E24DCCA9E",
                                BLERead | BLENotify, 20);
bool bleListo = false;

// ------------------------- ARTICULACIONES ----------------------------
struct Articulacion {
  const char* nombre;
  uint8_t pinServo;
  uint8_t pinJoy;
  int   minAng;      // límite mínimo seguro (grados)
  int   maxAng;      // límite máximo seguro (grados)
  int   homeAng;     // posición de reposo (brazo vertical)
  float velMax;      // grados por segundo con el joystick al tope
  bool  invertir;    // true si el servo se mueve al revés de lo que quieres

  // --- variables internas (no tocar) ---
  Servo servo{};
  int   centroJoy   = 512;
  float objetivo    = 90.0f;
  float actual      = 90.0f;
  float vel         = 0.0f;   // grados/s actuales
  float velAuto     = 0.0f;   // velocidad límite del movimiento automático
  int   ultimoPulso = -1;
};

// LÍMITES (25-nov-2026, respaldo en respaldo_2026-11-25_v7/):
//   - S4 Hombro: solo se mueve de 55° a 125° (originalmente 20..160).
//   - Los demás regresaron a sus límites originales.
// Todo (joysticks físicos, flechas de la app, voz y HOME) respeta estos
// límites.
Articulacion art[] = {
  // nombre     servo joy  min  max  home  vel    invertir
  { "S1 Pinza",   3,  A5,  20, 110,  70, 120.0f, false },  // NO dejar cerrada a fuerza
  { "S2 Muneca",  5,  A2,  10, 170,  90,  90.0f, false },
  { "S3 Codo",    6,  A1,  10, 170,  90,  90.0f, false },
  { "S4 Hombro",  9,  A3,  55, 125,  90,  60.0f, false },
  { "S5 Base",   10,  A4,   5, 175,  90,  60.0f, false },
};
const uint8_t NUM_ART = sizeof(art) / sizeof(art[0]);
enum { PINZA = 0, MUNECA, CODO, HOMBRO, BASE };

// ------------------------- POSICIONES GUARDADAS ----------------------
// Orden: pinza, muñeca, codo, hombro, base.   MANT = no mover esa articulación.
// CALIBRA estos ángulos en tu brazo: mueve el brazo con los joysticks a la
// posición que quieras, lee los ángulos en el monitor serie (o en la app) y
// cópialos aquí. Siempre se respetan los límites minAng/maxAng.
const int MANT = -1;
const int PINZA_ABIERTA = 105;
const int PINZA_CERRADA = 35;            // no la cierres "a fuerza" (mín. 20)

struct Pose { const char* nombre; int ang[5]; };
const Pose POSES[] = {
  // nombre        pinza          muñeca codo hombro base
  { "HOME",      { 70,            90,    90,  90,    90  } },  // vertical, reposo
  { "RECOGER",   { PINZA_CERRADA, MANT, MANT, MANT, MANT } },  // solo cierra la pinza (agarra)
  { "ENTREGAR",  { PINZA_ABIERTA, MANT, MANT, MANT, MANT } },  // solo abre la pinza (suelta)
  { "SALUDO",    { 70,            60,    45,  90,    90  } },
  { "IZQUIERDA", { MANT,          MANT, MANT, MANT, 160  } },  // solo gira la base
  { "DERECHA",   { MANT,          MANT, MANT, MANT,  20  } },
  { "CENTRO",    { MANT,          MANT, MANT, MANT,  90  } },
  { "ABRIR",     { PINZA_ABIERTA, MANT, MANT, MANT, MANT } },  // solo la pinza
  { "CERRAR",    { PINZA_CERRADA, MANT, MANT, MANT, MANT } },
};
const uint8_t NUM_POSES = sizeof(POSES) / sizeof(POSES[0]);

// ------------------------- HOLA / SUBIR / BAJAR ----------------------
// HOLA: la articulación HOLA_ART sube y baja HOLA_VECES veces (±HOLA_AMPLITUD
// grados) y al final el brazo regresa a la posición en la que estaba.
const uint8_t HOLA_ART      = CODO;      // CODO u HOMBRO
const int     HOLA_AMPLITUD = 25;        // grados hacia arriba y hacia abajo
const uint8_t HOLA_VECES    = 3;

// SUBIR / BAJAR: mueven el hombro PASO_SUBIR_BAJAR grados desde donde esté.
// En este brazo "abajo" es ángulo MENOR en el hombro (comprobado en el brazo
// real). Si algún día sube cuando dices "baja", cambia SENTIDO_BAJAR a +1.
// Con el límite del hombro (55..125), un "baja" lo lleva de 90 a 55
// y un "sube" de 90 a 125.
const int PASO_SUBIR_BAJAR = 45;
const int SENTIDO_BAJAR    = -1;

// CODO / MUÑECA por voz: mueven SOLO esa articulación PASO_xxx grados desde
// donde esté (el resto del brazo se queda quieto). "Arriba" usa los mismos
// sentidos del BESO (SENTIDO_SUBIR_CODO / SENTIDO_SUBIR_MUNECA, más abajo):
// si en el brazo real "codo arriba" lo baja, cambia ese sentido a -1.
const int PASO_CODO   = 30;
const int PASO_MUNECA = 30;

// ------------------------- BESO ---------------------------------------
// "Tirar un beso" HACIA ENFRENTE Y HACIA ARRIBA en UN SOLO movimiento fluido
// (sin pausas ni frenadas entre pasos; respaldo de la versión anterior en
// respaldo_2026-11-17/):
//   - el hombro se inclina un poco HACIA ENFRENTE, el codo y la muñeca SUBEN
//     (la punta va hacia la persona y hacia arriba) mientras la pinza se
//     CIERRA (junta los "labios");
//   - en el punto más alto la pinza se ABRE y se CIERRA (el "muak") con un
//     pequeño empujón extra hacia enfrente, sin detener el brazo;
//   - el brazo regresa suavemente a la posición en la que estaba.
// Todas las articulaciones siguen curvas suaves (coseno): arrancan y frenan
// sin tirones y nunca se quedan quietas a medio gesto.
// SENTIDOS: en el hombro "enfrente" = SENTIDO_BAJAR (ángulo MENOR, comprobado
// en el brazo real). Si el CODO o la MUÑECA bajan en vez de subir durante el
// beso, cambia su SENTIDO_SUBIR_xxx a -1.
const unsigned long BESO_DURACION_MS     = 2400;  // duración total del beso
const int           BESO_HOMBRO_ENFRENTE = 15;    // grados que se inclina el hombro hacia enfrente
const int           BESO_CODO_ARRIBA     = 35;    // grados que sube el codo
const int           BESO_MUNECA_ARRIBA   = 30;    // grados que sube la muñeca
const float         BESO_EMPUJON         = 0.15f; // empujón extra en el "muak" (15 % más)
const int           SENTIDO_SUBIR_CODO   = -1;    // -1 = subir es ángulo MENOR (comprobado en el brazo real, 25-nov-2026)
const int           SENTIDO_SUBIR_MUNECA = +1;

// HOME por el botón de J2: hay que mantenerlo presionado este número de
// ciclos seguidos (5 x 20 ms = 100 ms) con TODOS los joysticks sueltos.
// Así un pico de ruido eléctrico (p. ej. cuando el hombro S4 hace fuerza en
// su límite) o un "clic" accidental al empujar el joystick a tope ya no
// regresan el brazo a HOME.
const uint8_t CICLOS_BOTON_HOME = 5;

// ------------------------- JOYSTICKS DE LA APP -----------------------
// La app manda "Jix,iy,dx,dy" unas 10 veces por segundo mientras se toca un
// joystick táctil (valores -100..100; x: derecha = +, y: arriba = +):
//   joystick IZQUIERDO de la app -> X: S5 base     Y: S4 hombro
//   joystick DERECHO   de la app -> X: S2 muñeca   Y: S3 codo
// Funcionan exactamente como los joysticks físicos (control por velocidad,
// mismos límites y misma suavidad). Si un joystick físico se está moviendo,
// ese manda sobre el de la app.
// Si dejan de llegar mensajes (se soltó, se desconectó o se perdió la señal)
// el brazo se detiene solo después de APP_JOY_TIMEOUT_MS.
// SENTIDOS: si en el brazo real algún eje de la app va al revés, cambia su
// signo aquí (no toques "invertir", eso cambia los joysticks físicos).
const unsigned long APP_JOY_TIMEOUT_MS = 400;
const int   APP_JOY_ZONA_MUERTA = 8;     // de 0..100
const int   SIGNO_APP_BASE      = -1;    // derecha = ángulo MENOR (como la pose DERECHA)
const int   SIGNO_APP_HOMBRO    = +1;    // arriba  = ángulo MAYOR (abajo es menor)
const int   SIGNO_APP_MUNECA    = +1;
const int   SIGNO_APP_CODO      = -1;    // arriba  = ángulo MENOR (mismo sentido que SENTIDO_SUBIR_CODO)

// Memorias del usuario (EEPROM)
const uint8_t NUM_MEM   = 3;
const uint8_t MEM_MAGIA = 0xA7;
struct Memoria { uint8_t magia; int16_t ang[5]; };

// ------------------------- ESTADO ------------------------------------
enum Modo { MANUAL, AUTO };
Modo modo = MANUAL;
char destino[13] = "";                   // nombre de la posición en curso (máx. 12 letras)
uint8_t ciclosJoyActivo = 0;

// Secuencia HOLA (no bloqueante): -1 = inactiva. 0..2*HOLA_VECES-1 = subiendo/
// bajando, 2*HOLA_VECES = regresando a la posición original.
int holaPaso = -1;
int holaRegreso[5];

// BESO (no bloqueante): trayectoria por tiempo desde besoT0.
bool besoActivo = false;
unsigned long besoT0 = 0;
float besoInicio[5];                     // posición a la que regresa

// Joysticks de la app: valor -1.0..1.0 por articulación (ya con curva y signo)
float appJoy[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
unsigned long tAppJoy = 0;               // cuándo llegó el último mensaje J

// ------------------------- PROTOTIPOS --------------------------------
void enviarApp(const char* txt);
void eventoApp(const char* a, const char* b = nullptr);
void enviarPosicion(bool forzar);
void irAPose(const int ang[5], const char* nombre, bool avisar = true);

// ------------------------- JOYSTICKS ---------------------------------
int leerJoy(uint8_t pin) {
  analogRead(pin);                  // lectura de descarte (estabiliza el ADC)
  long suma = 0;
  for (uint8_t i = 0; i < 4; i++) suma += analogRead(pin);
  return suma / 4;                  // 0..1023
}

// Convierte la posición del joystick en -1.0 .. +1.0 (con zona muerta y curva)
float joyNormalizado(Articulacion& a) {
  int d = leerJoy(a.pinJoy) - a.centroJoy;
  if (abs(d) < ZONA_MUERTA) return 0.0f;

  float rango = (d > 0) ? (1023 - a.centroJoy) : a.centroJoy;
  float n = (abs(d) - ZONA_MUERTA) / (rango - ZONA_MUERTA);
  n = constrain(n, 0.0f, 1.0f);
  n = n * n;                        // curva cuadrática: más precisión cerca del centro
  if (d < 0) n = -n;
  return a.invertir ? -n : n;
}

// ------------------------- SERVOS ------------------------------------
void escribirServo(Articulacion& a) {
  int pulso = PULSO_MIN_US + (int)(a.actual * (PULSO_MAX_US - PULSO_MIN_US) / 180.0f + 0.5f);
  if (pulso != a.ultimoPulso) {
    a.servo.writeMicroseconds(pulso);
    a.ultimoPulso = pulso;
  }
}

float limitar(const Articulacion& a, float ang) {
  return constrain(ang, (float)a.minAng, (float)a.maxAng);
}

// Distancia que necesita la articulación para frenar desde su velocidad actual
float distanciaFrenado(const Articulacion& a, float acel) {
  return a.vel * fabs(a.vel) / (2.0f * acel);
}

// Mueve una articulación un ciclo hacia su objetivo con aceleración limitada
void seguirObjetivo(Articulacion& a, float dt) {
  float acel = (modo == AUTO) ? ACEL_AUTO : ACEL_MANUAL;
  float vLim = (modo == AUTO) ? a.velAuto : a.velMax;
  float err  = a.objetivo - a.actual;

  // Velocidad deseada: la máxima que todavía permite frenar justo en el objetivo
  // (versión para pasos de tiempo discretos: llega con velocidad casi cero)
  float h    = 0.5f * acel * dt;
  float vDes = sqrtf(h * h + 2.0f * acel * fabs(err)) - h;
  if (vDes > vLim) vDes = vLim;
  if (err < 0) vDes = -vDes;

  float dv    = vDes - a.vel;
  float dvMax = acel * dt;
  a.vel += constrain(dv, -dvMax, dvMax);

  float paso = a.vel * dt;
  // Nunca pasarse del objetivo (evita oscilaciones)
  if ((err > 0 && paso > err) || (err < 0 && paso < err) || fabs(err) < 0.001f) {
    paso  = err;
    a.vel = 0.0f;
  }
  a.actual = limitar(a, a.actual + paso);
  escribirServo(a);
}

// ------------------------- MOVIMIENTOS AUTOMÁTICOS -------------------
// Lleva el brazo a una pose. Las articulaciones se sincronizan para llegar
// al mismo tiempo (movimiento coordinado y repetible).
// avisar = false solo se usa dentro de la secuencia HOLA (no la cancela ni
// manda un aviso a la app en cada vaivén).
void irAPose(const int ang[5], const char* nombre, bool avisar) {
  if (avisar) { holaPaso = -1; besoActivo = false; }  // un comando nuevo cancela HOLA y BESO
  float objetivoNuevo[5];
  float tMax = 0.0f;

  for (uint8_t i = 0; i < NUM_ART; i++) {
    Articulacion& a = art[i];
    objetivoNuevo[i] = (ang[i] == MANT) ? a.objetivo : limitar(a, ang[i]);
    float vAuto = a.velMax * FACTOR_VEL_AUTO;
    float t = fabs(objetivoNuevo[i] - a.actual) / vAuto;
    if (t > tMax) tMax = t;
  }

  for (uint8_t i = 0; i < NUM_ART; i++) {
    Articulacion& a = art[i];
    float d = fabs(objetivoNuevo[i] - a.actual);
    float vAuto = a.velMax * FACTOR_VEL_AUTO;
    a.velAuto  = (tMax > 0.0f) ? d / tMax : vAuto;    // sincronizado
    if (a.velAuto < VEL_MIN_AUTO) a.velAuto = VEL_MIN_AUTO;
    if (a.velAuto > vAuto)        a.velAuto = vAuto;
    a.objetivo = objetivoNuevo[i];
  }

  modo = AUTO;
  ciclosJoyActivo = 0;
  strncpy(destino, nombre, sizeof(destino) - 1);
  destino[sizeof(destino) - 1] = '\0';

  if (!avisar) return;
  Serial.print(F(">> Moviendo a "));
  Serial.println(destino);
  eventoApp("MOV ", destino);
}

void irAHome() {
  irAPose(POSES[0].ang, "HOME");
}

// Detiene el brazo suavemente donde esté (frena sin regresar)
void detener(const char* motivo) {
  modo = MANUAL;
  holaPaso = -1;                           // también corta la secuencia HOLA
  besoActivo = false;                      // y el BESO
  for (uint8_t i = 0; i < NUM_ART; i++) {
    Articulacion& a = art[i];
    a.objetivo = limitar(a, a.actual + distanciaFrenado(a, ACEL_MANUAL));
  }
  Serial.print(F(">> "));
  Serial.println(motivo);
  eventoApp(motivo);
}

// ------------------------- HOLA (no bloqueante) ----------------------
// Fija el objetivo del paso actual de la secuencia. El loop llama aquí cada
// vez que el brazo llega, así que nunca se usa delay() y el BLE sigue vivo.
void holaMoverPaso() {
  int ang[5];
  for (uint8_t i = 0; i < NUM_ART; i++) ang[i] = holaRegreso[i];
  if (holaPaso < 2 * HOLA_VECES) {
    const Articulacion& a = art[HOLA_ART];
    // Centro del vaivén: donde estaba, recorrido si no cabe dentro de los límites
    int centro = constrain(holaRegreso[HOLA_ART], a.minAng + HOLA_AMPLITUD,
                           a.maxAng - HOLA_AMPLITUD);
    int haciaAbajo = SENTIDO_BAJAR * HOLA_AMPLITUD;
    ang[HOLA_ART] = (holaPaso % 2 == 0) ? centro - haciaAbajo    // arriba
                                        : centro + haciaAbajo;   // abajo
  }
  // Si holaPaso == 2*HOLA_VECES, ang = holaRegreso: regresa a donde estaba
  irAPose(ang, "HOLA", false);
}

void iniciarHola() {
  besoActivo = false;                      // HOLA cancela un BESO en curso
  for (uint8_t i = 0; i < NUM_ART; i++) holaRegreso[i] = (int)(art[i].actual + 0.5f);
  holaPaso = 0;
  holaMoverPaso();
  Serial.println(F(">> Moviendo a HOLA"));
  eventoApp("MOV ", "HOLA");
}

// ------------------------- BESO (no bloqueante) ----------------------
// El loop llama a besoActualizar() cada 20 ms: calcula la posición de cada
// articulación según el tiempo transcurrido (nunca usa delay(), así que
// ALTO, otro comando o los joysticks lo cortan al instante).

// Curva suave 0 -> 1 (coseno): arranca y termina con velocidad cero
float curvaSuave(float x) {
  x = constrain(x, 0.0f, 1.0f);
  return 0.5f - 0.5f * cosf(PI * x);
}

// Tramos del gesto (fracción de BESO_DURACION_MS)
const float BESO_IDA    = 0.35f;   // 0 .. IDA      : va hacia enfrente/arriba
const float BESO_VUELTA = 0.65f;   // IDA .. VUELTA : "muak" con empujón
const float BESO_MITAD  = 0.5f * (BESO_IDA + BESO_VUELTA);  // VUELTA .. 1: regresa

// Avance del gesto en el tiempo s (0..1)
float besoExtension(float s) {
  if (s < BESO_IDA) return curvaSuave(s / BESO_IDA);
  if (s < BESO_VUELTA) {
    float e = sinf(PI * (s - BESO_IDA) / (BESO_VUELTA - BESO_IDA));
    return 1.0f + BESO_EMPUJON * e * e;      // sin cambios bruscos de velocidad
  }
  return 1.0f - curvaSuave((s - BESO_VUELTA) / (1.0f - BESO_VUELTA));
}

// Pinza: se cierra al ir, se abre y se cierra en el punto alto, y regresa
float besoPinza(float s) {
  float p0 = besoInicio[PINZA];
  if (s < BESO_IDA)
    return p0 + (PINZA_CERRADA - p0) * curvaSuave(s / BESO_IDA);
  if (s < BESO_MITAD)
    return PINZA_CERRADA + (PINZA_ABIERTA - PINZA_CERRADA) * curvaSuave((s - BESO_IDA) / (BESO_MITAD - BESO_IDA));
  if (s < BESO_VUELTA)
    return PINZA_ABIERTA + (PINZA_CERRADA - PINZA_ABIERTA) * curvaSuave((s - BESO_MITAD) / (BESO_VUELTA - BESO_MITAD));
  return PINZA_CERRADA + (p0 - PINZA_CERRADA) * curvaSuave((s - BESO_VUELTA) / (1.0f - BESO_VUELTA));
}

// Mueve el brazo un ciclo del beso. Devuelve true cuando ya terminó.
bool besoActualizar(unsigned long ahora, float dt) {
  float s = (ahora - besoT0) / (float)BESO_DURACION_MS;
  bool fin = (s >= 1.0f);
  if (fin) s = 1.0f;

  // Cuánto se mueve cada articulación en el punto alto del beso
  float delta[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
  delta[HOMBRO] = SENTIDO_BAJAR        * BESO_HOMBRO_ENFRENTE;   // hacia enfrente
  delta[CODO]   = SENTIDO_SUBIR_CODO   * BESO_CODO_ARRIBA;       // hacia arriba
  delta[MUNECA] = SENTIDO_SUBIR_MUNECA * BESO_MUNECA_ARRIBA;     // hacia arriba
  float e = besoExtension(s);

  for (uint8_t i = 0; i < NUM_ART; i++) {
    Articulacion& a = art[i];
    float nuevo = (i == PINZA) ? besoPinza(s) : besoInicio[i] + e * delta[i];
    if (fin) nuevo = besoInicio[i];          // termina exactamente donde empezó
    nuevo = limitar(a, nuevo);
    // Se guarda la velocidad para que, si un joystick o ALTO cortan el beso,
    // el brazo frene suave desde donde va.
    a.vel      = (fin || dt <= 0.0f) ? 0.0f : (nuevo - a.actual) / dt;
    a.actual   = nuevo;
    a.objetivo = nuevo;
    escribirServo(a);
  }
  return fin;
}

void iniciarBeso() {
  holaPaso = -1;                           // BESO cancela un HOLA en curso
  for (uint8_t i = 0; i < NUM_ART; i++) {
    besoInicio[i]  = art[i].actual;
    art[i].objetivo = art[i].actual;
    art[i].vel      = 0.0f;
  }
  besoActivo = true;
  besoT0 = millis();
  modo = AUTO;
  ciclosJoyActivo = 0;
  strncpy(destino, "BESO", sizeof(destino) - 1);
  destino[sizeof(destino) - 1] = '\0';
  Serial.println(F(">> Moviendo a BESO"));
  eventoApp("MOV ", "BESO");
}

// ------------------------- SUBIR / BAJAR -----------------------------
// sentido: +1 = bajar, -1 = subir. Parte del objetivo actual, así que decir
// "baja" varias veces seguidas acumula los pasos.
void subirBajar(int sentido, const char* nombre) {
  int ang[5] = { MANT, MANT, MANT, MANT, MANT };
  ang[HOMBRO] = (int)(art[HOMBRO].objetivo + 0.5f) + sentido * SENTIDO_BAJAR * PASO_SUBIR_BAJAR;
  irAPose(ang, nombre);
}

// Mueve SOLO la articulación j "delta" grados desde su objetivo actual
// (decir "codo arriba" dos veces seguidas acumula los pasos).
void moverPaso(uint8_t j, int delta, const char* nombre) {
  int ang[5] = { MANT, MANT, MANT, MANT, MANT };
  ang[j] = (int)(art[j].objetivo + 0.5f) + delta;
  irAPose(ang, nombre);
}

// ------------------------- MEMORIAS (EEPROM) -------------------------
void guardarMemoria(uint8_t n) {           // n = 1..3
  Memoria m;
  m.magia = MEM_MAGIA;
  for (uint8_t i = 0; i < NUM_ART; i++) m.ang[i] = (int16_t)(art[i].actual + 0.5f);
  EEPROM.put((n - 1) * sizeof(Memoria), m);
  char txt[12];
  snprintf(txt, sizeof(txt), "GUARDADA M%u", n);
  Serial.print(F(">> "));
  Serial.println(txt);
  eventoApp(txt);
}

void irAMemoria(uint8_t n) {
  Memoria m;
  EEPROM.get((n - 1) * sizeof(Memoria), m);
  char nombre[4];
  snprintf(nombre, sizeof(nombre), "M%u", n);
  if (m.magia != MEM_MAGIA) {
    Serial.print(F(">> Memoria vacia: "));
    Serial.println(nombre);
    eventoApp("VACIA ", nombre);
    return;
  }
  int ang[5];
  for (uint8_t i = 0; i < NUM_ART; i++) ang[i] = m.ang[i];
  irAPose(ang, nombre);
}

// ------------------------- JOYSTICKS DE LA APP -----------------------
// Convierte -100..100 en -1.0..1.0 con zona muerta y la misma curva
// cuadrática que los joysticks físicos.
float valorAppJoy(long v, int signo) {
  v = constrain(v, -100L, 100L);
  if (labs(v) < APP_JOY_ZONA_MUERTA) return 0.0f;
  float n = (labs(v) - APP_JOY_ZONA_MUERTA) / (float)(100 - APP_JOY_ZONA_MUERTA);
  n = n * n;
  return (v < 0 ? -n : n) * signo;
}

// "Jix,iy,dx,dy" -> appJoy[]. Devuelve false si el formato no es válido.
bool procesarJoyApp(const char* cmd) {
  long v[4];
  const char* p = cmd + 1;
  for (uint8_t k = 0; k < 4; k++) {
    char* fin;
    v[k] = strtol(p, &fin, 10);
    if (fin == p) return false;                       // no había número
    if (k < 3) { if (*fin != ',') return false; p = fin + 1; }
    else if (*fin != '\0') return false;
  }
  appJoy[PINZA]  = 0.0f;
  appJoy[BASE]   = valorAppJoy(v[0], SIGNO_APP_BASE);
  appJoy[HOMBRO] = valorAppJoy(v[1], SIGNO_APP_HOMBRO);
  appJoy[MUNECA] = valorAppJoy(v[2], SIGNO_APP_MUNECA);
  appJoy[CODO]   = valorAppJoy(v[3], SIGNO_APP_CODO);
  tAppJoy = millis();
  return true;
}

// ------------------------- COMANDOS ----------------------------------
void procesarComando(char* cmd) {
  // Limpiar: quitar espacios/saltos al inicio y final, pasar a mayúsculas
  while (*cmd == ' ' || *cmd == '\r' || *cmd == '\n') cmd++;
  int len = strlen(cmd);
  while (len > 0 && (cmd[len - 1] == ' ' || cmd[len - 1] == '\r' || cmd[len - 1] == '\n')) cmd[--len] = '\0';
  for (int i = 0; i < len; i++) cmd[i] = toupper((unsigned char)cmd[i]);
  if (len == 0) return;

  // Joysticks de la app: llegan ~10 veces por segundo, por eso no se
  // imprimen en el monitor serie ni se contestan.
  if (cmd[0] == 'J' && procesarJoyApp(cmd)) return;

  Serial.print(F("Comando: "));
  Serial.println(cmd);

  if (!strcmp(cmd, "H") || !strcmp(cmd, "HOME") || !strcmp(cmd, "INICIO")) { irAHome(); return; }
  if (!strcmp(cmd, "ALTO") || !strcmp(cmd, "STOP"))                         { detener("ALTO"); return; }
  if (!strcmp(cmd, "?") || !strcmp(cmd, "ESTADO")) {
    eventoApp(modo == AUTO ? "MOV " : "MANUAL", modo == AUTO ? destino : nullptr);
    enviarPosicion(true);
    return;
  }
  if (!strncmp(cmd, "GUARDAR", 7) && len == 8 && cmd[7] >= '1' && cmd[7] <= '0' + NUM_MEM) {
    guardarMemoria(cmd[7] - '0');
    return;
  }
  if (!strncmp(cmd, "MEM", 3) && len == 4 && cmd[3] >= '1' && cmd[3] <= '0' + NUM_MEM) {
    irAMemoria(cmd[3] - '0');
    return;
  }
  if (!strcmp(cmd, "HOLA"))                              { iniciarHola(); return; }
  if (!strcmp(cmd, "BESO"))                              { iniciarBeso(); return; }
  if (!strcmp(cmd, "SUBIR") || !strcmp(cmd, "ARRIBA"))   { subirBajar(-1, "SUBIR"); return; }
  if (!strcmp(cmd, "BAJAR") || !strcmp(cmd, "ABAJO"))    { subirBajar(+1, "BAJAR"); return; }
  if (!strcmp(cmd, "CODOARRIBA"))   { moverPaso(CODO,    SENTIDO_SUBIR_CODO   * PASO_CODO,   cmd); return; }
  if (!strcmp(cmd, "CODOABAJO"))    { moverPaso(CODO,   -SENTIDO_SUBIR_CODO   * PASO_CODO,   cmd); return; }
  if (!strcmp(cmd, "MUNECAARRIBA")) { moverPaso(MUNECA,  SENTIDO_SUBIR_MUNECA * PASO_MUNECA, cmd); return; }
  if (!strcmp(cmd, "MUNECAABAJO"))  { moverPaso(MUNECA, -SENTIDO_SUBIR_MUNECA * PASO_MUNECA, cmd); return; }
  for (uint8_t p = 0; p < NUM_POSES; p++) {
    if (!strcmp(cmd, POSES[p].nombre)) { irAPose(POSES[p].ang, POSES[p].nombre); return; }
  }
  Serial.println(F(">> Comando desconocido"));
  eventoApp("DESCONOCIDO");
}

// ------------------------- BLE: ENVÍO A LA APP -----------------------
void enviarApp(const char* txt) {
  if (!bleListo || !BLE.connected()) return;
  chEstado.writeValue((const uint8_t*)txt, strlen(txt));
}

void eventoApp(const char* a, const char* b) {
  char txt[21];
  snprintf(txt, sizeof(txt), "E:%s%s", a, b ? b : "");
  enviarApp(txt);
}

// Manda los ángulos a la app (solo si cambiaron, o si se fuerza)
void enviarPosicion(bool forzar) {
  static int ultimo[5] = { -1, -1, -1, -1, -1 };
  int ang[5];
  bool cambio = forzar;
  for (uint8_t i = 0; i < NUM_ART; i++) {
    ang[i] = (int)(art[i].actual + 0.5f);
    if (ang[i] != ultimo[i]) cambio = true;
  }
  if (!cambio) return;
  char txt[21];                     // "P105,116,121,126,104" = 20 bytes como máximo
  snprintf(txt, sizeof(txt), "P%d,%d,%d,%d,%d", ang[0], ang[1], ang[2], ang[3], ang[4]);
  enviarApp(txt);
  for (uint8_t i = 0; i < NUM_ART; i++) ultimo[i] = ang[i];
}

void alConectar(BLEDevice central) {
  Serial.print(F(">> App conectada: "));
  Serial.println(central.address());
}

void alDesconectar(BLEDevice) {
  Serial.println(F(">> App desconectada (los joysticks siguen funcionando)"));
  for (uint8_t i = 0; i < NUM_ART; i++) appJoy[i] = 0.0f;   // soltar los joysticks de la app
  BLE.advertise();
}

void iniciarBLE() {
  if (!BLE.begin()) {
    Serial.println(F("!! No se pudo iniciar BLE. Los joysticks funcionan igual."));
    bleListo = false;
    return;
  }
  BLE.setLocalName(NOMBRE_BLE);
  BLE.setDeviceName(NOMBRE_BLE);
  BLE.setAdvertisedService(servicioBrazo);
  servicioBrazo.addCharacteristic(chComandos);
  servicioBrazo.addCharacteristic(chEstado);
  BLE.addService(servicioBrazo);
  chEstado.writeValue((const uint8_t*)"E:LISTO", 7);
  BLE.setEventHandler(BLEConnected, alConectar);
  BLE.setEventHandler(BLEDisconnected, alDesconectar);
  BLE.advertise();
  bleListo = true;
  Serial.print(F("BLE listo. Nombre: "));
  Serial.println(NOMBRE_BLE);
}

void revisarBLE() {
  if (!bleListo) return;
  BLE.poll();
  if (chComandos.written()) {
    char buf[21];
    int n = chComandos.valueLength();
    if (n > 20) n = 20;
    memcpy(buf, chComandos.value(), n);
    buf[n] = '\0';                  // la app puede mandar o no el byte nulo final
    procesarComando(buf);
  }
}

// ------------------------- MONITOR SERIE -----------------------------
void revisarSerie() {
  static char linea[24];
  static uint8_t largo = 0;
  static unsigned long tUltimo = 0;

  while (Serial.available()) {
    char c = Serial.read();
    tUltimo = millis();
    if (c == '\n' || c == '\r') {
      linea[largo] = '\0';
      procesarComando(linea);
      largo = 0;
    } else if (largo < sizeof(linea) - 1) {
      linea[largo++] = c;
    }
  }
  // Si el monitor serie no manda salto de línea, procesar tras 100 ms
  if (largo > 0 && millis() - tUltimo > 100) {
    linea[largo] = '\0';
    procesarComando(linea);
    largo = 0;
  }
}

void imprimirAngulos() {
  for (uint8_t i = 0; i < NUM_ART; i++) {
    Serial.print(art[i].nombre);
    Serial.print('=');
    Serial.print((int)(art[i].actual + 0.5f));
    Serial.print(F("  "));
  }
  Serial.println(modo == AUTO ? F("[AUTO]") : F("[MANUAL]"));
}

// ------------------------- SETUP -------------------------------------
void setup() {
  Serial.begin(115200);
  pinMode(PIN_BOTON_HOME, INPUT_PULLUP);
  delay(500);

  Serial.println(F("Brazo KUKA - joysticks + app por voz (BLE)"));
  Serial.println(F("No toques los joysticks: calibrando centros..."));

  // Calibrar el centro de cada joystick (deben estar sueltos)
  for (uint8_t i = 0; i < NUM_ART; i++) {
    long suma = 0;
    for (uint8_t k = 0; k < 32; k++) { suma += analogRead(art[i].pinJoy); delay(2); }
    art[i].centroJoy = suma / 32;
  }

  // Conectar servos UNO POR UNO (evita picos de corriente al encender)
  for (uint8_t i = 0; i < NUM_ART; i++) {
    art[i].objetivo    = art[i].homeAng;
    art[i].actual      = art[i].homeAng;
    art[i].vel         = 0.0f;
    art[i].ultimoPulso = -1;
    art[i].servo.attach(art[i].pinServo, PULSO_MIN_US, PULSO_MAX_US);
    escribirServo(art[i]);
    delay(400);
  }

  iniciarBLE();
  Serial.println(F("Listo. Boton de J2 = HOME. Comandos por serie: HOME, RECOGER, ABRIR, ..."));
}

// ------------------------- LOOP --------------------------------------
void loop() {
  static unsigned long tAnterior = 0;
  static unsigned long tImpresion = 0;
  static unsigned long tEnvio = 0;
  static uint8_t ciclosBoton = 0;        // ciclos seguidos con el botón de J2 presionado
  static bool botonBloqueado = false;    // se presionó mientras se movía un joystick

  // BLE y monitor serie se atienden siempre (no esperan al ciclo de 20 ms)
  revisarBLE();
  revisarSerie();

  unsigned long ahora = millis();
  if (ahora - tAnterior < PERIODO_MS) return;
  float dt = (ahora - tAnterior) / 1000.0f;
  tAnterior = ahora;
  if (dt > 0.1f) dt = 0.1f;

  // --- Leer joysticks (físicos y de la app) ---
  // Si la app dejó de mandar, sus joysticks se consideran sueltos.
  if (ahora - tAppJoy > APP_JOY_TIMEOUT_MS) {
    for (uint8_t i = 0; i < NUM_ART; i++) appJoy[i] = 0.0f;
  }
  float n[NUM_ART];
  bool algunJoy = false;
  for (uint8_t i = 0; i < NUM_ART; i++) {
    n[i] = joyNormalizado(art[i]);
    if (n[i] == 0.0f) n[i] = appJoy[i];   // el joystick físico tiene prioridad
    if (n[i] != 0.0f) algunJoy = true;
  }

  // --- Botón HOME del joystick 2 ---
  // Solo cuenta si se mantiene presionado CICLOS_BOTON_HOME ciclos seguidos y
  // con todos los joysticks sueltos. Si se presionó mientras se empujaba un
  // joystick (clic accidental al llegar a tope), se ignora hasta soltarlo.
  if (digitalRead(PIN_BOTON_HOME) == LOW) {
    if (algunJoy) botonBloqueado = true;
    if (!botonBloqueado && ciclosBoton < CICLOS_BOTON_HOME) {
      ciclosBoton++;
      if (ciclosBoton == CICLOS_BOTON_HOME) {
        Serial.println(F(">> Boton J2 -> HOME"));
        irAHome();
      }
    }
  } else {
    ciclosBoton = 0;
    botonBloqueado = false;
  }

  // --- Si el usuario mueve un joystick durante un movimiento automático,
  //     el control manual retoma desde donde está el brazo ---
  if (modo == AUTO) {
    ciclosJoyActivo = algunJoy ? ciclosJoyActivo + 1 : 0;
    if (ciclosJoyActivo >= CICLOS_TOMA_MANUAL) {
      detener("MANUAL");
    }
  }

  // --- Control manual: el joystick mueve el objetivo (control por velocidad) ---
  if (modo == MANUAL) {
    for (uint8_t i = 0; i < NUM_ART; i++) {
      if (n[i] != 0.0f) {
        Articulacion& a = art[i];
        a.objetivo = limitar(a, a.objetivo + n[i] * a.velMax * dt);
      }
    }
  }

  // --- Mover suavemente cada servo hacia su objetivo ---
  bool llego = true;
  if (modo == AUTO && besoActivo) {
    llego = besoActualizar(ahora, dt);      // BESO: trayectoria suave por tiempo
  } else {
    for (uint8_t i = 0; i < NUM_ART; i++) {
      seguirObjetivo(art[i], dt);
      if (fabs(art[i].objetivo - art[i].actual) > 0.3f || fabs(art[i].vel) > 0.5f) llego = false;
    }
  }
  if (modo == AUTO && llego && holaPaso >= 0 && holaPaso < 2 * HOLA_VECES) {
    holaPaso++;                             // HOLA: siguiente vaivén o regreso
    holaMoverPaso();
  } else if (modo == AUTO && llego) {
    holaPaso = -1;
    besoActivo = false;
    modo = MANUAL;
    Serial.print(F(">> Llego a "));
    Serial.println(destino);
    eventoApp("LLEGO ", destino);
  }

  // --- Mandar ángulos a la app (5 veces por segundo como máximo) ---
  if (ahora - tEnvio >= 200) {
    tEnvio = ahora;
    enviarPosicion(false);
  }

  // --- Imprimir ángulos para calibración ---
  if (ahora - tImpresion > 300) {
    tImpresion = ahora;
    imprimirAngulos();
  }
}
