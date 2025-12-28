/*
 * encoder_reader.h
 * 
 * 엔코더 읽기 모듈 (아두이노 버전)
 */

#ifndef ENCODER_READER_H
#define ENCODER_READER_H

#include <Arduino.h>

#define ENCODER_PPR  12000.0  // 1회전당 펄스 수

class EncoderReader {
private:
  uint8_t pinA;
  uint8_t pinB;
  
  volatile int32_t count;
  int32_t offset;
  
  uint8_t prevState;
  
  // Lookup table
  static const int8_t lut[16];
  
public:
  EncoderReader();
  
  void init(uint8_t pinA, uint8_t pinB);
  void update();  // loop()에서 주기적으로 호출
  
  int32_t getCount();
  int32_t getRawCount();
  
  void reset();
  void setOffset(int32_t offset);
  
  float countToAngle(int32_t count);
  int32_t angleToCount(float angle);
  float getAngle();
};

#endif
