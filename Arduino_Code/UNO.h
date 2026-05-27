#include <DHT.h>

// ================= MOTOR PINS =================

// LEFT MOTOR DRIVER
#define LEFT_IN1 12
#define LEFT_IN2 13

// RIGHT MOTOR DRIVER
#define RIGHT_IN1 10
#define RIGHT_IN2 11

// ================= ULTRASONIC =================

#define TRIG 2
#define ECHO 4

// ================= BUZZER =================

#define BUZZER A1

// ================= DHT11 =================

#define DHTPIN 7
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// ================= MQ2 =================

#define GAS_SENSOR A0

// ================= FLYSKY =================

#define CH1 3
#define CH2 5

String botStatus = "STOPPED";



void setup()
{
  Serial.begin(9600);

  // MOTOR PINS

  pinMode(LEFT_IN1, OUTPUT);
  pinMode(LEFT_IN2, OUTPUT);

  pinMode(RIGHT_IN1, OUTPUT);
  pinMode(RIGHT_IN2, OUTPUT);

  // ULTRASONIC

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  // BUZZER

  pinMode(BUZZER, OUTPUT);

  // FLYSKY

  pinMode(CH1, INPUT);
  pinMode(CH2, INPUT);

  dht.begin();

  stopMotors();
}



void loop()
{
  // ================= SENSOR VALUES =================

  int distance = getDistance();

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  int gasValue = analogRead(GAS_SENSOR);



  // ================= FLYSKY VALUES =================

  int ch1Value = pulseIn(CH1, HIGH);

  int ch2Value = pulseIn(CH2, HIGH);
  if(ch1Value == 0 || ch2Value == 0)
{
  stopMotors();
  return;
}



  // ================= FIRE DETECTION =================

  bool smokeDetected = false;

  if(gasValue > 1500 || temperature > 45)
  {
    smokeDetected = true;

    // FAST WARNING BEEP

    digitalWrite(BUZZER, HIGH);
    delay(60);

    digitalWrite(BUZZER, LOW);
    delay(60);

    botStatus = "SMOKE DETECTED";
  }



  // ================= OBSTACLE DETECTION =================

  if(distance < 20)
  {
    // WARNING BEEP

    digitalWrite(BUZZER, HIGH);
    delay(80);

    digitalWrite(BUZZER, LOW);
    delay(80);

    // FORWARD BLOCKED

    if(ch2Value > 1600)
    {
      stopMotors();

      botStatus = "OBSTACLE DETECTED";
    }

    // BACKWARD ALLOWED

    else if(ch2Value < 1400)
    {
      backward();

      botStatus = "MOVING BACKWARD";
    }

    // LEFT ALLOWED

    else if(ch1Value < 1400)
    {
      left();

      botStatus = "TURNING LEFT";
    }

    // RIGHT ALLOWED

    else if(ch1Value > 1600)
    {
      right();

      botStatus = "TURNING RIGHT";
    }

    // OTHERWISE STOP

    else
    {
      stopMotors();

      botStatus = "OBSTACLE DETECTED";
    }
  }



  // ================= NORMAL CONTROL =================

  else
  {
    // TURN OFF BUZZER IF NO SMOKE

    if(!smokeDetected)
    {
      digitalWrite(BUZZER, LOW);
    }

    // DEADZONE

    if(ch1Value > 1450 && ch1Value < 1550 &&
       ch2Value > 1450 && ch2Value < 1550)
    {
      stopMotors();

      botStatus = "STOPPED";
    }

    // FORWARD

    else if(ch2Value > 1600)
    {
      forward();

      botStatus = "MOVING FORWARD";
    }

    // BACKWARD

    else if(ch2Value < 1400)
    {
      backward();

      botStatus = "MOVING BACKWARD";
    }

    // LEFT

    else if(ch1Value < 1400)
    {
      left();

      botStatus = "TURNING LEFT";
    }

    // RIGHT

    else if(ch1Value > 1600)
    {
      right();

      botStatus = "TURNING RIGHT";
    }

    else
    {
      stopMotors();

      botStatus = "STOPPED";
    }
  }



  // ================= SEND DATA TO ESP32 =================

  Serial.print(distance);
  Serial.print(",");

  Serial.print(temperature);
  Serial.print(",");

  Serial.print(humidity);
Serial.print(",");

  Serial.print(gasValue);
  Serial.print(",");

  Serial.println(botStatus);

  delay(40);
}



// ================= DISTANCE FUNCTION =================

int getDistance()
{
  long duration;

  int distance;

  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH, 30000);

  if(duration == 0)
  {
    return 999;
  }

  distance = duration * 0.034 / 2;

  return distance;
}



// ================= FORWARD =================

void forward()
{
  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, HIGH);

  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, HIGH);
}



// ================= BACKWARD =================

void backward()
{
  digitalWrite(LEFT_IN1, HIGH);
  digitalWrite(LEFT_IN2, LOW);

  digitalWrite(RIGHT_IN1, HIGH);
  digitalWrite(RIGHT_IN2, LOW);
}



// ================= LEFT =================

void left()
{
  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, HIGH);

  digitalWrite(RIGHT_IN1, HIGH);
  digitalWrite(RIGHT_IN2, LOW);
}



// ================= RIGHT =================

void right()
{
  digitalWrite(LEFT_IN1, HIGH);
  digitalWrite(LEFT_IN2, LOW);

  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, HIGH);
}



// ================= STOP =================

void stopMotors()
{
  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, LOW);

  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, LOW);
}