#include <SoftwareSerial.h>
SoftwareSerial BTserial(2, 3); // RX | TX

#define SERIAL 9600

//Pines de Sensores y actuadores
#define LED_BLANCO         12
#define LED_ROJO_MANUAL    11
#define SENSOR_LUZ         A1
#define SENSOR_LLUVIA      A2
#define PIN_MOTOR_A        5
#define PIN_MOTOR_C        6
#define PIN_PUL_MANUAL     8
#define PIN_PUL_CERRADO    7
#define PIN_PUL_ABIERTO    4

char c = ' ';

//Señales de comando admitidos
#define PUL_REMOTO           '1'          // seañal para cambiar estado ABIERTO <->CERRADO
#define AUTO_REMOTO          '2'          // cambiar estado a AUTOMATICO
#define MANUAL_REMOTO        '3'          // cambiar estado a MANUAL
#define SENSOR_LUZ_REMOTO    '4'          // solicitar valores del SENSOR_LUZ
#define SENSOR_LLUVIA_REMOTO '5'          // solicitar valores del SENSOR_lluvia

//Tipo de mensajes enviados
#define VALOR_SENSOR         "1"
#define VALOR_ESTADO         "0" 


#define TIEMPO_MAX_MILIS      250         // 0.25 segundo.
#define UMBRAL_TIMEOUT        10000       // 10 seg
unsigned long tiempo_actual = 0;
unsigned long tiempo_anterior = 0;
unsigned long tiempoIncial = 0;
unsigned long tiempoActual = 0;

bool activar_timeout = true;
bool pulsador_remoto = false;
#define VOL_MAX             1023
#define VOL_MIN             0
#define F_ESCALA_MAX        100
#define F_ESCALA_MIN        0
#define MAX_LUZ             40           // UMBRAL DEL SENSOR DE LUZ
#define MAX_LLUVIA          85           // UMBRAL DEL sensor de lluvia
#define VEL_MOTOR_MIN       0            // velocidad de movimiento de motor detenido
#define VEL_MOTOR_MAX       255          // velocidad de movimiento de motor

//Modos de funcionamiento del sistema
#define MODO_AUTOMATICO     11
#define MODO_MANUAL         22

//Estados del sitema
#define ESTADO_INICIAL     -1
#define ESTADO_ABRIENDO     0
#define ESTADO_CERRANDO     1
#define ESTADO_ABIERTO      2
#define ESTADO_CERRADO      3
#define ESTADO_ERROR        4

//Eventos del sistema
#define EVENTO_PULSADOR     0
#define EVENTO_FIN_CARRERA  1
#define EVENTO_LLUVIA_HIGH  2
#define EVENTO_LUZ_HIGH     3
#define EVENTO_LUZ_LOW      4
#define EVENTO_LLUVIA_LOW   5
#define EVENTO_TIMEOUT      6
#define EVENTO_CONTINUE     7

int modo_actual = MODO_AUTOMATICO;    // modo de inicial del sistema
int estado_actual = ESTADO_INICIAL;   // estado de iniciacion del sistema
int evento = -1;
bool fin_carrera = false;
bool luz_dia = false;
bool lluvia = false;
bool TIMEOUT = false;
int lluviaSensor = 0;
int lectura_per = 0;
int lectura_per_lluvia = 0;


/*
 * Leer monitor serial y sensor bluetooth HC05
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

/*
 * Analiza el comando recivido por el mopnitor serial/ bluetooth
 */
void analizarDato(char c)
{
  switch (c)
  {
  case (PUL_REMOTO):
    pulsador_remoto = true;
    break;
  case (AUTO_REMOTO):
    modo_actual = MODO_AUTOMATICO;
    break;
  case (MANUAL_REMOTO):
    modo_actual = MODO_MANUAL;    
    break;
  case (SENSOR_LUZ_REMOTO):
    BTserial.println(VALOR_SENSOR + String(lectura_per) + " %"); // envia valores del SENSOR_LUZ a la APP
    break;
  case (SENSOR_LLUVIA_REMOTO):
    BTserial.println(VALOR_SENSOR + String(lectura_per_lluvia) + " %");
    break;
  default:
    Serial.println("Comando no admitido");
  }
}

/**
* Comprueba Timeout de la duracion de giro del motor
 */
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
int girar_motor(int pin_motor_A, int pin_motor_B, int pin_finCarrera)
{
  if (verificarTimeout() == true) // Timeout
  {
    TIMEOUT = true;
    analogWrite(pin_motor_A, VEL_MOTOR_MIN); // parar giro del motor
    return 0;
  }

  if (digitalRead(pin_finCarrera) != HIGH) // sigue girando el motor
  {
    analogWrite(pin_motor_B, VEL_MOTOR_MIN);
    analogWrite(pin_motor_A, VEL_MOTOR_MAX);
    return 0;
  }
  else
  {
    analogWrite(pin_motor_A, VEL_MOTOR_MIN);
    fin_carrera = true;
  }
  return 1;
}

/**
 * RELEVAMIENTO DE LOS SENSORES
 */
