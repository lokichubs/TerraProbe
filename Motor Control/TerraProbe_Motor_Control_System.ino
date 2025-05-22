// L298N Motor Driver Pins
const int ENA = 9;
const int IN1 = 4;
const int IN2 = 5;
const int IN3 = 6;
const int IN4 = 7;
const int ENB = 10;

// Limit switch pin
const int limitSwitchPin = 11;

// ON/OFF button pin
const int toggleButtonPin = 3;

// States
bool isReversed = false;
bool lastLimitSwitchState = HIGH;
bool lastButtonState = HIGH;
bool motorsOn = false;

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void setup() {
  Serial.begin(9600);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(limitSwitchPin, INPUT_PULLUP);
  pinMode(toggleButtonPin, INPUT_PULLUP);

  Serial.println("System ready. Press button on pin 3 to start motors.");
}

void loop() {
  // === Handle ON/OFF toggle button ===
  bool currentButtonState = digitalRead(toggleButtonPin);
  if (currentButtonState == LOW && lastButtonState == HIGH && millis() - lastDebounceTime > debounceDelay) {
    motorsOn = !motorsOn;

    if (motorsOn) {
      isReversed = false; // Always start in FORWARD direction
      setMotorDirection(isReversed);
      Serial.println("Motors TURNED ON.");
    } else {
      stopMotors();
      Serial.println("Motors TURNED OFF.");
    }

    lastDebounceTime = millis();
  }
  lastButtonState = currentButtonState;

  // === Handle limit switch toggle only if motors are ON ===
  if (motorsOn) {
    bool currentLimitState = digitalRead(limitSwitchPin);
    if (currentLimitState == LOW && lastLimitSwitchState == HIGH && millis() - lastDebounceTime > debounceDelay) {
      isReversed = !isReversed;
      setMotorDirection(isReversed);
      Serial.print("Limit switch toggled → Motors now: ");
      Serial.println(isReversed ? "REVERSE" : "FORWARD");
      lastDebounceTime = millis();
    }
    lastLimitSwitchState = currentLimitState;

    // Debug info
    Serial.print("Limit Switch: ");
    Serial.print(digitalRead(limitSwitchPin) == LOW ? "PRESSED (1)" : "RELEASED (0)");
    Serial.print(" | Direction: ");
    Serial.println(isReversed ? "REVERSE" : "FORWARD");

    delay(100);
  }
}

void setMotorDirection(bool reverse) {
  if (reverse) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }

  analogWrite(ENA, 255);
  analogWrite(ENB, 255);
}

void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
