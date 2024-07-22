#include <stdint.h>

#include <avr/io.h>
#include <util/delay.h>
#include <assert.h>

// 5 PB0 - (MOSI)
// 6 PB1 - LED_Drive (MISO)
// 7 PB2 - (SCK)
// 2 PB3 - LED D9 (GP3)
// 3 PB4 - USR1 Button
// 1 PB5 - (Reset)

// The LFUSE must be manually written, the PlatformIO Command will not work!
// cd ~/.platformio/packages/tool-avrdude
// avrdude -c stk500v1 -P COM10 -b 19200 -p attiny13a -U lfuse:w:0x79:m

/* --- BEGIN CONFIG --- */
const uint8_t channel = CHANNEL - 1;
/* --- END CONFIG --- */

#if F_CPU == 16000000L
const uint8_t channelTOP[4] = {80, 64, 53, 46}; // 12.500kHz, 15.625kHz, 18.750kHz, 21.875kHz for FREQ = 1MHz
#elif F_CPU == 9600000L
const uint8_t channelTOP[4] = {46, 38, 32, 27}; // 12.500kHz, 15.625kHz, 18.750kHz, 21.875kHz for FREQ = 600KHz
#else 
#error "A frequency was specified, that was not considered"
#endif

#define LED_PIN PB4

void setup()
{
	static_assert(channel < sizeof(channelTOP), "The channel is not in the allowed range!");

	DDRB |= _BV(DDB1); // Set GP1 as Output
	DDRB |= _BV(DDB4); // Set GP4 as Output
	//DDRB &= ~_BV(DDB4); // Set GP4 as Input

	// Enter Phase correct PWM Mode with OCRA being the TOP Value
	// WGM00 - WGM02		Fast PWM with OCRA being the TOP Value (Waveform Generator Mode)
	// COM0B1				Set Pin COM0B LOW on Compare Match and HIGH on TOP
	// CS00					Enable Clock (Clock Select)
	TCCR0A = _BV(COM0B1) | _BV(WGM00); // OC0B Pin connected, WGM01 left out = Phase Correct
	TCCR0B = _BV(WGM02) | _BV(CS01); // Clock Divide 8

	// The external clock provides 16MHz reference
	OCR0A = channelTOP[channel];
	OCR0B = OCR0A/2;
}

void loop()
{
	PORTB |= _BV(LED_PIN); // Set GP0 to HIGH
	_delay_ms(100);

	for(uint8_t i=0; i<channel; i++)
	{
		PORTB &= ~_BV(LED_PIN); // Set GP0 to LOW
		_delay_ms(100);
		PORTB |= _BV(LED_PIN); // Set GP0 to HIGH
		_delay_ms(100);
	}

	PORTB &= ~_BV(LED_PIN); // Set GP0 to LOW
	_delay_ms(5000);
}

int main()
{
	setup();

	while(1)
		loop();
}