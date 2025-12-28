/*
 * encoder_reader.cpp
 * 
 * 엔코더 읽기 구현
 */

#include "encoder_reader.h"

// Lookup Table (상태 천이)
const int8_t EncoderReader::lut[16] = {
    0,  // 00 → 00: 정지
   +1,  // 00 → 01: CW
   -1,  // 00 → 10: CCW
    0,  // 00 → 11: 펄스 손실
   -1,  // 01 → 00: CCW
    0,  // 01 → 01: 정지
    0,  // 01 → 10: 펄스 손실
   +1,  // 01 → 11: CW
   +1,  // 10 → 00: CW
    0,  // 10 → 01: 펄스 손실
    0,  // 10 → 10: 정지
   -1,  // 10 → 11: CCW
    0,  // 11 → 00: 펄스 손실
   -1,  // 11 → 01: CCW
   +1,  // 11 → 10: CW
    0   // 11 → 11: 정지
};

EncoderReader::EncoderReader() {
  count = 0;
  offset = 0;
  prevState = 0;
}

void EncoderReader::init(uint8_t a, uint8_t b) {
  pinA = a;
  pinB = b;
  
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  
  // 초기 상태 읽기
  uint8_t a_val = digitalRead(pinA);
  uint8_t b_val = digitalRead(pinB);
  prevState = (a_val << 1) | b_val;
  
  count = 0;
  offset = 0;
}

void EncoderReader::update() {
  // 현재 상태 읽기
  uint8_t a = digitalRead(pinA);
  uint8_t b = digitalRead(pinB);
  uint8_t nowState = (a << 1) | b;
  
  // Lookup Table 인덱스
  uint8_t index = (prevState << 2) | nowState;
  
  // 카운트 업데이트
  count += lut[index];
  
  // 상태 저장
  prevState = nowState;
}

int32_t EncoderReader::getRawCount() {
  return count;
}

int32_t EncoderReader::getCount() {
  return count - offset;
}

void EncoderReader::reset() {
  offset = count;
}

void EncoderReader::setOffset(int32_t newOffset) {
  offset = newOffset;
}

float EncoderReader::countToAngle(int32_t cnt) {
  return (float)cnt * 360.0 / ENCODER_PPR;
}

int32_t EncoderReader::angleToCount(float angle) {
  return (int32_t)(angle * ENCODER_PPR / 360.0);
}

float EncoderReader::getAngle() {
  return countToAngle(getCount());
}
