tyyyuu\p[]\p[]ipupppppppp\p]i]u]\\i]\]iyoo[\pp// IR Sensor
const int irSensorPin = 2;

// Motor A pins (Left motor)
const int IN1 = 4;
const int IN2 = 5;
const int ENA = 9;

// Motor B pins (Right motor)
const int IN3 = 6;
const int IN4 = 7;
const int ENB = 10;

// Pulse tracking
volatile unsigned long lastPulseTime = 0;
volatile unsigned long pulseInterval = 0;
volatile bool newPulse = false;

// Debounce time (in microseconds)
const unsigned long debounceMicros = 200000; // 200 ms for slow motors

void setup() {
  Serial.begin(9600);

  // Set motor pins as outputs
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT); pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT); pinMode(ENB, OUTPUT);

  // Set IR sensor pin
  pinMode(irSensorPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(irSensorPin), onPulse, FALLING);

  // Start Motor A forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 250);

  // Start Motor B forward
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 255);

  Serial.println("Measuring RPM from IR sensor...");
}

void loop() {
  if (newPulse) {
    noInterrupts();
    unsigned long interval = pulseInterval;
    newPulse = false;
    interrupts();

    if (interval > 0) {
      float rpm = (60.0 * 1000000.0) / interval;
      Serial.print("RPM: ");
      Serial.println(rpm, 2);
    }
  }
}

void onPulse() {
  unsigned long now = micros();
  if (now - lastPulseTime > debounceMicros) {
    if (lastPulseTime > 0) {
      pulseInterval = now - lastPulseTime;
      newPulse = true;
    }
    lastPulseTime = now;
  }
}