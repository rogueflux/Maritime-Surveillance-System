#include "ov7670_config.h"

// OV7670 register configuration for QQVGA RGB565
static const struct {
    uint8_t reg;
    uint8_t val;
} OV7670_RegConfig[] = {
    {REG_COM7, 0x80},     // Reset all registers
    {HAL_Delay(10)},
    
    // RGB565 Output, QQVGA
    {REG_COM7, 0x04},     // VGA & RGB
    {REG_COM15, 0xD0},    // RGB565, full output range
    {REG_RGB444, 0x00},   // No RGB444
    {REG_COM1, 0x00},     // CCIR601 not required
    
    // QQVGA resolution (160x120)
    {REG_COM14, 0x1A},    // DCW & PCLK divider
    {REG_SCALING_XSC, 0x3A},
    {REG_SCALING_YSC, 0x35},
    {REG_SCALING_DCWCTR, 0x11},
    {REG_SCALING_PCLK_DIV, 0xF0},
    {REG_SCALING_PCLK_DELAY, 0x02},
    
    // Timing
    {REG_HSTART, 0x16},
    {REG_HSTOP, 0x04},
    {REG_VSTART, 0x22},
    {REG_VSTOP, 0x7A},
    {REG_HREF, 0x24},
    
    // Matrix coefficients
    {REG_MTX1, 0x80},
    {REG_MTX2, 0x80},
    {REG_MTX3, 0x00},
    {REG_MTX4, 0x22},
    {REG_MTX5, 0x5E},
    {REG_MTX6, 0x80},
    
    // AGC and AEC
    {REG_COM8, 0xE7},     // AGC, AWB, AEC enable
    {REG_GAIN, 0x00},
    {REG_AECH, 0x80},
    {REG_COM4, 0x40},     // PLL aperture
    {REG_COM9, 0x2C},     // AGC ceiling 8x
    
    // Additional settings
    {REG_BD50MAX, 0x05},
    {REG_BD60MAX, 0x07},
    {REG_AEW, 0x95},
    {REG_EBW, 0x33},
    {REG_VPT, 0xE3},
    {REG_HAECC1, 0x78},
    {REG_HAECC2, 0x68},
    {REG_COM3, 0x00},     // No scaling
    {REG_COM5, 0x00},     // No scaling
    
    {0xFF, 0xFF}  // End marker
};

uint8_t OV7670_WriteReg(uint8_t reg, uint8_t data) {
    return HAL_I2C_Mem_Write(&hi2c1, OV7670_I2C_ADDR, reg, 1, &data, 1, 100);
}

uint8_t OV7670_ReadReg(uint8_t reg) {
    uint8_t data = 0;
    HAL_I2C_Mem_Read(&hi2c1, OV7670_I2C_ADDR, reg, 1, &data, 1, 100);
    return data;
}

uint8_t OV7670_Init(void) {
    uint8_t error = 0;
    const uint8_t *config = (const uint8_t*)OV7670_RegConfig;
    
    // Reset the camera
    error |= OV7670_WriteReg(REG_COM7, 0x80);
    HAL_Delay(100);
    
    // Write configuration registers
    for(int i = 0; OV7670_RegConfig[i].reg != 0xFF; i++) {
        if(OV7670_RegConfig[i].reg == HAL_Delay) {
            HAL_Delay(OV7670_RegConfig[i].val);
        } else {
            error |= OV7670_WriteReg(OV7670_RegConfig[i].reg, OV7670_RegConfig[i].val);
        }
    }
    
    return error;
}
