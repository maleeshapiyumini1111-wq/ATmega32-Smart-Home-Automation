#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define LCD_DATA_PORT PORTC
#define LCD_DATA_DDR  DDRC
#define RS PC2
#define EN PC3

void LCD_Cmd(unsigned char cmd) {
	LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (cmd & 0xF0);
	LCD_DATA_PORT &= ~(1<<RS);
	LCD_DATA_PORT |= (1<<EN);
	_delay_us(1);
	LCD_DATA_PORT &= ~(1<<EN);
	_delay_us(200);
	LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (cmd << 4);
	LCD_DATA_PORT |= (1<<EN);
	_delay_us(1);
	LCD_DATA_PORT &= ~(1<<EN);
	_delay_ms(2);
}

void LCD_Char(unsigned char data) {
	LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (data & 0xF0);
	LCD_DATA_PORT |= (1<<RS);
	LCD_DATA_PORT |= (1<<EN);
	_delay_us(1);
	LCD_DATA_PORT &= ~(1<<EN);
	_delay_us(200);
	LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (data << 4);
	LCD_DATA_PORT |= (1<<EN);
	_delay_us(1);
	LCD_DATA_PORT &= ~(1<<EN);
	_delay_ms(2);
}

void LCD_Init(void) {
	LCD_DATA_DDR |= 0xFC;
	_delay_ms(20);
	LCD_Cmd(0x02);
	LCD_Cmd(0x28);
	LCD_Cmd(0x0C);
	LCD_Cmd(0x06);
	LCD_Cmd(0x01);
	_delay_ms(2);
}

void LCD_String(const char *str) {
	while(*str) LCD_Char(*str++);
}

void LCD_SetCursor(char row, char pos) {
	if (row == 1) LCD_Cmd((pos & 0x0F) | 0x80);
	else if (row == 2) LCD_Cmd((pos & 0x0F) | 0xC0);
}

void ADC_Init() {
	DDRA = 0x00;
	ADCSRA = (1<<ADEN) | (1<<ADPS1) | (1<<ADPS0);
}

uint16_t ADC_Read(uint8_t ch) {
	ch &= 0x07;
	ADMUX = (1<<REFS0) | ch;
	ADCSRA |= (1<<ADSC);
	while(!(ADCSRA & (1<<ADIF)));
	ADCSRA |= (1<<ADIF);
	return ADC;
}

int main(void) {
	LCD_Init();
	ADC_Init();
	
	DDRB |= 0x3F;
	DDRD &= ~0x0F;
	PORTD |= (1<<PD1) | (1<<PD3);

	char buffer1[16];
	uint16_t t1, gas, water;
	float light;
	uint8_t last_state = 0;

	LCD_String("ULTIMATE SYSTEM");
	LCD_SetCursor(2, 0);
	LCD_String("INITIALIZING...");
	_delay_ms(2000);
	LCD_Cmd(0x01);

	while (1) {
		t1 = ADC_Read(0) * 0.488;
		light = ADC_Read(1) / 10.23;
		gas = ADC_Read(2);
		water = ADC_Read(3) / 10;

		sprintf(buffer1, "T:%dC L:%.0f%% W:%d%%  ", t1, light, water);
		LCD_SetCursor(1, 0);
		LCD_String(buffer1);

		if (light < 30) {
			PORTB |= (1<<PB5);
			} else {
			PORTB &= ~(1<<PB5);
		}

		if (t1 > 35) {
			if (last_state != 7) { LCD_Cmd(0x01); last_state = 7; }
			LCD_SetCursor(2, 0);
			LCD_String("TEMP HIGH: FAN  ");
			PORTB = (PORTB & 0xC0) | (1<<PB4) | (PORTB & (1<<PB5));
		}
		else if (PIND & (1<<PD0)) {
			if (last_state != 1) { LCD_Cmd(0x01); last_state = 1; }
			LCD_SetCursor(2, 0);
			LCD_String("CRITICAL: MOTION!");
			PORTB = (PORTB & 0xC0) | (1<<PB0) | (PORTB & (1<<PB5));
		}
		else if (!(PIND & (1<<PD1))) {
			if (last_state != 2) { LCD_Cmd(0x01); last_state = 2; }
			LCD_SetCursor(2, 0);
			LCD_String("ALERT: DOOR OPEN");
			PORTB = (PORTB & 0xC0) | (1<<PB0) | (PORTB & (1<<PB5));
		}
		else if (PIND & (1<<PD2)) {
			if (last_state != 3) { LCD_Cmd(0x01); last_state = 3; }
			LCD_SetCursor(2, 0);
			LCD_String("BEAM BROKEN!!");
			PORTB = (PORTB & 0xC0) | (1<<PB0) | (PORTB & (1<<PB5));
		}
		else if (gas > 400 || !(PIND & (1<<PD3))) {
			if (last_state != 4) { LCD_Cmd(0x01); last_state = 4; }
			LCD_SetCursor(2, 0);
			LCD_String("FIRE/GAS LEAK!");
			PORTB = (PORTB & 0xC0) | (1<<PB0) | (1<<PB1) | (PORTB & (1<<PB5));
		}
		else if (water < 30) {
			if (last_state != 5) { LCD_Cmd(0x01); last_state = 5; }
			LCD_SetCursor(2, 0);
			LCD_String("WATER LOW: PUMP ");
			PORTB = (PORTB & 0xC0) | (1<<PB2) | (PORTB & (1<<PB5));
		}
		else {
			if (last_state != 6) { LCD_Cmd(0x01); last_state = 6; }
			LCD_SetCursor(2, 0);
			LCD_String("SYS SECURE - OK ");
			PORTB = (PORTB & 0xC0) | (1<<PB3) | (PORTB & (1<<PB5));
		}

		_delay_ms(200);
	}
}