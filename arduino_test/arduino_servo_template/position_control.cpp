/*
 * position_control.cpp
 * 
 * 위치 제어 구현
 */

#include "position_control.h"

PositionControl::PositionControl() {
  targetAngle = 0.0;
  state = CTRL_IDLE;
  pulseFreq = 100000;  // 100kHz (기본값)
}

void PositionControl::init(PulseControl* pulseCtrl, EncoderReader* enc) {
  pulse = pulseCtrl;
  encoder = enc;
  
  targetAngle = 0.0;
  state = CTRL_IDLE;
}

void PositionControl::update() {
  if (state != CTRL_MOVING) {
    return;
  }
  
  // 펄스 업데이트
  pulse->update();
  
  // 현재 각도
  float currentAngle = getCurrentAngle();
  
  // 오차 계산
  float error = targetAngle - currentAngle;
  
  // 도착 확인
  if (abs(error) < ANGLE_TOLERANCE) {
    pulse->stop();
    state = CTRL_ARRIVED;
    return;
  }
  
  // 펄스 실행 중이면 대기
  if (pulse->getState() == PULSE_RUNNING) {
    return;
  }
  
  // 펄스 완료됐으면 다음 펄스
  if (pulse->getState() == PULSE_COMPLETE) {
    // 아직 목표 도달 안했으면 계속
    if (abs(error) >= ANGLE_TOLERANCE) {
      // 방향 결정
      PulseDirection dir = (error > 0) ? PULSE_DIR_FORWARD : PULSE_DIR_REVERSE;
      
      // 필요 펄스 수
      int32_t pulses = encoder->angleToCount(abs(error));
      
      // 최소 펄스 제한
      if (pulses < 10) {
        pulses = 10;
      }
      
      // 최대 한번에 이동할 펄스 제한 (안전)
      if (pulses > 1000) {
        pulses = 1000;
      }
      
      // 펄스 시작
      pulse->start(dir, pulses, pulseFreq);
    }
  }
  
  // 처음 시작할 때
  if (pulse->getState() == PULSE_IDLE) {
    // 방향 결정
    PulseDirection dir = (error > 0) ? PULSE_DIR_FORWARD : PULSE_DIR_REVERSE;
    
    // 필요 펄스 수
    int32_t pulses = encoder->angleToCount(abs(error));
    
    if (pulses < 10) {
      pulses = 10;
    }
    
    if (pulses > 1000) {
      pulses = 1000;
    }
    
    // 펄스 시작
    pulse->start(dir, pulses, pulseFreq);
  }
}

void PositionControl::setTargetAngle(float angle) {
  // 한계 체크
  if (angle > MAX_STEERING_ANGLE) {
    angle = MAX_STEERING_ANGLE;
  } else if (angle < -MAX_STEERING_ANGLE) {
    angle = -MAX_STEERING_ANGLE;
  }
  
  targetAngle = angle;
  state = CTRL_MOVING;
}

float PositionControl::getTargetAngle() {
  return targetAngle;
}

float PositionControl::getCurrentAngle() {
  return encoder->getAngle();
}

ControlState PositionControl::getState() {
  return state;
}

void PositionControl::homing() {
  pulse->stop();
  encoder->reset();
  targetAngle = 0.0;
  state = CTRL_IDLE;
}

void PositionControl::emergencyStop() {
  pulse->stop();
  state = CTRL_ERROR;
}
