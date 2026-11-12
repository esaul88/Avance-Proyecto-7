#include <Servo.h>
#include <DHT.h>

#define ENA 5
#define ENB 6
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 11
#define SERVO 3
#define LASER 13
#define LIGHT A3
#define ENV 2

#define TRIG A5
#define ECHO A4

#define DHTTYPE DHT11

DHT dht(ENV, DHTTYPE);

Servo servo;

unsigned char carSpeed = 250;
char currentCommand = 's';

int rec[] = {0, 0, 0, 0};
int ult[] = {0, 0, 0};
int angle[] = {45, 90, 135};

int duration;
int distance;

void forward(){ 
  digitalWrite(ENA,HIGH); digitalWrite(ENB,HIGH);
  digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);  digitalWrite(IN4,HIGH);
}

void back(){
  digitalWrite(ENA,HIGH); digitalWrite(ENB,HIGH);
  digitalWrite(IN1,LOW);  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);
}

void left(){
  analogWrite(ENA,carSpeed); analogWrite(ENB,carSpeed);
  digitalWrite(IN1,LOW);     digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW);     digitalWrite(IN4,HIGH); 
}

void right(){
  analogWrite(ENA,carSpeed); analogWrite(ENB,carSpeed);
  digitalWrite(IN1,HIGH);    digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH);    digitalWrite(IN4,LOW);
}

void stopCar(){
  digitalWrite(ENA,LOW); digitalWrite(ENB,LOW);
}

void analyze(){
  rec[1] = map(analogRead(LIGHT), 0, 1023, 100, 0);
  rec[2] = dht.readTemperature();
  rec[3] = dht.readHumidity();
}

void sample(){
  digitalWrite(LASER, HIGH);
  delay(3000);
  digitalWrite(LASER, LOW);
  rec[0] = random(1, 11);    
  delay(1000);
}

float scan() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH);

  if(duration < 150) {
    duration = 0;
  } else if (duration > 23200) {
    duration = 0;
  }

  distance = duration * (0.0343/2);
  return distance;
}

void ultrascan() {
  for (int i = 0; i < 3; i++) {
    servo.write(angle[i]);
    ult[i] = scan();
    delay(1000);
  }
  servo.write(90);
}

void data() {
  analyze();
  sample();
  ultrascan();
}

void setup() { 
  Serial.begin(9600);
  servo.attach(SERVO);
  servo.write(90);

  pinMode(IN1,OUTPUT); pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT); pinMode(IN4,OUTPUT);
  pinMode(ENA,OUTPUT); pinMode(ENB,OUTPUT);
  
  pinMode(LASER, OUTPUT);
  pinMode(LIGHT, INPUT);
  pinMode(ENV, INPUT);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  randomSeed(analogRead(A0));

  dht.begin();

  stopCar();

  data();
}

void loop() { 
  if (Serial.available() > 0) {
    char getstr = Serial.read();

    if (getstr != '\n' && getstr != '\r') {
      currentCommand = getstr;
      
      switch(currentCommand){
        case 'f': forward();   break;
        case 'b': back();      break;
        case 'l': left();      break;
        case 'r': right();     break;
        case 's': stopCar();   break;
        case 'a': data();      break;
        default:  break;
      }
    }
  }
}