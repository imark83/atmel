#define F_CPU 1000000L

#include <avr/io.h>                    // adding header files
#include <util/delay.h>                // for _delay_ms()


int main(void) {
   DDRB |= (1<<DDB0);                       // setting DDR of PORT B
   while(1) {
       // LED on
       PORTB |= (1<<PB0);            // PCB = High = LED attached on PC0 is ON
       _delay_ms(500);                // wait 500 milliseconds

       //LED off
       PORTB &= ~(1<<PB0);            // PC0 = Low = LED attached on PC0 is OFF
       _delay_ms(500);                // wait 500 milliseconds
   }
}
