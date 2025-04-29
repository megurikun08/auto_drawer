// Pin definitions
const int dirPin = 2;
const int stepPin = 3;
const int greenLED = 4;
const int redLED = 5;
const int buzzer = 6;
const int buttonPin = 9;

// State definitions
enum State {
  STATE_0, // Initial state: All off
  IDLE_CLOSED,
  OPENING,
  IDLE_OPEN,
  CLOSING
};

State currentState = STATE_0; // Start in the initial state
bool buttonPressed = false;

unsigned long movementStartTime = 0;
const unsigned long movementDuration = 5000; // 5 seconds

void setup() {
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  // Initialize all outputs to LOW in setup()
  digitalWrite(dirPin, LOW);
  digitalWrite(stepPin, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW);
  digitalWrite(buzzer, LOW);
}

void loop() {
  checkButton();

  switch (currentState) {
    case STATE_0: // New initial state
      // All components are off, do nothing
      if (buttonPressed) {
        buttonPressed = false; // Consume the button press
        currentState = IDLE_CLOSED; // Go to the IDLE_CLOSED state to begin operation
      }
      break;

    case IDLE_CLOSED:
      stopMotor();
      digitalWrite(redLED, LOW);
      digitalWrite(greenLED, LOW);
      noTone(buzzer);

      if (buttonPressed) {
        buttonPressed = false;
        beepOnce();
        digitalWrite(dirPin, HIGH); // Forward direction
        movementStartTime = millis();
        currentState = OPENING;
      }
      break;

    case OPENING:
      if (millis() - movementStartTime < movementDuration) {
        blinkRedLED();
        stepMotor();
      } else {
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

      if (buttonPressed) {
        buttonPressed = false;
        digitalWrite(greenLED, LOW);
        beepOnce();
        digitalWrite(dirPin, LOW); // Reverse direction
        movementStartTime = millis();
        currentState = CLOSING;
      }
      break;

    case CLOSING:
      if (millis() - movementStartTime < movementDuration) {
        blinkRedLED();
        stepMotor();
      } else {
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
