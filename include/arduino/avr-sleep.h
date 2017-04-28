#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>


void initSleep(uint8_t flags) {
}

// precondition: interval.next != NULL

void doSleep() {
  uint8_t secs, prescale;
  unsigned long now, wait;

  // precheck before we shut things down, 2s minimum sleep
  now = currentMillis();
  wait = interval.next->waitTime(now);
  if (wait < 2000) return;
  
  sleepDevices();

  // re-calculate wait after shutting down devices
  now = currentMillis();
  wait = interval.next->waitTime(now);
  secs = wait / 1000;

  if (secs >= 1) {
    if (secs >= 8) {
      secs = 8;
      prescale = 1<<WDP3 | 1<<WDP0;
    } else if (secs >= 4) {
      secs = 4;
      prescale = 1<<WDP3;
    } else if (secs >= 2) {
      secs = 2;
      prescale = 1<<WDP2 | 1<<WDP1 | 1<<WDP0;
    } else {
      secs = 1;
      prescale = 1<<WDP2 | 1<<WDP1;
    }

    cli();
    // Clear reset
    MCUSR &= ~(1<<WDRF);
    // This must happen in 4 cycles: set 
    WDTCSR |= (1<<WDCE) | (1<<WDE);
    WDTCSR = prescale;
    WDTCSR |= (1<<WDIE);
    sei();
    
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_mode();
    sleep_disable();
    milliOffset += secs * 1000;
  }

  wakeDevices();
}

ISR(WDT_vect) {
}
