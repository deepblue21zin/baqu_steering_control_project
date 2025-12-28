/*
 * pulse_control.cpp
 * 
 * 펄스 생성 구현
 */

#include "pulse_control.h"

PulseControl::PulseControl() {
  pulseCount = 0;
  pulseTarget = 0;
  state = PULSE_IDLE;
  pulseInterval = 10;
  lastPulseTime = 0;
  pulseHigh = false;
}

void PulseControl::init(uint8_t fwdPin, uint8_t revPin) {
  pinForward = fwdPin;
  pinReverse = revPin;
  
  pinMode(pinForward, OUTPUT);
  pinMode(pinReverse, OUTPUT);
  
  digitalWrite(pinForward, LOW);
  digitalWrite(pinReverse, LOW);
  
  state = PULSE_IDLE;
}

void PulseControl::start(PulseDirection dir, uint32_t numPulses, uint32_t freqHz) {
  stop();
  
  direction = dir;
  pulseCount = 0;
  pulseTarget = numPulses;
  
  // 주파수 → 인터벌 변환
  // freqHz = 100000 → 인터벌 = 5us (반주기)
  pulseInterval = 1000000UL / (freqHz * 2);
  
  if (pulseInterval < 1) {
    pulseInterval = 1;
  }
  
  state = PULSE_RUNNING;
  lastPulseTime = micros();
  pulseHigh = false;
}

void PulseControl::stop() {
  digitalWrite(pinForward, LOW);
  digitalWrite(pinReverse, LOW);
  
  state = PULSE_IDLE;
  pulseHigh = false;
}

void PulseControl::update() {
  if (state != PULSE_RUNNING) {
    return;
  }
  
  unsigned long now = micros();
  
  if (now - lastPulseTime >= pulseInterval) {
    lastPulseTime = now;
    
    if (pulseHigh) {
      // HIGH → LOW
      if (direction == PULSE_DIR_FORWARD) {
        digitalWrite(pinForward, LOW);
      } else {
        digitalWrite(pinReverse, LOW);
      }
      
      pulseHigh = false;
      pulseCount++;
      
      // 목표 도달 확인
      if (pulseCount >= pulseTarget) {
        stop();
        state = PULSE_COMPLETE;
      }
    } else {
      // LOW → HIGH
      if (direction == PULSE_DIR_FORWARD) {
        digitalWrite(pinForward, HIGH);
      } else {
        digitalWrite(pinReverse, HIGH);
      }
      
      pulseHigh = true;
    }
  }
}

PulseState PulseControl::getState() {
  return state;
}

uint32_t PulseControl::getCount() {
  return pulseCount;
}
