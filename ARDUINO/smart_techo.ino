#include <SoftwareSerial.h>
SoftwareSerial BTserial(2, 3); // RX | TX

char c = ' ';

#define pul_remoto 1        // seañal para cambiar estado ABIERTO <->CERRADO
#define auto_remoto 2       // cambiar estado a AUTOMATICO
#define manual_remoto 3     // cambiar estado a MANUAL
#define sensor_luz_remoto 4 // solicitar valores del SENSOR_LUZ

#define SERIAL 9600

#define LED_BLANCO 12
#define LED_ROJO_MANUAL 11
#define SENSOR_LUZ A2
#define SENSOR_LLUVIA A1
#define PIN_MOTOR_A 5
#define PIN_MOTOR_C 6
#define PIN_PUL_MANUAL 8
#define PIN_PUL_CERRADO 7
#define PIN_PUL_ABIERTO 4

#define TIEMPO_MAX_MILIS 250 // 0.25 segundo.
unsigned long tiempo_actual = 0;
unsigned long tiempo_anterior = 0;
unsigned long tiempoIncial = 0;
unsigned long tiempoActual = 0;
#define UMBRAL_TIMEOUT 10000 // 10 seg
bool activar_timeout = true;
bool pulsador_remoto = false;
#define vol_max 1023
#define vol_min 0
#define f_escala_max 100
#define f_escala_min 0
#define max_luz 40              // UMBRAL DEL SENSOR DE LUZ
#define MAX_LLUVIA 100          // UMBRAL DEL sensor de lluvia
#define velocidad_motor_min 0   // velocidad de movimiento de motor detenido
#define velocidad_motor_max 255 // velocidad de movimiento de motor

#define MODO_AUTOMATICO 11
#define MODO_MANUAL 22

#define ESTADO_INICIAL -1
#define ESTADO_ABRIENDO 0
#define ESTADO_CERRANDO 1
#define ESTADO_ABIERTO 2
#define ESTADO_CERRADO 3
#define ESTADO_ERROR 4

#define EVENTO_PULSADOR 0
#define EVENTO_FIN_CARRERA 1
#define EVENTO_LLUVIA_HIGH 2
#define EVENTO_LUZ_HIGH 3
#define EVENTO_LUZ_LOW 4
#define EVENTO_LLUVIA_LOW 5
#define EVENTO_TIMEOUT 6
#define EVENTO_CONTINUE 7

int modo_actual = MODO_AUTOMATICO;
int estado_actual = ESTADO_INICIAL; // estado de iniciacion del sistema
int evento = -1;
bool fin_carrera = false;
bool luz_dia = false;
bool lluvia = false;
bool TIMEOUT = false;
int lluviaSensor = 0;
int lectura_per = 0;

/*
 * leer monitor serial y sensor bluetooth HC05
 */
void lectura_comand()
{
  // sI reciben datos del HC05
  if (BTserial.available())
  {
    // se los lee y se los muestra en el monitor serie
    c = BTserial.read();
    Serial.write(c);
    analizarDato(c);
  }
  // Si se ingresa datos por teclado en el monitor serie
  if (Serial.available())
  {
    // se los lee y se los envia al HC05
    c = Serial.read();
    Serial.write(c);
    BTserial.write(c);
  }
}

void analizarDato(char c)
{
  switch (c)
  {
  case (pul_remoto):
    pulsador_remoto = true;
    break;
  case (auto_remoto):
    modo_actual = MODO_AUTOMATICO;
    break;
  case (manual_remoto):
    modo_actual = MODO_MANUAL;
    break;
  case (sensor_luz_remoto):
    BTserial.write(lectura_per); // envia valores del SENSOR_LUZ a la APP
    break;
  }
}
bool verificarTimeout()
{
  if (activar_timeout == true)
  {
    tiempoIncial = millis();
    activar_timeout = false;
  }
  else
  {
    tiempoActual = millis();
    if ((tiempoActual - tiempoIncial) >= UMBRAL_TIMEOUT)
    {
      return true;
    }
  }
  return false;
}

