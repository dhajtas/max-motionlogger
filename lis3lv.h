#ifndef LIS3LV_H_
#define LIS3LV_H_

#ifndef GLUE
	#define GLUE(a, b)     a##b
	#define PORT(x)        GLUE(PORT, x)
	#define PIN(x)         GLUE(PIN, x)
	#define DDR(x)         GLUE(DDR, x)
#endif	//GLUE

#define LIS_DECIMATE 		0						//nefunguje dobre...

#define LIS_CS_PORT			PORT(LIS_CTRL_PORT)
#define LIS_CS_DDR			DDR(LIS_CTRL_PORT)
#define LIS_RDY_PORT		PORT(LIS_RDY_P)
#define LIS_RDY_DDR			DDR(LIS_RDY_P)

#define LIS_CS_ASSERT		LIS_CS_PORT&=~(_BV(LIS_CS))
#define LIS_CS_DEASSERT		LIS_CS_PORT|=_BV(LIS_CS)

// register definitions
#define WHO_AM_I			0x0F
#define CTRL1				0x20
#define CTRL2				0x21
#define CTRL3				0x22
#define HP_FILT				0x23
#define STATUS				0x27
#define OUTX_L				0x28
#define OUTY_L				0x2A
#define OUTZ_L				0x2C
#define FF_CFG				0x30
#define FF_SRC				0x31
#define FF_ACK				0x32
#define FF_THS_L			0x34 
#define FF_DUR				0x36
#define DD_CFG				0x38
#define DD_SRC				0x39
#define DD_ACK				0x3A
#define DD_THSI_L			0x3C 
#define DD_THSE_L			0x3E

#define DF_512				0x00
#define DF_128				0x10
#define DF_32				0x20
#define DF_8				0x30

// bit definitions
#define	LIS_PD1		7
#define LIS_PD0		6
#define DF1		5
#define DF0		4
#define ST		3
#define ZEN		2
#define YEN		1
#define XEN		0

#define FS		7
#define BDU		6
#define BLE		5
#define BOOT	4
#define IEN		3
#define DRDY	2
#define SIM		1
#define DAS		0

#define ECK		7
#define HPDD	6
#define HPFF	5
#define FDS 	4
#define CFS1	1
#define CFS0	0

// -------------------------------------------------------------
// -------------- Define structures ----------------------------
// -------------------------------------------------------------

typedef struct XYZ_structure
{
	int16_t	x;
	int16_t y;
	int16_t z;
} XYZ;


// -------------------------------------------------------------
// -------------- Define variables  ----------------------------
// -------------------------------------------------------------

extern volatile XYZ LIS_Data[10];
extern volatile uint8_t LIS_Index;



void LIS_Init_hw(void);

uint8_t LIS_Init(void);

void LIS_Start(void);

void LIS_Stop(void);

//void LIS_read_XYZ(uint8_t index, uint8_t acc);
void LIS_read_XYZ(XYZ *xyz_data);

//void LIS_decimate_XYZ(uint8_t index);

uint8_t LIS_read_reg_8(uint8_t reg);

uint16_t LIS_read_reg_16(uint8_t reg);

void LIS_write_reg_8(uint8_t reg, uint8_t data);

void LIS_write_reg_16(uint8_t reg, uint16_t data);


#endif	//LIS3LV_H_