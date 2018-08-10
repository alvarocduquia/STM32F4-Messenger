#include "stm32f4xx.h"

#include "mtrix.h"
#include "alphabet.h"

char* str = "DIGITAL? EVERY IDIOT CAN COUNT TO ONE.";

int main(void) {

	RCC->AHB1ENR |= 0x87;		// PORTC
	RCC->APB2ENR |= 0x60000;	// TIM9, TIM10

	GPIOC->MODER |= 0x155541;
	GPIOC->OTYPER |= 0x7F8;

	MTRIX_sync();

	TIM10->PSC = 99;
	TIM10->ARR = 119;
	TIM10->CR1 |= 0x1;			// 1.25ms

	TIM11->PSC = 999;
	TIM11->ARR = 999;
	TIM11->CR1 |= 0x1;			// 16Hz

	int encoded[strlen(str)];

	ALPHABET_encode(str, encoded);

	int len = MTRIX_WIDTH + (strlen(str) - 1);

	for(int i = 0; i < strlen(str); i++) {
		len += (encoded[i] == -1) ? 3 : CHARACTER_HEIGHT;
	}

	char code[len][CHARACTER_HEIGHT];

	for(int i = 0; i < len; i++) {
		for(int j = 0; j < CHARACTER_HEIGHT; j++) {
			code[i][j] = 0x0;
		}
	}

	int x = MTRIX_WIDTH - 1;

	for(int i = 0; i < strlen(str); i++) {
		if(encoded[i] == -1) {
			x += 3;

			continue;
		}

		for(int m = 0; m < CHARACTER_WIDTH; m++) {
			for(int k = 0; k < CHARACTER_HEIGHT; k++) {
				if(ALPHABET[encoded[i]][k][m]) {
					code[x][k] = 0x1;
				}
			}

			x++;
		}

		x++;
	}

	int shift = 0;

	while(1) {
		if(TIM11->SR & 0x1) {
			TIM11->SR &= ~(0x1);

			for(int i = 0; i < MTRIX_WIDTH; i++) {
				for(int j = 0; j < CHARACTER_HEIGHT; j++) {
					if((shift + i) < len) {
						MTRIX_DATA[i][j] = code[i + shift][j];
					} else {
						MTRIX_DATA[i][j] = 0x0;
					}
				}
			}

			if(++shift >= len) { shift = 0; }
		}

		if(TIM10->SR & 0x1) {
			TIM10->SR &= ~(0x1);

			MTRIX_multiplex();
		}
	}

}
