# STM32 서보모터 제어 - 아두이노 테스트 버전

## 🎯 목적

STM32로 구현할 서보모터 제어 시스템을 **아두이노로 먼저 테스트**하기 위한 프로젝트입니다.

- 보드가 저렴함 (Arduino Uno: 1-2만원)
- 즉시 테스트 가능
- 로직 검증 완벽
- STM32로 쉽게 포팅 가능

---

## 📁 파일 구성

```
arduino_servo_template/
├── servo_control.ino          # 메인 파일
├── pulse_control.h            # 펄스 생성 (헤더)
├── pulse_control.cpp          # 펄스 생성 (구현)
├── encoder_reader.h           # 엔코더 읽기 (헤더)
├── encoder_reader.cpp         # 엔코더 읽기 (구현)
├── position_control.h         # 위치 제어 (헤더)
├── position_control.cpp       # 위치 제어 (구현)
├── README.md                  # 이 파일
└── WIRING.md                  # 배선 가이드
```

---

## 🔌 필요한 하드웨어

### 최소 구성 (개념 테스트)

```
✅ Arduino Uno (또는 호환 보드)
✅ USB 케이블
✅ 점퍼 와이어
✅ LED 2개 (펄스 확인용)
✅ 저항 220Ω × 2개
```

### 완전 구성 (실제 테스트)

```
✅ Arduino Uno
✅ L7 서보 드라이브 + 모터
✅ 24V 전원
✅ 릴레이 모듈 × 2개 (5V)
✅ 점퍼 와이어
```

---

## 🔧 배선

### 기본 테스트 (LED만)

```
Arduino    →    LED
━━━━━━━━━━━━━━━━━━━━━━
D2 (펄스+) → LED1 (220Ω) → GND
D3 (펄스-) → LED2 (220Ω) → GND
D4 (ENC_A) → (나중에 연결)
D5 (ENC_B) → (나중에 연결)
```

### 실제 서보 연결

```
Arduino    →    릴레이    →    L7 서보
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
D2         → Relay1 IN
            Relay1 COM ← 24V
            Relay1 NO  → Pin 8 (PF+)

D3         → Relay2 IN
            Relay2 COM ← 24V
            Relay2 NO  → Pin 9 (PR+)

D4         ← Pin 32 (AO) (라인리시버 필요)
D5         ← Pin 30 (BO) (라인리시버 필요)

D6         → SVON 릴레이
D7         → EMG 릴레이
```

자세한 배선은 `WIRING.md` 참조!

---

## 💻 사용 방법

### 1. 아두이노 IDE 설정

```
1. Arduino IDE 열기
2. File → Open
3. servo_control.ino 선택
4. Tools → Board → Arduino Uno
5. Tools → Port → (자동 감지된 포트)
```

### 2. 업로드

```
1. 업로드 버튼 (→) 클릭
2. "Done uploading" 메시지 확인
```

### 3. 시리얼 모니터

```
1. Tools → Serial Monitor
2. Baud rate: 115200
3. Both NL & CR 선택
```

---

## 🎮 시리얼 명령어

시리얼 모니터에서 명령어 입력:

```
help           도움말
status         현재 상태
reset          시스템 리셋
demo           자동 데모 실행
stop           비상 정지
goto [각도]    특정 각도로 이동

예시:
> goto 30      (30도로 이동)
> goto -20     (-20도로 이동)
> goto 0       (중앙 복귀)
```

---

## 📊 자동 데모

프로그램 시작 5초 후 자동으로 데모 실행:

```
1. 오른쪽 30도로 이동
2. 중앙(0도) 복귀
3. 왼쪽 -30도로 이동
4. 최종 중앙 복귀
```

시리얼 모니터에서 실시간 상태 확인 가능!

---

## 🔍 테스트 시나리오

### 테스트 1: LED로 펄스 확인

```
목적: 펄스 생성 확인

1. LED만 연결 (서보 없이)
2. 업로드
3. LED 깜빡임 확인
   - D2 LED: 정방향 펄스
   - D3 LED: 역방향 펄스
```

### 테스트 2: 엔코더 읽기

```
목적: 엔코더 입력 확인

1. 엔코더 A, B 연결
2. 시리얼 모니터 열기
3. 모터 수동 회전
4. 카운트 변화 확인
```

### 테스트 3: 위치 제어

```
목적: 전체 시스템 확인

1. 서보 완전 연결
2. 자동 데모 실행
3. 목표 각도로 이동 확인
```

---

## 🚀 STM32로 포팅하기

아두이노로 테스트 완료 후:

### 변경 사항

```
아두이노                     STM32
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
digitalWrite()        →    HAL_GPIO_WritePin()
digitalRead()         →    HAL_GPIO_ReadPin()
pinMode()             →    MX_GPIO_Init() (CubeMX)
micros()              →    HAL_GetTick() * 1000
delay()               →    HAL_Delay()

펄스 생성 (software)  →    TIM1 PWM (hardware)
엔코더 읽기 (polling) →    TIM2 Encoder Mode
```

### 포팅 순서

```
1. STM32CubeMX로 프로젝트 생성
2. TIM1, TIM2, GPIO 설정
3. .h 파일 복사 (Core/Inc/)
4. .cpp → .c 변환 및 복사 (Core/Src/)
5. Arduino 함수 → HAL 함수로 교체
6. 컴파일 & 테스트
```

자세한 포팅 가이드는 별도 문서 참조!

---

## ⚠️ 주의사항

### 하드웨어

```
❌ 아두이노에 24V 직접 연결 금지!
   → 반드시 릴레이 사용

❌ 전원 극성 확인!
   → GND는 모두 공통 연결

❌ 엔코더 A, B 직접 연결 주의
   → 5V 레벨로 변환 필요
```

### 소프트웨어

```
❌ loop()에서 delay() 많이 사용 금지
   → update() 함수 주기적 호출

❌ 시리얼 출력 너무 많이 하지 말 것
   → 성능 저하

✅ 주기적으로 상태 확인
✅ 비상 정지 기능 테스트
```

---

## 🐛 트러블슈팅

### 문제: 컴파일 에러

```
증상: "was not declared in this scope"

해결:
1. 모든 .h, .cpp 파일이 같은 폴더에 있는지 확인
2. Arduino IDE 재시작
3. Verify (✓) 버튼으로 문법 확인
```

### 문제: 업로드 안됨

```
증상: "avrdude: stk500_recv(): programmer is not responding"

해결:
1. USB 케이블 재연결
2. 다른 USB 포트 시도
3. Tools → Port 확인
4. 아두이노 리셋 버튼 누르고 재시도
```

### 문제: 시리얼 모니터 안 열림

```
해결:
1. Baud rate 115200 확인
2. 다른 프로그램에서 포트 사용 중 확인
3. Arduino IDE 재시작
```

---

## 📚 참고 자료

- [Arduino 공식 문서](https://www.arduino.cc/reference/ko/)
- [펄스 제어 원리](../docs/pulse_control_theory.md)
- [엔코더 읽기 원리](../docs/encoder_theory.md)
- [STM32 포팅 가이드](../docs/stm32_porting.md)

---

## 🎯 다음 단계

1. ✅ LED로 펄스 확인
2. ✅ 엔코더 읽기 테스트
3. ✅ 위치 제어 확인
4. → STM32CubeIDE로 포팅
5. → 실제 조향 시스템 통합

---

**즐거운 코딩 되세요! 🚀**

문제 있으면 언제든 질문하세요! 😊
