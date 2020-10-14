#ifndef SETTINGS_H_INCLUDED
#define SETTINGS_H_INCLUDED

#include "Arduino.h"
#include "helpers.h"
#include "EEPROM.h"

#define EEPROM_ADDR_HEAT_VALUE        1   // адрес записанной во flash-память силы нагрева (значения: 1='000', 2='I00', 3='II0', 4='III')
#define EEPROM_ADDR_HEAT_DURATION     2   // адрес записанной во flash-память длительности таймера автовыключения обогрева (значения: 1=HEAT_DURATION_1, 2=HEAT_DURATION_2, ...
#define TIMER_ROM_WRITE_DELAY      3000   // задержка перед записью в EEPROM, в миллисекундах. Для продления жизни flash-памяти

//  ============ Настройки =============
struct TSettings {
  private:
    TTimer _writeEEPROMtimer_heatValue = TTimer(TIMER_ROM_WRITE_DELAY, false);
    TTimer _writeEEPROMtimer_heatDuration = TTimer(TIMER_ROM_WRITE_DELAY, false);
  public:
    byte heatValue = 0;
    byte heatDuration = 0;
    byte ROMheatValue = 0;
    byte ROMheatDuration = 0;
    int  heatDurationSeconds = 0;
    int EEPROMaddress_heatValue = 0;
    int EEPROMaddress_heatDuration = 0;

    TSettings(int addr_Hvalue = EEPROM_ADDR_HEAT_VALUE, int addr_Hduration = EEPROM_ADDR_HEAT_DURATION);
    void init(const int HEAT_DUR[4]);
    void inc_heatValue();
    void inc_heatDuration();
    bool update_EEPROM();
};

//  ======== Вспомогат. функции ========
byte readEEPROM(int address);
#endif
