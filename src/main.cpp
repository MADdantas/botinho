#include <Arduino.h>
#include <PS4Controller.h>

// Posições das rodas:

//[D]-------[B]
//  |       |
//  |   ⇧   |
//  |       |
//[C]-------[A]

// LED pin
#define LED 2

// Definir as portas do ESP32 que serão usadas para controlar a ponte H
#define motorA_EN 13
#define motorA_IN1 12
#define motorA_IN2 14
#define motorB_EN 27
#define motorB_IN1 26
#define motorB_IN2 25
#define motorC_EN 15
#define motorC_IN1 4
#define motorC_IN2 5
#define motorD_EN 18
#define motorD_IN1 19
#define motorD_IN2 21

// Definir a frequência PWM para controle de velocidade
#define pwmFreq 30000

void setup() {
  // Configurar as portas como saídas
  pinMode(motorA_EN, OUTPUT);
  pinMode(motorA_IN1, OUTPUT);
  pinMode(motorA_IN2, OUTPUT);
  pinMode(motorB_EN, OUTPUT);
  pinMode(motorB_IN1, OUTPUT);
  pinMode(motorB_IN2, OUTPUT);
  pinMode(motorC_EN, OUTPUT);
  pinMode(motorC_IN1, OUTPUT);
  pinMode(motorC_IN2, OUTPUT);
  pinMode(motorD_EN, OUTPUT);
  pinMode(motorD_IN1, OUTPUT);
  pinMode(motorD_IN2, OUTPUT);
  pinMode(LED, OUTPUT);

  // Iniciar o Serial para depuração
  Serial.begin(9600);

  // Configurar a frequência PWM para controle de velocidade
  ledcSetup(0, pwmFreq, 8);
  ledcSetup(1, pwmFreq, 8);
  ledcSetup(2, pwmFreq, 8);
  ledcSetup(3, pwmFreq, 8);
  ledcAttachPin(motorA_EN, 0);
  ledcAttachPin(motorB_EN, 1);
  ledcAttachPin(motorC_EN, 2);
  ledcAttachPin(motorD_EN, 3);

  PS4.begin("F8:28:19:75:E5:EE");
  digitalWrite(LED, HIGH);
}


void loop() {
  if (PS4.isConnected()) {

    digitalWrite(LED, HIGH);

    PS4.setLed(0, 255, 0);
    PS4.sendToController();

    // Posições das rodas:
    //[D]-------[B]
    //  |       |
    //  |   ⇧   |
    //  |       |
    //[C]-------[A]

    // Velocity input
    int Vx = map(PS4.LStickY(), -128, 128, -100, 100);
    int Vy = map(PS4.LStickX(), -128, 128, -100, 100);
    int W = map(PS4.RStickX(), -128, 128, -80, 80);

    // Wheel velocity for each wheel
    // int wA = (1/0.04)*(Vx - Vy + ((0.12+0.11)/2) * W);
    // int wB = (1/0.04)*(Vx + Vy + ((0.12+0.11)/2) * W);
    // int wC = (1/0.04)*(Vx + Vy - ((0.12+0.11)/2) * W);
    // int wD = (1/0.04)*(Vx - Vy - ((0.12+0.11)/2) * W);

    // Velocidade das rodas pra cada roda
    int wA = (Vx + Vy - W);
    int wB = (Vx - Vy - W);
    int wC = (Vx - Vy + W);
    int wD = (Vx + Vy + W);

    // Definir a velocidade dos motores
    int dz = 12; // Valor da dead zone do analógico
    int motorASpeed = abs(wA)<dz ? 0 : abs(wA); // Velocidade do motor A, entre 0 e 255
    int motorBSpeed = abs(wB)<dz ? 0 : abs(wB); // Velocidade do motor B, entre 0 e 255
    int motorCSpeed = abs(wC)<dz ? 0 : abs(wC); // Velocidade do motor B, entre 0 e 255
    int motorDSpeed = abs(wD)<dz ? 0 : abs(wD); // Velocidade do motor B, entre 0 e 255

    // Definir a direção dos motores
    bool motorADir = wA<0 ? false : true; // true para frente, false para trás
    bool motorBDir = wB<0 ? false : true; // true para frente, false para trás
    bool motorCDir = wC<0 ? false : true; // true para frente, false para trás
    bool motorDDir = wD<0 ? false : true; // true para frente, false para trás

    // Controlar os motores
    analogWrite(motorA_EN, motorASpeed);
    digitalWrite(motorA_IN1, motorADir ? HIGH : LOW);
    digitalWrite(motorA_IN2, motorADir ? LOW : HIGH);
    analogWrite(motorB_EN, motorBSpeed);
    digitalWrite(motorB_IN1, motorBDir ? HIGH : LOW);
    digitalWrite(motorB_IN2, motorBDir ? LOW : HIGH);
    analogWrite(motorC_EN, motorCSpeed);
    digitalWrite(motorC_IN1, motorCDir ? HIGH : LOW);
    digitalWrite(motorC_IN2, motorCDir ? LOW : HIGH);
    analogWrite(motorD_EN, motorDSpeed);
    digitalWrite(motorD_IN1, motorDDir ? HIGH : LOW);
    digitalWrite(motorD_IN2, motorDDir ? LOW : HIGH);

    // Imprimir a velocidade dos motores no Serial para depuração
    Serial.print("Velocidade calculada A: ");
    Serial.print(wA);
    Serial.print("Motor A speed: ");
    Serial.println(motorASpeed);
    Serial.print("Velocidade calculada B: ");
    Serial.print(wB);
    Serial.print("Motor B speed: ");
    Serial.println(motorBSpeed);
    Serial.print("Velocidade calculada C: ");
    Serial.print(wC);
    Serial.print("Motor C speed: ");
    Serial.println(motorCSpeed);
    Serial.print("Velocidade calculada D: ");
    Serial.print(wD);
    Serial.print("Motor D speed: ");
    Serial.println(motorDSpeed);

  } else {
    Serial.println("Controle desconectado!");
    digitalWrite(LED, LOW);
    analogWrite(motorA_EN, 0);
    analogWrite(motorB_EN, 0);
    analogWrite(motorC_EN, 0);
    analogWrite(motorD_EN, 0);
  }
}