void leer_sensores()
{
  int lectura_luz = analogRead(SENSOR_LUZ);
  lectura_per = map(lectura_luz, VOL_MIN, VOL_MAX, F_ESCALA_MIN, F_ESCALA_MAX);
  lectura_per > MAX_LUZ ? luz_dia = true : luz_dia = false;
  
  lluviaSensor = analogRead(SENSOR_LLUVIA);
  lectura_per_lluvia = map(lluviaSensor, VOL_MIN, VOL_MAX, F_ESCALA_MIN, F_ESCALA_MAX);
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
  leer_sensores();                // relevamiento de sensores
  actualizar_led();               // actualizar leds
  lectura_comand();               // relevamiento de monitor serial y bluetooth
  switch (modo_actual)
  {
  case (MODO_AUTOMATICO):
    if (TIMEOUT && (verificarSensorPulsador() || pulsador_remoto))
      return EVENTO_PULSADOR;
    if (TIMEOUT)
      return EVENTO_TIMEOUT;
    if (fin_carrera) 
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
 * Maquina de transicion de estados
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
      BTserial.println(VALOR_ESTADO + String("CERRADO"));
      break;
    case (ESTADO_ABRIENDO):
      switch (evento)
      {
      case (EVENTO_FIN_CARRERA):
        Serial.println("ESTADO_ABIERTO");
        BTserial.println(VALOR_ESTADO + String("ABIERTO"));
        estado_actual = ESTADO_ABIERTO;
        activar_timeout = true;
        fin_carrera = false;
        break;
      case (EVENTO_TIMEOUT):
        Serial.println("ESTADO_ERROR");
        BTserial.println(VALOR_ESTADO + String("ESTADO_ERROR"));
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
        BTserial.println(VALOR_ESTADO + String("CERRADO"));
        estado_actual = ESTADO_CERRADO;
        activar_timeout = true;
        fin_carrera = false;
        break;
      case (EVENTO_TIMEOUT):
        Serial.println("ESTADO_ERROR");
        BTserial.println(VALOR_ESTADO + String("ESTADO_ERROR"));
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
        BTserial.println(VALOR_ESTADO + String("CERRANDO >>>>"));
        estado_actual = ESTADO_CERRANDO;
        modo_actual = MODO_MANUAL;
        pulsador_remoto = false;
        break;
      case (EVENTO_LLUVIA_HIGH): //
        Serial.println("ESTADO_CERRANDO POR LLUVIA>>>>");
        BTserial.println(VALOR_ESTADO + String("CERRANDO POR LLUVIA>>>>"));
        estado_actual = ESTADO_CERRANDO;
        break;
      case (EVENTO_LUZ_LOW):
        Serial.println("ESTADO_CERRANDO >>>>");
        BTserial.println(VALOR_ESTADO + String("CERRANDO >>>>"));
        estado_actual = ESTADO_CERRANDO;
        break;
      case (EVENTO_LUZ_HIGH): // continue....

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
        BTserial.println(VALOR_ESTADO + String("ABRIENDO >>>>"));
        estado_actual = ESTADO_ABRIENDO;
        modo_actual = MODO_MANUAL;
        pulsador_remoto = false;

        break;
      case (EVENTO_LLUVIA_HIGH):

        break;
      case (EVENTO_LUZ_HIGH):
        Serial.println("ESTADO_ABRIENDO >>>>");
        BTserial.println(VALOR_ESTADO + String("ABRIENDO >>>>"));
        estado_actual = ESTADO_ABRIENDO;
        break;
      case (EVENTO_LUZ_LOW): // continue...

        break;
      case (EVENTO_CONTINUE):

        break;
      }
      break;
    case (ESTADO_ERROR):
      switch (evento)
      {
      case (EVENTO_PULSADOR):
        estado_actual = ESTADO_CERRANDO;
        Serial.println("ESTADO_CERRANDO >>>>");
        BTserial.println(VALOR_ESTADO + String("CERRANDO >>>>"));
        TIMEOUT = false;
        activar_timeout = true;
        fin_carrera = false;
        pulsador_remoto = false;
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
        BTserial.println(VALOR_ESTADO + String("ABIERTO"));
        estado_actual = ESTADO_ABIERTO;
        activar_timeout = true;
        fin_carrera = false;
        break;
      case (EVENTO_PULSADOR):
        Serial.println("MODO MANUAL DESACTIVADO");
        modo_actual = MODO_AUTOMATICO;
        pulsador_remoto = false;
        break;
      case (EVENTO_TIMEOUT):
        Serial.println("ESTADO_ERROR");
        BTserial.println(VALOR_ESTADO + String("ESTADO_ERROR"));
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
        BTserial.println(VALOR_ESTADO + String("CERRADO"));
        estado_actual = ESTADO_CERRADO;
        activar_timeout = true;
        fin_carrera = false;
        break;
      case (EVENTO_PULSADOR):
        Serial.println("MODO MANUAL DESACTIVADO");
        modo_actual = MODO_AUTOMATICO;
        pulsador_remoto = false;
        break;
      case (EVENTO_TIMEOUT):
        Serial.println("ESTADO_ERROR");
        BTserial.println(VALOR_ESTADO + String("ESTADO_ERROR"));
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
        BTserial.println(VALOR_ESTADO + String("CERRANDO >>>>"));
        estado_actual = ESTADO_CERRANDO;
        pulsador_remoto = false;
        break;
      case (EVENTO_CONTINUE):

        break;
      }
      break;
    case (ESTADO_CERRADO):
      switch (evento)
      {
      case (EVENTO_PULSADOR):
        pulsador_remoto = false;
        Serial.println("ESTADO_ABRIENDO >>>>");
        BTserial.println(VALOR_ESTADO + String("ABRIENDO >>>>"));
        estado_actual = ESTADO_ABRIENDO;
        break;
      case (EVENTO_CONTINUE):

        break;
      }
      break;
    case (ESTADO_ERROR):
      switch (evento)
      {
      case (EVENTO_PULSADOR):
        pulsador_remoto = false;
        estado_actual = ESTADO_CERRANDO;
        Serial.println("ESTADO_CERRANDO >>>>");
        BTserial.println(VALOR_ESTADO + String("CERRANDO >>>>"));
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
