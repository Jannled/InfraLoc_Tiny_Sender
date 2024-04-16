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
const uint8_t channel = CHANNEL;
/* --- END CONFIG --- */

const uint8_t channelTOP[4] = {240, 160, 120, 96}; // 20kHz, 30kHz, 40kHz, 50kHz for FREQ = 4.8MHz

void setup()
{
	static_assert(channel < sizeof(channelTOP), "The channel is not in the allowed range!");

	DDRB |= _BV(DDB1); // Set GP1 as Output
	DDRB |= _BV(DDB3); // Set GP3 as Output
	DDRB &= ~_BV(DDB4); // Set GP4 as Input

	// Enter Phase correct PWM Mode with OCRA being the TOP Value
	// WGM00 - WGM02		Fast PWM with OCRA being the TOP Value (Waveform Generator Mode)
	// COM0B1				Set Pin COM0B LOW on Compare Match and HIGH on TOP
	// CS00					Enable Clock (Clock Select)
	TCCR0A = _BV(COM0B1) | _BV(WGM00); // WGM01 left out = Phase Correct
	TCCR0B = _BV(WGM02) | _BV(CS00);

	// Using Phase Correct PWM to half the F_CPU of 9.6MHz, because 4.8MHz calibration is bad
	// 4.8 MHz = 9600000 Hz
	OCR0A = channelTOP[channel]; // 200 // 160
	OCR0B = OCR0A/2;
}

void loop()
{
	PORTB |= _BV(PB3); // Set GP0 to HIGH
	_delay_ms(100);

	for(uint8_t i=0; i<channel; i++)
	{
		PORTB &= ~_BV(PB3); // Set GP0 to LOW
		_delay_ms(100);
		PORTB |= _BV(PB3); // Set GP0 to HIGH
		_delay_ms(100);
	}

	PORTB &= ~_BV(PB3); // Set GP0 to LOW
	_delay_ms(5000);
}

int main()
{
	setup();

	while(1)
		loop();
}