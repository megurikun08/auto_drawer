// Pin definitions
const int dirPin = 2;
const int stepPin = 3;
const int greenLED = 4;
const int redLED = 5;
const int buzzer = 6;
const int limitSwitch1 = 7;  // Drawer closed
const int limitSwitch2 = 8;  // Drawer open
const int buttonPin = 9;

// State definitions
enum State {
  IDLE_CLOSED,
  OPENING,
  IDLE_OPEN,
  CLOSING
};

State currentState = IDLE_CLOSED;
bool buttonPressed = false;

void setup() {
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  pinMode(limitSwitch1, INPUT_PULLUP); // Active LOW when pressed
  pinMode(limitSwitch2, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  checkButton();

  switch (currentState) {
    case IDLE_CLOSED:
      stopMotor();
      digitalWrite(redLED, LOW);
      digitalWrite(greenLED, LOW);
      noTone(buzzer);

      // Wait for button press and confirmation that limit switch 1 is pressed
      if (buttonPressed && digitalRead(limitSwitch1) == LOW) {
        buttonPressed = false;
        beepOnce();
        digitalWrite(dirPin, HIGH); // Set motor to forward
        currentState = OPENING;
      }
      break;

    case OPENING:
      if (digitalRead(limitSwitch2) == HIGH) {  // Continue moving until switch is PRESSED
        blinkRedLED();
        stepMotor();
      } else if (digitalRead(limitSwitch2) == LOW) { // Target reached
        stopMotor();
        digitalWrite(redLED, LOW);
        beepTwice();
        currentState = IDLE_OPEN;
      }
      break;

    case IDLE_OPEN:
      stopMotor();
      digitalWrite(redLED, LOW);
      digitalWrite(greenLED, HIGH);
      noTone(buzzer);

      // Wait for button press and confirmation that limit switch 2 is pressed
      if (buttonPressed && digitalRead(limitSwitch2) == LOW) {
        buttonPressed = false;
        digitalWrite(greenLED, LOW);
        beepOnce();
        digitalWrite(dirPin, LOW); // Set motor to reverse
        currentState = CLOSING;
      }
      break;

    case CLOSING:
      if (digitalRead(limitSwitch1) == HIGH) {  // Continue moving until switch is PRESSED
        blinkRedLED();
        stepMotor();
      } else if (digitalRead(limitSwitch1) == LOW) { // Target reached
        stopMotor();
        digitalWrite(redLED, LOW);
        beepTwice();
        currentState = IDLE_CLOSED;
      }
      break;
  }
}

void checkButton() {
  static bool lastButtonState = HIGH;
  bool currentButtonState = digitalRead(buttonPin);
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    delay(50); // debounce
    if (digitalRead(buttonPin) == LOW) {
      buttonPressed = true;
    }
  }
  lastButtonState = currentButtonState;
}

void stepMotor() {
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(800);
  digitalWrite(stepPin, LOW);
  delayMicroseconds(800);
}

void stopMotor() {
  digitalWrite(stepPin, LOW);
}

void blinkRedLED() {
  static unsigned long lastBlink = 0;
  static bool ledState = false;
  if (millis() - lastBlink >= 300) {
    ledState = !ledState;
    digitalWrite(redLED, ledState);
    lastBlink = millis();
  }
}

void beepOnce() {
  tone(buzzer, 1000);
  delay(200);
  noTone(buzzer);
}

void beepTwice() {
  tone(buzzer, 1000);
  delay(200);
  noTone(buzzer);
  delay(200);
  tone(buzzer, 1000);
  delay(200);
  noTone(buzzer);
}
