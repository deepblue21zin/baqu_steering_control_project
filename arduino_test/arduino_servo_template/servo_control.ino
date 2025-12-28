/*
 * STM32 서보모터 제어 - 아두이노 테스트 버전
 * 
 * 핀 연결:
 * - D2: 펄스 정방향 (PF+)
 * - D3: 펄스 역방향 (PR+)
 * - D4: 엔코더 A상
 * - D5: 엔코더 B상
 * - D6: SVON (릴레이)
 * - D7: EMG (릴레이)
 * - D13: LED (상태 표시)
 * 
 * 시리얼 모니터: 115200 baud
 */

#include "pulse_control.h"
#include "encoder_reader.h"
#include "position_control.h"

// ============================================
// 핀 정의
// ============================================
#define PIN_PULSE_FWD    2    // 정방향 펄스
#define PIN_PULSE_REV    3    // 역방향 펄스
#define PIN_ENC_A        4    // 엔코더 A상
#define PIN_ENC_B        5    // 엔코더 B상
#define PIN_SVON         6    // 서보 ON
#define PIN_EMG          7    // 비상 정지 해제
#define PIN_LED          13   // 상태 LED

// ============================================
// 전역 객체
// ============================================
PulseControl pulse;
EncoderReader encoder;
PositionControl posControl;

// ============================================
// 시스템 상태
// ============================================
enum SystemState {
  SYS_INIT,
  SYS_IDLE,
  SYS_RUNNING,
  SYS_ERROR
};

SystemState sysState = SYS_INIT;

// ============================================
// Setup
// ============================================
void setup() {
  // 시리얼 통신 시작
  Serial.begin(115200);
  delay(1000);
  
  Serial.println(F("\n========================================"));
  Serial.println(F("   STM32 서보 제어 - 아두이노 테스트"));
  Serial.println(F("========================================\n"));
  
  // 핀 모드 설정
  pinMode(PIN_PULSE_FWD, OUTPUT);
  pinMode(PIN_PULSE_REV, OUTPUT);
  pinMode(PIN_ENC_A, INPUT_PULLUP);
  pinMode(PIN_ENC_B, INPUT_PULLUP);
  pinMode(PIN_SVON, OUTPUT);
  pinMode(PIN_EMG, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  
  // 초기 상태
  digitalWrite(PIN_PULSE_FWD, LOW);
  digitalWrite(PIN_PULSE_REV, LOW);
  digitalWrite(PIN_SVON, LOW);
  digitalWrite(PIN_EMG, HIGH);  // EMG는 HIGH = 정상
  digitalWrite(PIN_LED, LOW);
  
  Serial.println(F("[1/4] 핀 초기화 완료"));
  
  // 모듈 초기화
  pulse.init(PIN_PULSE_FWD, PIN_PULSE_REV);
  Serial.println(F("[2/4] 펄스 모듈 초기화 완료"));
  
  encoder.init(PIN_ENC_A, PIN_ENC_B);
  Serial.println(F("[3/4] 엔코더 모듈 초기화 완료"));
  
  posControl.init(&pulse, &encoder);
  Serial.println(F("[4/4] 위치 제어 모듈 초기화 완료\n"));
  
  // 서보 활성화
  digitalWrite(PIN_SVON, HIGH);
  Serial.println(F("✓ 서보 활성화 (SVON = HIGH)"));
  Serial.println(F("✓ 비상정지 해제 (EMG = HIGH)\n"));
  
  delay(500);
  
  // 시스템 준비 완료
  sysState = SYS_IDLE;
  Serial.println(F("========================================"));
  Serial.println(F("   시스템 준비 완료!"));
  Serial.println(F("========================================\n"));
  
  // 사용 방법 출력
  printHelp();
  
  // 데모 시작
  Serial.println(F("\n자동 데모를 5초 후 시작합니다...\n"));
  delay(5000);
  
  runDemo();
}

// ============================================
// Loop
// ============================================
void loop() {
  // 엔코더 업데이트
  encoder.update();
  
  // 위치 제어 업데이트
  posControl.update();
  
  // 상태 LED 깜빡임
  static unsigned long lastBlink = 0;
  if (millis() - lastBlink > 500) {
    digitalWrite(PIN_LED, !digitalRead(PIN_LED));
    lastBlink = millis();
  }
  
  // 시리얼 명령어 처리
  handleSerialCommand();
  
  delay(10);
}

// ============================================
// 데모 함수
// ============================================
void runDemo() {
  Serial.println(F("========================================"));
  Serial.println(F("   자동 데모 시작"));
  Serial.println(F("========================================\n"));
  
  sysState = SYS_RUNNING;
  
  // 시나리오 1: 오른쪽 30도
  Serial.println(F("[시나리오 1] 오른쪽 30도로 이동"));
  posControl.setTargetAngle(30.0);
  
  while (posControl.getState() != CTRL_ARRIVED) {
    encoder.update();
    posControl.update();
    
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 200) {
      printStatus();
      lastPrint = millis();
    }
    
    delay(10);
  }
  
  Serial.println(F("✓ 도착!\n"));
  delay(2000);
  
  // 시나리오 2: 중앙 복귀
  Serial.println(F("[시나리오 2] 중앙(0도)으로 복귀"));
  posControl.setTargetAngle(0.0);
  
  while (posControl.getState() != CTRL_ARRIVED) {
    encoder.update();
    posControl.update();
    delay(10);
  }
  
  Serial.println(F("✓ 도착!\n"));
  delay(2000);
  
  // 시나리오 3: 왼쪽 30도
  Serial.println(F("[시나리오 3] 왼쪽 -30도로 이동"));
  posControl.setTargetAngle(-30.0);
  
  while (posControl.getState() != CTRL_ARRIVED) {
    encoder.update();
    posControl.update();
    delay(10);
  }
  
  Serial.println(F("✓ 도착!\n"));
  delay(2000);
  
  // 시나리오 4: 중앙 복귀
  Serial.println(F("[시나리오 4] 최종 중앙 복귀"));
  posControl.setTargetAngle(0.0);
  
  while (posControl.getState() != CTRL_ARRIVED) {
    encoder.update();
    posControl.update();
    delay(10);
  }
  
  Serial.println(F("✓ 도착!\n"));
  
  Serial.println(F("========================================"));
  Serial.println(F("   데모 완료!"));
  Serial.println(F("========================================\n"));
  
  sysState = SYS_IDLE;
  
  printHelp();
}

