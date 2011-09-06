#ifndef KEYB_H_
#define KEYB_H_

#if KEYB == 1

#ifndef GLUE
	#define GLUE(a, b)     a##b
	#define PORT(x)        GLUE(PORT, x)
	#define PIN(x)         GLUE(PIN, x)
	#define DDR(x)         GLUE(DDR, x)
#endif	//GLUE

#define KB_PORT		PORT(KB_PRT)
#define KB_DDR		DDR(KB_PRT)
#define KB_PIN		PIN(KB_PRT)
#define KB_L_PORT	PORT(KB_PRT_LEFT)
#define KB_L_DDR	DDR(KB_PRT_LEFT)
#define KB_L_PIN	PIN(KB_PRT_LEFT)
#define KB_R_PORT	PORT(KB_PRT_RIGHT)
#define KB_R_DDR	DDR(KB_PRT_RIGHT)
#define KB_R_PIN	PIN(KB_PRT_RIGHT)

#define KBLEFT		1
#define KBUP		2
#define KBRIGHT		3
#define KBDOWN		4
#define KBOK		5


void KB_Init(void);

uint8_t KB_getkey(uint8_t wait);

#endif		//KEYB

#endif		//KEYB_H_
