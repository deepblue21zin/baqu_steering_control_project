/*
 * position_control.h
 * 
 * 위치 제어 모듈 (아두이노 버전)
 */

#ifndef POSITION_CONTROL_H
#define POSITION_CONTROL_H

#include <Arduino.h>
#include "pulse_control.h"
#include "encoder_reader.h"

#define MAX_STEERING_ANGLE  45.0  // 최대 조향 각도
#define ANGLE_TOLERANCE     0.5   // 도착 허용 오차

enum ControlState {
  CTRL_IDLE = 0,
  CTRL_MOVING = 1,
  CTRL_ARRIVED = 2,
  CTRL_ERROR = 3
};

class PositionControl {
private:
  PulseControl* pulse;
  EncoderReader* encoder;
  
  float targetAngle;
  ControlState state;
  
  uint32_t pulseFreq;  // 펄스 주파수 (Hz)
  
public:
  PositionControl();
  
  void init(PulseControl* pulseCtrl, EncoderReader* enc);
  void update();  // loop()에서 주기적으로 호출
  
  void setTargetAngle(float angle);
  float getTargetAngle();
  float getCurrentAngle();
  ControlState getState();
  
  void homing();
  void emergencyStop();
};

#endif
