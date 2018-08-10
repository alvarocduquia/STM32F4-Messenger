// -------- CONFIGURAÇÕES --------

#define MTRIX_WIDTH			16
#define MTRIX_HEIGHT		8

#define MTRIX_REGISTER		GPIOC

#define MTRIX_CLK_PIN		0x1
#define MTRIX_SYNC1_PIN		0x2
#define MTRIX_SYNC2_PIN		0x4

uint16_t MTRIX_ROW_PINS[MTRIX_HEIGHT] = {

		0x8,
		0x10,
		0x20,
		0x40,
		0x80,
		0x100,
		0x200,
		0x400

};

// -------- INTERNO --------

char MTRIX_DATA[MTRIX_WIDTH][MTRIX_HEIGHT] = { 0 };
int MTRIX_COLUMN = 0;

void MTRIX_sync() {

	while(!(MTRIX_REGISTER->IDR & MTRIX_SYNC1_PIN && MTRIX_REGISTER->IDR & MTRIX_SYNC2_PIN)) {
		MTRIX_REGISTER->ODR |= MTRIX_CLK_PIN;

		int t = (5 * 1.6);

		while(t-- > 0) { __asm("nop"); }

		MTRIX_REGISTER->ODR &= ~(MTRIX_CLK_PIN);
	}

}

void MTRIX_multiplex() {

	for(int i = 0; i < MTRIX_HEIGHT; i++) {
		MTRIX_REGISTER->ODR |= MTRIX_ROW_PINS[i];
	}

	MTRIX_REGISTER->ODR |= MTRIX_CLK_PIN;

	if(++MTRIX_COLUMN >= MTRIX_WIDTH) { MTRIX_COLUMN = 0; }

	for(int i = 0; i < MTRIX_HEIGHT; i++) {
		if(MTRIX_DATA[MTRIX_COLUMN][i]) {
			MTRIX_REGISTER->ODR &= ~(MTRIX_ROW_PINS[i]);
		}
	}

	MTRIX_REGISTER->ODR &= ~(MTRIX_CLK_PIN);

}
