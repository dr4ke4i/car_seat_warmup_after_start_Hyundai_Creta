#include "helpers.h"
#include "settings.h"

#define PIN_CAR_BUTTON                0   // кнопка управления подогревом сидения
#define PIN_LED_BUILTIN               1   // LED для индикации (обратной связи), встроен в плату
#define HEAT_DURATION_1             600   // длительность автовыключения обогрева сидения, в секундах. Уставка 2 = 10 мин.
#define HEAT_DURATION_2             900   // длительность автовыключения обогрева сидения, в секундах. Уставка 3 = 15 мин.
#define HEAT_DURATION_3            1200   // длительность автовыключения обогрева сидения, в секундах. Уставка 4 = 20 мин.
#define HEAT_DURATION_4             300   // длительность автовыключения обогрева сидения, в секундах. Уставка 1 =  5 мин.
#define BUTTON_PUSH_TIME            150   // длительность удерживания кнопки нажатой контроллером, в миллисекундах. Для иммитации нажатия человеком
#define BUTTON_WAIT_TIME            250   // длительность задержки между двумя нажатиями кнопки контроллером, в миллисекундах. Для иммитации нажатия человеком
#define TIMER_LONGPRESS            2000   // сколько миллисекунд нужно удерживать кнопку подогрева для детектирования длительного нажатия
const int HEAT_DUR[]={HEAT_DURATION_4, HEAT_DURATION_3, HEAT_DURATION_2, HEAT_DURATION_1};

TDiscrete car_pin(PIN_CAR_BUTTON, 10, TIMER_LONGPRESS); // второй параметр в конструкторе - таймер защиты от требезга = 10мс 
TSettings s;                                            // содержит настройки, и работает с их чтением и записью в EEPROM
TTimer    cdown_timer(1000,true);                       // таймер посекундного обратного отсчёта, для автоматического выключения обогрева
bool      bStartSequence;                               // если флаг поднят - контроллер только что запущен, нужно выдать серию импульсов прочитанных из EEPROM
bool      bHeatDurationSequence;                        // если флаг поднят - мы в режиме программирования длительности автовыключения подогрева

void makeOnePulse();                                    // иммитирует одно нажатие кнопки подогрева сидения

void setup() {
  pinMode(PIN_LED_BUILTIN, OUTPUT);
  for (int i = 0; i <= 100; i++) { delay(10); analogWrite(PIN_LED_BUILTIN, i*2); }
  digitalWrite(PIN_LED_BUILTIN, HIGH);
  s.init(HEAT_DUR);
  bStartSequence = true;
  bHeatDurationSequence = false;
}

void loop() {
  if (bStartSequence)
    {
      if (s.heatValue > 1)
        {
          int i = (int)s.heatValue - 1;
          while (i > 0)
            { makeOnePulse();  i--;  }
        }
      car_pin.init(INPUT);
      bStartSequence = false;
    }
  else
    {
      if (car_pin.bHasFallenLong())
        {  bHeatDurationSequence = true;
           digitalWrite(PIN_LED_BUILTIN, HIGH); delay(50); }
      if (car_pin.bHasRisen())
        {
          if (!bHeatDurationSequence)
            { 
              if (s.heatDurationSeconds > 0)
                { s.inc_heatValue(); }
            }
          else
            {
              delay(BUTTON_PUSH_TIME + BUTTON_WAIT_TIME);     // задержка чтобы сразу не мешать пользователю
              car_pin.LOOP();                                 // для сброса состояния кнопки (сброс что было длительное нажатие)
              /// 1. Проигрываем анимацию и выставляем индикатор в положение автовыключения длительности нагрева
                byte current_value = (s.heatValue % 4) + 1;
                for (int i = 8; i > 0; i--) { makeOnePulse(); current_value = (current_value % 4) + 1; }
                delay( BUTTON_PUSH_TIME + BUTTON_WAIT_TIME );
                while (current_value != s.heatDuration) { makeOnePulse(); current_value = (current_value % 4) + 1; }
              /// 2. Ожидаем от пользователя: изменения значения автовыключения обогрева коротким нажатием,
              ///    или выхода из режима программирования длительности выключения обогрева длительным нажатием на кнопку
                do
                  {
                    if (car_pin.bHasRisen())
                      { s.inc_heatDuration();
                        current_value = (current_value % 4) + 1;
                        for (int i = 2; i > 0; i--) { digitalWrite(PIN_LED_BUILTIN, LOW); delay(15); digitalWrite(PIN_LED_BUILTIN, HIGH); delay(15); }
                       }
                    if (s.update_EEPROM()) {  digitalWrite(PIN_LED_BUILTIN, LOW); delay(50); digitalWrite(PIN_LED_BUILTIN, HIGH); }
                    car_pin.LOOP();
                   }
                while (!car_pin.bHasFallenLong());
                digitalWrite(PIN_LED_BUILTIN, LOW);
                do { car_pin.LOOP(); } while (!car_pin.bHasRisen());
                delay( BUTTON_PUSH_TIME + BUTTON_WAIT_TIME );
                digitalWrite(PIN_LED_BUILTIN, HIGH);
//                current_value = (current_value % 4) + 1;        // неверно, т.к. после 2 секунд нажатия машина сбрасывает (выключает нагрев)
                current_value = 1;
              /// 3. Проигрываем ещё раз анимацию и восстанавливаем положение индикатора в соответствии с уровнем обогрева
                for (int i = 12; i > 0; i--) { makeOnePulse(); current_value = (current_value % 4) + 1; }
                delay(BUTTON_PUSH_TIME + BUTTON_WAIT_TIME);
                while (current_value != s.heatValue) { makeOnePulse(); current_value = (current_value % 4) + 1; }
                bHeatDurationSequence = false;
            }
        }
      if (s.update_EEPROM()) {  digitalWrite(PIN_LED_BUILTIN, LOW); delay(50); digitalWrite(PIN_LED_BUILTIN, HIGH); }
      car_pin.LOOP();
      digitalWrite(PIN_LED_BUILTIN, car_pin.isHigh());
    }
    
  cdown_timer.check();
  if (cdown_timer.fired())
    { s.heatDurationSeconds--;
      if (s.heatDurationSeconds <= 0) 
        {
          cdown_timer.stop();
          byte current_value = s.heatValue;
          while (current_value != 1) { makeOnePulse(); current_value = (current_value % 4) + 1; }
        }
      else
        { cdown_timer.restart(); }
    }
}

void makeOnePulse() {
  pinMode(PIN_CAR_BUTTON,OUTPUT);
  digitalWrite(PIN_CAR_BUTTON, LOW);
  digitalWrite(PIN_LED_BUILTIN, LOW);
  delay(BUTTON_PUSH_TIME);
  pinMode(PIN_CAR_BUTTON,INPUT);
  digitalWrite(PIN_LED_BUILTIN, HIGH);
  delay(BUTTON_WAIT_TIME);
}
