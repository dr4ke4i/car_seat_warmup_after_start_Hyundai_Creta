#include "Arduino.h"
#include "helpers.h"

//  ============ Таймер =============
TTimer::TTimer(unsigned long new_delay, bool initial_state)
  {
    delay = new_delay;
    _millis = millis();
    _bFired = false;
    isRunning = initial_state;
  }

bool TTimer::check() {
  if ((isRunning==true) && (millis() - _millis >= delay))
    {
      isRunning = false;
      _bFired = true;    
    }
  return isRunning;
}

bool TTimer::fired() {
  if (_bFired==true)
  {
    _bFired = false;
    return true;
  }
  return false;
}

void TTimer::setDelay(unsigned long new_delay) {
  delay = new_delay;
}

void TTimer::restart() {
  _millis = millis();
  isRunning = true;
}

void TTimer::stop() {
  isRunning = false;
}

void TTimer::LOOP() {
  if (check()!=true) {
    restart();
  }
}

//  ============ Кнопки и дискретные сигналы =============
TDiscrete::TDiscrete(uint8_t new_pin, unsigned long debounce_timer, unsigned long longstate_timer)
{
  _pin = new_pin;
  _debounce = debounce_timer;
  _longState = longstate_timer;
  _prev_state = ( digitalRead(_pin) == HIGH ) ? true : false;
  bIsHigh = _prev_state;
  _debounceTimer.setDelay(_debounce);
  _longStateTimer.setDelay(_longState);
}

void TDiscrete::init(uint8_t input_mode)
{
  pinMode(_pin, input_mode);
  _prev_state = ( digitalRead(_pin) == HIGH ) ? true : false;
  bIsHigh = _prev_state;
}

bool TDiscrete::bHasRisen()
{
  return bRisen;
}

bool TDiscrete::bHasRisenLong()
{
  return bRisenLong;
}

bool TDiscrete::bHasFallen()
{
  return bFallen;
}

bool TDiscrete::bHasFallenLong()
{
  return bFallenLong;
}

bool TDiscrete::isHigh()
{
  return bIsHigh;
}

bool TDiscrete::isLow()
{
  return (!bIsHigh);
}

void TDiscrete::LOOP()
{
  bool _current_state = ( digitalRead(_pin) == HIGH ) ? true : false;
  bRisen = false;
  bFallen = false;
  bRisenLong = false;
  bFallenLong = false;
  _debounceTimer.check();
  if (_debounceTimer.isRunning) {
    if (bIsHigh)
    {
      if ((_prev_state==false) && (_current_state==true))   _debounceTimer.restart();
    }
    else
    {
      if ((_prev_state==true) && (_current_state==false))   _debounceTimer.restart();
    }
  }
  else
  {
    if ((bIsHigh==true) && (_current_state==false))
    {
      bFallen = true;
      bIsHigh = false;
      _debounceTimer.restart();
      _longStateTimer.restart();
    }
    else if ((bIsHigh==false) && (_current_state==true))
    {
      bRisen = true;
      bIsHigh = true;
      _debounceTimer.restart();      
      _longStateTimer.restart();
    }
  }
  _prev_state = _current_state;
  if (_longStateTimer.isRunning)
    {
      _longStateTimer.check();
      if (_longStateTimer.fired())
        { bRisenLong = bIsHigh; bFallenLong = !bIsHigh; }
    }
}
