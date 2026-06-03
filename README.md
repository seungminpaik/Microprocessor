
// ===============================
// Arduino posture feedback system
// GOOD  -> 초록 LED
// WARN  -> 노란 LED + 약한 부저 + 약한 진동
// BAD   -> 빨간 LED + 강한 부저 + 강한 진동
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
bool badToggle = false;

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
  // 웹에서 들어온 시리얼 명령 읽기
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "GOOD" || cmd == "WARN" || cmd == "BAD") {
      postureState = cmd;
    }
  }

  if (postureState == "GOOD") {
    setGood();
  } 
  else if (postureState == "WARN") {
    setWarn();
  } 
  else if (postureState == "BAD") {
    setBad();
  }
}

// -------------------------------
// 자세 양호
// -------------------------------
void setGood() {
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);

  noTone(BUZZER_PIN);
  analogWrite(MOTOR_PIN, 0);
}

// -------------------------------
// 자세 살짝 불량
// 노란 LED + 약한 경고음 + 약한 진동
// -------------------------------
void setWarn() {
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, HIGH);
  digitalWrite(RED_LED, LOW);

  unsigned long now = millis();

  // 1초마다 짧게 경고
  if (now - lastActionTime >= 1000) {
    lastActionTime = now;

    tone(BUZZER_PIN, 1200, 120);   // 약한 삐 소리
    analogWrite(MOTOR_PIN, 90);    // 약한 진동

    delay(120);

    analogWrite(MOTOR_PIN, 0);
    noTone(BUZZER_PIN);
  }
}

// -------------------------------
// 자세 불량
// 빨간 LED + 강한 경고음 + 강한 진동
// -------------------------------
void setBad() {
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, HIGH);

  unsigned long now = millis();

  // 0.3초마다 반복 경고
  if (now - lastActionTime >= 300) {
    lastActionTime = now;
    badToggle = !badToggle;

    if (badToggle) {
      tone(BUZZER_PIN, 2200);
      analogWrite(MOTOR_PIN, 255);   // 강한 진동
    } else {
      noTone(BUZZER_PIN);
      analogWrite(MOTOR_PIN, 0);
    }
  }
}
