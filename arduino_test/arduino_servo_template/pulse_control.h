/*
 * pulse_control.h
 * 
 * 펄스 생성 모듈 (아두이노 버전)
 */

#ifndef PULSE_CONTROL_H
#define PULSE_CONTROL_H

#include <Arduino.h>

enum PulseDirection {
  PULSE_DIR_FORWARD = 0,
  PULSE_DIR_REVERSE = 1
};

enum PulseState {
  PULSE_IDLE = 0,
  PULSE_RUNNING = 1,
  PULSE_COMPLETE = 2
};

class PulseControl {
private:
  uint8_t pinForward;
  uint8_t pinReverse;
  
  volatile uint32_t pulseCount;
  volatile uint32_t pulseTarget;
  volatile PulseState state;
  PulseDirection direction;
  
  uint32_t pulseInterval;  // 마이크로초
  unsigned long lastPulseTime;
  bool pulseHigh;
  
public:
  PulseControl();
  
  void init(uint8_t fwdPin, uint8_t revPin);
  void start(PulseDirection dir, uint32_t numPulses, uint32_t freqHz);
  void stop();
  
  void update();  // loop()에서 주기적으로 호출
  
  PulseState getState();
  uint32_t getCount();
};

#endif
