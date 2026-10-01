#include "mid_lcd2004.h"
#include "board_config.h"
#include "drv_i2c_core.h"

#define LCD2004_BACKLIGHT       0x08    /* Bit 3: Backlight control (1 = On, 0 = Off) */
#define LCD2004_ENABLE          0x04    /* Bit 2: Enable strobe pulse */
#define LCD2004_RW              0x00    /* Bit 1: Read/Write (always 0 for Write) */
#define LCD2004_RS_CMD          0x00    /* Bit 0: RS = 0 for Command registers */
#define LCD2004_RS_DATA         0x01    /* Bit 0: RS = 1 for Data registers */

static void LCD2004_I2C_Write(uint16_t byte_data)
{
    drv_i2c_send_message(LCD_ADDR, &byte_data);
}

static void LCD2004_SendNibble(uint16_t nibble, uint16_t rs_mode)
{
    uint16_t data_en_high = nibble | rs_mode | LCD2004_BACKLIGHT | LCD2004_ENABLE;
    uint16_t data_en_low  = nibble | rs_mode | LCD2004_BACKLIGHT;

    // Pull EN high
    LCD2004_I2C_Write(data_en_high);
    DEVICE_DELAY_US(10);

    // Pull EN low to create falling edge and latch data
    LCD2004_I2C_Write(data_en_low);
    DEVICE_DELAY_US(50);
}

void LCD2004_SendCmd(uint16_t cmd)
{
    LCD2004_SendNibble(cmd & 0xF0, LCD2004_RS_CMD);         // Send high nibble
    LCD2004_SendNibble((cmd << 4) & 0xF0, LCD2004_RS_CMD);  // Send low nibble

    if (cmd == 0x01 || cmd == 0x02)
    {
        DEVICE_DELAY_US(2000);                      // Clear/Home commands require execution time > 1.52ms
    }
}

void LCD2004_SendData(uint16_t data)
{
    LCD2004_SendNibble(data & 0xF0, LCD2004_RS_DATA);        // Send high nibble
    LCD2004_SendNibble((data << 4) & 0xF0, LCD2004_RS_DATA); // Send low nibble
}

void LCD2004_Init(void)
{
    DEVICE_DELAY_US(50000); // Wait for LCD power supply stabilization (> 40ms)

    // Hardware reset sequence for HD44780 controller in 8-bit mode
    LCD2004_SendNibble(0x30, LCD2004_RS_CMD);
    DEVICE_DELAY_US(5000);
    LCD2004_SendNibble(0x30, LCD2004_RS_CMD);
    DEVICE_DELAY_US(200);
    LCD2004_SendNibble(0x30, LCD2004_RS_CMD);
    DEVICE_DELAY_US(200);

    // Switch to 4-bit interface mode
    LCD2004_SendNibble(0x20, LCD2004_RS_CMD);
    DEVICE_DELAY_US(1000);

    // Function set and display configuration
    LCD2004_SendCmd(0x28); // 4-bit mode, 4 lines (LCD2004), 5x8 dot matrix
    LCD2004_SendCmd(0x0C); // Display ON, Cursor OFF, Blinking OFF
    LCD2004_SendCmd(0x06); // Entry mode: Auto-increment cursor to the right
    LCD2004_SendCmd(0x01); // Clear entire screen display
    DEVICE_DELAY_US(2000);
}

// row: 0 to 3, col: 0 to 19 (Standard DDRAM address offsets for LCD2004)
void LCD2004_SetCursor(uint16_t row, uint16_t col)
{
    static const uint16_t row_offsets[] = {0x80, 0xC0, 0x94, 0xD4};
    if (row > 3)
    {
        row = 3;
    }
    LCD2004_SendCmd(row_offsets[row] + col);
}

void LCD2004_Print(const char * str)
{
    while ( * str)
    {
        LCD2004_SendData((uint16_t) * str++);
    }
}

void LCD2004_Clear(void)
{
    LCD2004_SendCmd(0x01);
    DEVICE_DELAY_US(2000);
}
