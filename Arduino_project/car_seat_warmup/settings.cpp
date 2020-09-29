#include "Arduino.h"
#include <EEPROM.h>
#include "settings.h"

//  ============ TSettings =============
TSettings::TSettings(int addr_Hvalue, int addr_Hduration) {
	EEPROMaddress_heatValue = addr_Hvalue;
	EEPROMaddress_heatDuration = addr_Hduration;
}

void TSettings::init(const int HEAT_DUR[4]) {
	heatValue = readEEPROM(EEPROMaddress_heatValue);
  heatDuration = readEEPROM(EEPROMaddress_heatDuration);
  ROMheatValue = heatValue;
  ROMheatDuration = heatDuration;
  heatDurationSeconds = HEAT_DUR[heatDuration-1];
}

void TSettings::inc_heatValue() {
  heatValue = (heatValue % 4) + 1;
  _writeEEPROMtimer_heatValue.restart();
}

void TSettings::inc_heatDuration() {
  heatDuration = (heatDuration % 4) + 1;
  _writeEEPROMtimer_heatDuration.restart();
}

bool TSettings::update_EEPROM() {
  if (_writeEEPROMtimer_heatValue.isRunning)
    {
      _writeEEPROMtimer_heatValue.check();
      if (_writeEEPROMtimer_heatValue.fired() && (heatValue != ROMheatValue))
        { EEPROM.write(EEPROMaddress_heatValue, heatValue);
          ROMheatValue = heatValue;
          return true;                }
    }
  if (_writeEEPROMtimer_heatDuration.isRunning)
    {
      _writeEEPROMtimer_heatDuration.check();
      if (_writeEEPROMtimer_heatDuration.fired() && (heatDuration != ROMheatDuration))
        { EEPROM.write(EEPROMaddress_heatDuration, heatDuration);
          ROMheatDuration = heatDuration;
          return true;                }
    }
  return false;
}

//  ======== Вспомогат. функции ========
byte readEEPROM(int address) {
  byte value = EEPROM.read(address);
  if (value==0 || value >= 5)
    { value = 1;
      EEPROM.write(address, value);   }
  return value;
}
