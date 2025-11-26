#ifndef OV7670_CONFIG_H
#define OV7670_CONFIG_H

#include "main.h"
#include "i2c.h"

// OV7670 I2C Address
#define OV7670_I2C_ADDR 0x42

// Image dimensions
#define OV7670_WIDTH   320
#define OV7670_HEIGHT  240
#define IMAGE_BUFFER_SIZE (OV7670_WIDTH * OV7670_HEIGHT * 2)  // RGB565

// Register definitions
#define REG_GAIN       0x00
#define REG_BLUE       0x01
#define REG_RED        0x02
#define REG_VREF       0x03
#define REG_COM1       0x04
#define REG_BAVE       0x05
#define REG_GbAVE      0x06
#define REG_GrAVE      0x07
#define REG_RAVE       0x08
#define REG_COM2       0x09
#define REG_PID        0x0A
#define REG_VER        0x0B
#define REG_COM3       0x0C
#define REG_COM4       0x0D
#define REG_COM5       0x0E
#define REG_COM6       0x0F
#define REG_AECH       0x10
#define REG_CLKRC      0x11
#define REG_COM7       0x12
#define REG_COM8       0x13
#define REG_COM9       0x14
#define REG_COM10      0x15
#define REG_HSTART     0x17
#define REG_HSTOP      0x18
#define REG_VSTART     0x19
#define REG_VSTOP      0x1A
#define REG_PSHFT      0x1B
#define REG_MIDH       0x1C
#define REG_MIDL       0x1D
#define REG_MVFP       0x1E
#define REG_BOS        0x20
#define REG_GBOS       0x21
#define REG_GROS       0x22
#define REG_ROS        0x23
#define REG_AEW        0x24
#define REG_EBW        0x25
#define REG_VPT        0x26
#define REG_BBIAS      0x27
#define REG_GbBIAS     0x28
#define REG_GRCOM      0x29
#define REG_EXHCH      0x2A
#define REG_EXHCL      0x2B
#define REG_RBIAS      0x2C
#define REG_ADVFL      0x2D
#define REG_ADVFH      0x2E
#define REG_YAVE       0x2F
#define REG_HSYST      0x30
#define REG_HSYEN      0x31
#define REG_HREF       0x32
#define REG_CHLF       0x33
#define REG_ARBLM      0x34
#define REG_RSVD       0x35
#define REG_ADC        0x37
#define REG_ACOM       0x38
#define REG_OFON       0x39
#define REG_TSLB       0x3A
#define REG_COM11      0x3B
#define REG_COM12      0x3C
#define REG_COM13      0x3D
#define REG_COM14      0x3E
#define REG_EDGE       0x3F
#define REG_REG75      0x75
#define REG_REG76      0x76
#define REG_RGB444     0x8C
#define REG_REG9A      0x9A
#define REG_REG9B      0x9B
#define REG_REG9C      0x9C
#define REG_REG9D      0x9D
#define REG_REG9E      0x9E
#define REG_REG9F      0x9F
#define REG_REGA0      0xA0
#define REG_REGA1      0xA1
#define REG_REGA2      0xA2
#define REG_REGA3      0xA3
#define REG_REGA4      0xA4
#define REG_REGA5      0xA5
#define REG_REGA6      0xA6
#define REG_REGA7      0xA7
#define REG_REGA8      0xA8
#define REG_REGA9      0xA9
#define REG_REGAA      0xAA
#define REG_REGAB      0xAB
#define REG_GFIX       0xAC
#define REG_REGAD      0xAD
#define REG_REGAE      0xAE
#define REG_REGAF      0xAF
#define REG_REGCOMS17  0xB0

// Function prototypes
uint8_t OV7670_Init(void);
uint8_t OV7670_WriteReg(uint8_t reg, uint8_t data);
uint8_t OV7670_ReadReg(uint8_t reg);
void OV7670_Config_QQVGA_RGB565(void);

#endif
