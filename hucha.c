#define F_CPU 1000000L

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <time.h>


void initADC() {
  ADMUX =
            (1 << ADLAR) |     // left shift result
            (0 << REFS1) |     // Sets ref. voltage to VCC, bit 1
            (0 << REFS0) |     // Sets ref. voltage to VCC, bit 0
            (0 << MUX3)  |     // use ADC1 for input (PB2), MUX bit 3
            (0 << MUX2)  |     // use ADC1 for input (PB2), MUX bit 2
            (0 << MUX1)  |     // use ADC1 for input (PB2), MUX bit 1
            (1 << MUX0);       // use ADC1 for input (PB2), MUX bit 0

  ADCSRA =
            (0 << ADEN)  |     // Not Enable ADC yet
            (0 << ADPS2) |     // set prescaler to 8, bit 2
            (1 << ADPS1) |     // set prescaler to 8, bit 1
            (1 << ADPS0);      // set prescaler to 8, bit 0
}


ISR(PCINT0_vect) {
  // DISABLE PIN CHANGE INTERRUPTS
  GIMSK &= ~(1<<PCIE);                    // disable Pin Change Interrupts
  PCMSK &= ~(1 << PCINT4);                   // disable PB4 as interrupt pin

  // SWITCH ON MIC
  PORTB |= (1 << PB3);
  ADCSRA |= (1<<ADEN);
  //clear clock

  //start clock (scale 1024)
  TCCR1 |= (1 << CS13)|(1 << CS12)|(1 << CS11)|(1 << CS10);
  TCNT1 = 0;
  unsigned char claps = 0;
  while (TCNT1 < 130) {
    ADCSRA |= (1 << ADSC);         // start ADC measurement
    while (ADCSRA & (1 << ADSC)); // wait till conversion complete
    if (ADCH > 127) {
      ++claps;
      PORTB |= (1<<PB0);
      _delay_ms(500);
      PORTB &= ~(1<<PB0);
      TCNT1 = 0;
    }
  }

  // open magic box
  if (claps > 1) {
    PORTB |= (1<<PB1);
    TCNT1 = 0;
    while(TCNT1<240);
    PORTB &= ~(1<<PB1);
  }


  // SWITCH OFF MIC
  PORTB &= ~(1 << PB3);
  // stop timer
  TCCR1 &= ~((1 << CS13)|(1 << CS12)|(1 << CS11)|(1 << CS10));
  // Enable Pin Change Interrupts
  GIMSK |= (1<<PCIE);
  PCMSK |= (1 << PCINT4);                   // Use PB4 as interrupt pin

  // disable ADC
  ADCSRA &= ~(1<<ADEN);

  // KILL PENDING INTERRUPTS
  GIFR &= ~((1<<PCIF)|(1<<INTF0));


}


int main (void) {
  initADC();

  DDRB |= (1 << DD0); 	//LED0
  DDRB |= (1 << DD1); 	//LED1
	DDRB &= ~(1 << DD2); 	//ADC
  DDRB |= (1 << DD3);   // sensor power
  DDRB &= ~(1 << DD4); 	//PCINT4, where a button is attached (pulled up)

  PORTB &= ~(1 << PB0); 	//Make sure the LED is turned off
	PORTB &= ~(1 << PB1); 	//Make sure the LED is turned off
  PORTB &= ~(1 << PB3);
  PORTB |= (1 << PB4); 	//Pull up using software

  cli();					//Deactivate Interrupts as long as I change Bits
  GIMSK |= (1<<PCIE);                    // Enable Pin Change Interrupts
  PCMSK = (1 << PCINT4);                   // Use PB4 as interrupt pin
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // go to deep sleep
  sei();					//Activate Interrupts

	while (1) {
		sleep_mode(); 			//set all the registers and go to sleep
	}

	return 1;
}