// ============================================
// 시리얼 명령어 처리
// ============================================
void handleSerialCommand() {
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    
    if (cmd.equalsIgnoreCase("help") || cmd.equalsIgnoreCase("h")) {
      printHelp();
    }
    else if (cmd.equalsIgnoreCase("status") || cmd.equalsIgnoreCase("s")) {
      printStatus();
    }
    else if (cmd.equalsIgnoreCase("reset") || cmd.equalsIgnoreCase("r")) {
      resetSystem();
    }
    else if (cmd.equalsIgnoreCase("demo") || cmd.equalsIgnoreCase("d")) {
      runDemo();
    }
    else if (cmd.equalsIgnoreCase("stop")) {
      emergencyStop();
    }
    else if (cmd.startsWith("goto ") || cmd.startsWith("g ")) {
      // 각도 추출
      int spaceIndex = cmd.indexOf(' ');
      if (spaceIndex > 0) {
        float angle = cmd.substring(spaceIndex + 1).toFloat();
        
        if (angle >= -45.0 && angle <= 45.0) {
          Serial.print(F("목표 각도: "));
          Serial.print(angle);
          Serial.println(F("도"));
          
          posControl.setTargetAngle(angle);
          sysState = SYS_RUNNING;
        } else {
          Serial.println(F("⚠ 오류: 각도 범위 -45 ~ +45"));
        }
      }
    }
    else if (cmd.length() > 0) {
      Serial.println(F("⚠ 알 수 없는 명령어. 'help' 입력"));
    }
  }
}

// ============================================
// 도움말 출력
// ============================================
void printHelp() {
  Serial.println(F("========================================"));
  Serial.println(F("   사용 가능한 명령어"));
  Serial.println(F("========================================"));
  Serial.println(F("help 또는 h       : 도움말"));
  Serial.println(F("status 또는 s     : 현재 상태"));
  Serial.println(F("reset 또는 r      : 시스템 리셋"));
  Serial.println(F("demo 또는 d       : 자동 데모"));
  Serial.println(F("stop              : 비상 정지"));
  Serial.println(F("goto [각도]       : 특정 각도로 이동"));
  Serial.println(F("  예: goto 30     (30도로 이동)"));
  Serial.println(F("  예: goto -20    (-20도로 이동)"));
  Serial.println(F("========================================\n"));
}

// ============================================
// 상태 출력
// ============================================
void printStatus() {
  Serial.print(F("목표: "));
  Serial.print(posControl.getTargetAngle(), 1);
  Serial.print(F("° | 현재: "));
  Serial.print(posControl.getCurrentAngle(), 1);
  Serial.print(F("° | 오차: "));
  Serial.print(posControl.getTargetAngle() - posControl.getCurrentAngle(), 1);
  Serial.print(F("° | 상태: "));
  
  switch (posControl.getState()) {
    case CTRL_IDLE:
      Serial.print(F("대기"));
      break;
    case CTRL_MOVING:
      Serial.print(F("이동중"));
      break;
    case CTRL_ARRIVED:
      Serial.print(F("도착"));
      break;
    case CTRL_ERROR:
      Serial.print(F("에러"));
      break;
  }
  
  Serial.print(F(" | 카운트: "));
  Serial.println(encoder.getCount());
}

// ============================================
// 시스템 리셋
// ============================================
void resetSystem() {
  Serial.println(F("\n시스템 리셋..."));
  
  pulse.stop();
  encoder.reset();
  posControl.setTargetAngle(0.0);
  
  sysState = SYS_IDLE;
  
  Serial.println(F("✓ 리셋 완료\n"));
}

// ============================================
// 비상 정지
// ============================================
void emergencyStop() {
  Serial.println(F("\n⚠️ 비상 정지!"));
  
  pulse.stop();
  digitalWrite(PIN_EMG, LOW);
  
  sysState = SYS_ERROR;
  
  delay(1000);
  
  digitalWrite(PIN_EMG, HIGH);
  sysState = SYS_IDLE;
  
  Serial.println(F("✓ 비상 정지 해제\n"));
}