/**
 * GIRA EL MOTOR, +TIME, SI EXCEDE EL TIEMPO DE APERTURA/CIERRE
 * LANZA ERROR
 */
int girar_motor(int pin_motor, int pin_motor_B, int finCarrera)
{
  if (verificarTimeout() == true) // Timeout
  {
    TIMEOUT = true;
    analogWrite(pin_motor, velocidad_motor_min); // parar giro del motor
    return 0;
  }

  if (digitalRead(finCarrera) != HIGH) // sigue girando el motor
  {
    analogWrite(pin_motor_B, velocidad_motor_min);
    analogWrite(pin_motor, velocidad_motor_max);
    return 0;
  }
  else
  {
    analogWrite(pin_motor, velocidad_motor_min);
    fin_carrera = true;
    activar_timeout = true;
  }
  return 1;
}

/**
 * RELEVAMIENTO DE LOS SENSORES
 */
void leer_sensores()
{
  int lectura_luz = analogRead(SENSOR_LUZ);
  lectura_per = map(lectura_luz, vol_min, vol_max, f_escala_min, f_escala_max);
  lectura_per > max_luz ? luz_dia = true : luz_dia = false;

  lluviaSensor = analogRead(SENSOR_LLUVIA);
  int lectura_per_lluvia = map(lluviaSensor, vol_min, vol_max, f_escala_min, f_escala_max);
  lectura_per_lluvia > MAX_LLUVIA ? lluvia = true : lluvia = false;
}

/**
 * ACTUALIZA ENCENDIDO/APAGADO DE LEDs
 */
void actualizar_led()
{
  estado_actual == ESTADO_CERRADO ? digitalWrite(LED_BLANCO, HIGH) : digitalWrite(LED_BLANCO, LOW);
  if (modo_actual == MODO_MANUAL)
    digitalWrite(LED_ROJO_MANUAL, HIGH);
  else
    digitalWrite(LED_ROJO_MANUAL, LOW);
}

bool verificarSensorPulsador()
{
  if (digitalRead(PIN_PUL_MANUAL) == HIGH)
  {
    return true; // pulsador manual oprimido
  }
  return false;
}

/*
 * GENERADOR DE EVENTO BASAO EN MODO Y SENSORES
 */
int generar_evento()
{
  leer_sensores();  // relevamiento de sensores
  actualizar_led(); // actualizar leds
  lectura_comand(); // relevamiento de monitor serial y bluetooth
  switch (modo_actual)
  {
  case (MODO_AUTOMATICO):
    if (TIMEOUT && (verificarSensorPulsador() || pulsador_remoto))
      return EVENTO_PULSADOR;
    if (TIMEOUT)
      return EVENTO_TIMEOUT;
    if (fin_carrera) //??
      return EVENTO_FIN_CARRERA;
    if (verificarSensorPulsador() || pulsador_remoto)
      return EVENTO_PULSADOR;
    if (lluvia == true)
      return EVENTO_LLUVIA_HIGH;
    if (luz_dia == true)
      return EVENTO_LUZ_HIGH;
    return EVENTO_LUZ_LOW;

  case (MODO_MANUAL):
    if (TIMEOUT && (verificarSensorPulsador() || pulsador_remoto))
      return EVENTO_PULSADOR;
    if (TIMEOUT)
      return EVENTO_TIMEOUT;
    if (fin_carrera)
      return EVENTO_FIN_CARRERA;
    if (verificarSensorPulsador() || pulsador_remoto)
      return EVENTO_PULSADOR;
    return EVENTO_CONTINUE;
  }
}

/*
 *Maquina de transicion de estados
 */
