#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#define F_CPU 16000000
#include <util/delay.h>

#define C0 0b00011111
#define C1 0b00010000
#define C2 0b00101110
#define C3 0b00111100
#define C4 0b00110001
#define C5 0b00111101
#define C6 0b00111111
#define C7 0b00011000
#define C8 0b00111111
#define C9 0b00111101

#define B0 0b00111110
#define B1 0b00111110
#define B2 0b00111110
#define B3 0b00111110
#define B4 0b00111110
#define B5 0b00011110
#define B6 0b00011110
#define B7 0b00111110
#define B8 0b00111110
#define B9 0b00111110

static const int to_portc[10] =
  {C0, C1, C2, C3, C4, C5, C6, C7, C8, C9};

static const int to_portb[10] =
  {B0, B1, B2, B3, B4, B5, B6, B7, B8, B9};

void draw_int_sec (int);
void uartInit();
int uartReadCo2();

int main() {
  DDRC |= 0b00111111;
  DDRB |= 0b00111110;

  while (1) {
    uartInit();
    int co = uartReadCo2();
    if (co > 0) {
      draw_int_sec(co);
    } else {
      draw_int_sec(9999);
    }
  }
  
  return 0;
}

void draw_int_sec (int n) {
  for (int s = 0; s < 100; s++) {
    int i = 0, m = n;
    PORTB |= 0b00011111;
    for (i = 1; i < 5; i++) {
      if (m > 0) {
        PORTC = to_portc[m % 10];
        PORTB |= to_portb[m % 10];
        PORTB |= 0b00011111;
        PORTB &= ~(1 << i);
        m /= 10;
        _delay_ms(5);
      } else {
        PORTC &= 0b00000000;
        PORTB = 0b00011111;
        PORTB &= ~(1 << i);
        _delay_ms(5);
      }
    }
  }
}

void uartInit() {
  UBRR0H = (uint8_t) (103 >> 8);
  UBRR0L = (uint8_t) (103 & 0xFF);
  UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
  UCSR0C = (0 << USBS0) | (3 << UCSZ00);
}

int uartReadCo2 () {
  uint8_t ret[9];

  while (!(UCSR0A & (1 << UDRE0)));
  UDR0 = 255;
  while (!(UCSR0A & (1 << UDRE0)));
  UDR0 = 1;
  while (!(UCSR0A & (1 << UDRE0)));
  UDR0 = 134; // 0x86
  for (int i = 3; i < 8; i++) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = 0;
  }
  while (!(UCSR0A & (1 << UDRE0)));
  UDR0 = 121; // 0x79

  for (int i = 0; i < 9; i++) {
    while (!(UCSR0A & (1 << RXC0)));
    ret[i] = UDR0;
  }
  if (ret[1] == 134) {
    return ret[2] * 256 + ret[3];
  } else {
    return -1;
  }
}

