// ===============================
// Arduino posture feedback system - 5 states
//
// EXCELLENT -> 매우 준수: 초록 LED
// GOOD      -> 준수: 초록 LED
// NORMAL    -> 보통: 노란 LED
// BAD       -> 불량: 빨간 LED + 약한 경고음 + 중간 진동
// VERY_BAD  -> 매우 불량: 빨간 LED + 강한 경고음 + 강한 진동
// ===============================

// LED 핀
const int GREEN_LED  = 8;
const int YELLOW_LED = 9;
const int RED_LED    = 10;

// 부저 핀
const int BUZZER_PIN = 11;

// 모터 제어 핀
// PWM 가능한 핀 권장: 3, 5, 6, 9, 10, 11
const int MOTOR_PIN  = 6;

// 현재 자세 상태
String postureState = "GOOD";

// 경고 타이밍 제어용
unsigned long lastActionTime = 0;
bool toggleState = false;

void setup() {
  Serial.begin(9600);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);

  setGood();
}

void loop() {
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (
      cmd == "EXCELLENT" ||
      cmd == "GOOD" ||
      cmd == "NORMAL" ||
      cmd == "BAD" ||
      cmd == "VERY_BAD"
    ) {
      postureState = cmd;
    }
  }

  if (postureState == "EXCELLENT") {
    setExcellent();
  } 
  else if (postureState == "GOOD") {
    setGood();
  } 
  else if (postureState == "NORMAL") {
    setNormal();
  } 
  else if (postureState == "BAD") {
    setBad();
  } 
  else if (postureState == "VERY_BAD") {
    setVeryBad();
  }
}

// 매우 준수
void setExcellent() {
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);

  noTone(BUZZER_PIN);
  analogWrite(MOTOR_PIN, 0);
}

// 준수
void setGood() {
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);

  noTone(BUZZER_PIN);
  analogWrite(MOTOR_PIN, 0);
}

// 보통
void setNormal() {
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, HIGH);
  digitalWrite(RED_LED, LOW);

  noTone(BUZZER_PIN);
  analogWrite(MOTOR_PIN, 0);
}

// 불량
void setBad() {
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, HIGH);

  unsigned long now = millis();

  // 0.8초마다 약한 경고
  if (now - lastActionTime >= 800) {
    lastActionTime = now;

    tone(BUZZER_PIN, 1400, 150);
    analogWrite(MOTOR_PIN, 130);

    delay(150);

    analogWrite(MOTOR_PIN, 0);
    noTone(BUZZER_PIN);
  }
}

// 매우 불량
void setVeryBad() {
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, HIGH);

  unsigned long now = millis();

  // 0.25초마다 강한 경고
  if (now - lastActionTime >= 250) {
    lastActionTime = now;
    toggleState = !toggleState;

    if (toggleState) {
      tone(BUZZER_PIN, 2300);
      analogWrite(MOTOR_PIN, 255);
    } else {
      noTone(BUZZER_PIN);
      analogWrite(MOTOR_PIN, 0);
    }
  }
}