void maquina_estado()
{
  evento = generar_evento();

  switch (modo_actual)
  {
  /*
   *MODO AUTOMATICO
   */
  case (MODO_AUTOMATICO):
    switch (estado_actual)
    {
    case (ESTADO_INICIAL):
      Serial.println("INICIANDO......");
      estado_actual = ESTADO_CERRADO;
      Serial.println("ESTADO_CERRADO");
      BTserial.println("ESTADO_CERRADO");
      break;
    case (ESTADO_ABRIENDO):
      switch (evento)
      {
      case (EVENTO_FIN_CARRERA):
        Serial.println("ESTADO_ABIERTO");
        BTserial.println("ESTADO_ABIERTO");
        estado_actual = ESTADO_ABIERTO;
        activar_timeout = true;
        fin_carrera = false;
        break;
      case (EVENTO_TIMEOUT):
        Serial.println("ESTADO_ERROR");
        BTserial.println("ESTADO_ERROR");
        estado_actual = ESTADO_ERROR;
        evento = EVENTO_CONTINUE;
        break;
      default:
        girar_motor(PIN_MOTOR_A, PIN_MOTOR_C, PIN_PUL_ABIERTO);
      }
      break;
    case (ESTADO_CERRANDO):
      switch (evento)
      {
      case (EVENTO_FIN_CARRERA):
        Serial.println("ESTADO_CERRADO");
        BTserial.println("ESTADO_CERRADO");
        estado_actual = ESTADO_CERRADO;
        activar_timeout = true;
        fin_carrera = false;
        break;
      case (EVENTO_TIMEOUT):
        Serial.println("ESTADO_ERROR");
        estado_actual = ESTADO_ERROR;
        evento = EVENTO_CONTINUE;
        break;
      case (EVENTO_LLUVIA_HIGH):
        girar_motor(PIN_MOTOR_C, PIN_MOTOR_A, PIN_PUL_CERRADO);
        break;
      default:
        girar_motor(PIN_MOTOR_C, PIN_MOTOR_A, PIN_PUL_CERRADO);
      }
      break;
    case (ESTADO_ABIERTO):
      switch (evento)
      {
      case (EVENTO_PULSADOR):
        Serial.println("MODO MANUAL ACTIVADO");
        Serial.println("ESTADO_CERRANDO >>>>");
        estado_actual = ESTADO_CERRANDO;
        modo_actual = MODO_MANUAL;
        break;
      case (EVENTO_LLUVIA_HIGH): //
        Serial.println("ESTADO_CERRANDO POR LLUVIA>>>>");
        estado_actual = ESTADO_CERRANDO;
        break;
      case (EVENTO_LUZ_LOW):
        Serial.println("ESTADO_CERRANDO >>>>");
        estado_actual = ESTADO_CERRANDO;
        break;
      case (EVENTO_LUZ_HIGH): // continue....
        // actualizar_led();
        break;
      case (EVENTO_CONTINUE):
        break;
      }
      break;
    case (ESTADO_CERRADO):
      switch (evento)
      {
      case (EVENTO_PULSADOR):
        Serial.println("MODO MANUAL ACTIVADO");
        Serial.println("ESTADO_ABRIENDO >>>>");
        estado_actual = ESTADO_ABRIENDO;
        modo_actual = MODO_MANUAL;
        // actualizar_led();
        break;
      case (EVENTO_LLUVIA_HIGH):
        // actualizar_led();
        break;
      case (EVENTO_LUZ_HIGH):
        Serial.println("ESTADO_ABRIENDO >>>>");
        estado_actual = ESTADO_ABRIENDO;
        break;
      case (EVENTO_LUZ_LOW): // continue...
        // actualizar_led();
        break;
      case (EVENTO_CONTINUE):
        // actualizar_led();
        break;
      }
      break;
    case (ESTADO_ERROR):
      switch (evento)
      {
      case (EVENTO_PULSADOR):
        estado_actual = ESTADO_CERRANDO;
        Serial.println("ESTADO_CERRANDO >>>>");
        TIMEOUT = false;
        activar_timeout = true;
        fin_carrera = false;
        break;
      case (EVENTO_CONTINUE):
        break;
      }
      break;
    }
    break;

    /*
     *MODO MANUAL
     */
  case (MODO_MANUAL):
    switch (estado_actual)
    {
    case (ESTADO_ABRIENDO):
      switch (evento)
      {
      case (EVENTO_FIN_CARRERA):
        Serial.println("ESTADO_ABIERTO");
        BTserial.println("ESTADO_ABIERTO");
        estado_actual = ESTADO_ABIERTO;
        activar_timeout = true;
        fin_carrera = false;
        break;
      case (EVENTO_PULSADOR):
        Serial.println("MODO MANUAL DESACTIVADO");
        modo_actual = MODO_AUTOMATICO;
        break;
      case (EVENTO_TIMEOUT):
        Serial.println("ESTADO_ERROR");
        estado_actual = ESTADO_ERROR;
        evento = EVENTO_CONTINUE;
        break;
      case (EVENTO_CONTINUE):
        girar_motor(PIN_MOTOR_A, PIN_MOTOR_C, PIN_PUL_ABIERTO);
        break;
      default:
        girar_motor(PIN_MOTOR_A, PIN_MOTOR_C, PIN_PUL_ABIERTO);
      }
      break;
    case (ESTADO_CERRANDO):
      switch (evento)
      {
      case (EVENTO_FIN_CARRERA):
        Serial.println("ESTADO_CERRADO");
        BTserial.println("ESTADO_CERRADO");
        estado_actual = ESTADO_CERRADO;
        activar_timeout = true;
        fin_carrera = false;
        break;
      case (EVENTO_PULSADOR):
        Serial.println("MODO MANUAL DESACTIVADO");
        modo_actual = MODO_AUTOMATICO;
        break;
      case (EVENTO_TIMEOUT):
        Serial.println("ESTADO_ERROR");
        estado_actual = ESTADO_ERROR;
        evento = EVENTO_CONTINUE;
        break;
      case (EVENTO_CONTINUE):
        girar_motor(PIN_MOTOR_C, PIN_MOTOR_A, PIN_PUL_CERRADO);
        break;
      default:
        girar_motor(PIN_MOTOR_C, PIN_MOTOR_A, PIN_PUL_CERRADO);
      }
    case (ESTADO_ABIERTO):
      switch (evento)
      {
      case (EVENTO_PULSADOR):
        Serial.println("ESTADO_CERRANDO >>>>");
        estado_actual = ESTADO_CERRANDO;
        // actualizar_led();
        break;
      case (EVENTO_CONTINUE):
        // actualizar_led();
        break;
      }
      break;
    case (ESTADO_CERRADO):
      switch (evento)
      {
      case (EVENTO_PULSADOR):
        Serial.println("ESTADO_ABRIENDO >>>>");
        estado_actual = ESTADO_ABRIENDO;
        // actualizar_led();
        break;
      case (EVENTO_CONTINUE):
        // actualizar_led();
        break;
      }
      break;
    case (ESTADO_ERROR):
      switch (evento)
      {
      case (EVENTO_PULSADOR):
        estado_actual = ESTADO_CERRANDO;
        Serial.println("ESTADO_CERRANDO >>>>");
        TIMEOUT = false;
        activar_timeout = true;
        fin_carrera = false;
        break;
      case (EVENTO_CONTINUE):
        break;
      }
      break;
    }
    break;
  }
}

void setup()
{
  Serial.begin(SERIAL);
  // Se configura la velocidad de transferencia de datos entre el Bluethoot  HC05 y el de Android.
  BTserial.begin(9600);

  pinMode(LED_BLANCO, OUTPUT);
  pinMode(LED_ROJO_MANUAL, OUTPUT);
  pinMode(SENSOR_LUZ, INPUT);
  pinMode(SENSOR_LLUVIA, INPUT);
  pinMode(PIN_PUL_CERRADO, INPUT);
  pinMode(PIN_PUL_ABIERTO, INPUT);
  pinMode(PIN_PUL_MANUAL, INPUT);
  pinMode(PIN_MOTOR_A, OUTPUT);
  pinMode(PIN_MOTOR_C, OUTPUT);
}

void loop()
{
  // Toma el tiempo actual.
  tiempo_actual = millis();

  // Verifica cuanto transcurrido de tiempo
  if ((tiempo_actual - tiempo_anterior) >= TIEMPO_MAX_MILIS)
  {
    // Actualizo el tiempo anterior.
    tiempo_anterior = tiempo_actual;
    maquina_estado();
  }
}