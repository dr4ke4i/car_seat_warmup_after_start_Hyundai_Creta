#ifndef HELPERS_H_INCLUDED
#define HELPERS_H_INCLUDED

#include "Arduino.h"

//  ============ Таймер =============
struct TTimer {
  private:
    unsigned long _millis = 0;
    bool _bFired = false;
  public:
    unsigned long delay;
    bool isRunning;
  
  TTimer(unsigned long new_delay,  bool initial_state);
  bool check();
  bool fired();
  void setDelay(unsigned long new_delay);
  void restart();
  void stop();
  void LOOP();
};

//  ============ Кнопки и дискретные сигналы =============
struct TDiscrete {
  private:
    bool _prev_state = false;
    unsigned long _debounce = 10;
    unsigned long _longState = 1000;
    uint8_t _pin = 255;
    TTimer _debounceTimer = TTimer(_debounce, false);
    TTimer _longStateTimer = TTimer(_longState, false);
  public:
    bool bIsHigh = false;
    bool bRisen = false;
    bool bFallen = false;
    bool bRisenLong = false;
    bool bFallenLong = false;

  TDiscrete(uint8_t new_pin, unsigned long debounce_timer = 10, unsigned long longstate_timer = 1000);
  void init(uint8_t input_mode);
  bool bHasRisen();
  bool bHasRisenLong();
  bool bHasFallen();
  bool bHasFallenLong();
  bool isHigh();
  bool isLow();
  void LOOP();
};

#endif
