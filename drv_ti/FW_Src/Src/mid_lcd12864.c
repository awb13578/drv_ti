#include "mid_lcd12864.h"
#include "board_config.h"
#include "drv_i2c_core.h"

/*
 * Write 1 byte to a specific register in MCP23017.
 * Transmits [reg_addr, data_val] sequentially in a single transaction.
 */
static void MCP23017_WriteReg(uint16_t reg_addr, uint16_t data_val)
{
    uint16_t tx_buf[2];
    tx_buf[0] = reg_addr;
    tx_buf[1] = data_val;

    /*
     * Call your driver function to transmit 2 bytes.
     * Adjust signature if your drv_i2c core requires a length parameter.
     */
    drv_i2c_send_message(MCP23017_ADDR, tx_buf);
}

/* Generate falling edge strobe pulse on Enable (E) pin to latch data */
static void LCD12864_PulseEnable(uint16_t ctrl_state)
{
    /* Pull E high */
    MCP23017_WriteReg(MCP_GPIOB, ctrl_state | LCD_PIN_E);
    DEVICE_DELAY_US(2);

    /* Pull E low (falling edge latch) */
    MCP23017_WriteReg(MCP_GPIOB, ctrl_state & ~LCD_PIN_E);
    DEVICE_DELAY_US(50);
}

/* Send 8-bit command to ST7920 controller */
void LCD12864_SendCmd(uint16_t cmd)
{
    /* 1. Put command byte onto Port A (D0 - D7) */
    MCP23017_WriteReg(MCP_GPIOA, cmd);

    /* 2. RS = 0 (Command), RW = 0 (Write), RST = 1 */
    uint16_t ctrl = LCD_PIN_RST;
    MCP23017_WriteReg(MCP_GPIOB, ctrl);

    /* 3. Strobe Enable pin */
    LCD12864_PulseEnable(ctrl);

    if (cmd == 0x01)
    {
        DEVICE_DELAY_US(2000);  /* Clear screen command requires > 1.6ms */
    }
}

/* Send 8-bit data/character to ST7920 controller */
void LCD12864_SendData(uint16_t data)
{
    /* 1. Put data byte onto Port A (D0 - D7) */
    MCP23017_WriteReg(MCP_GPIOA, data);

    /* 2. RS = 1 (Data), RW = 0 (Write), RST = 1 */
    uint16_t ctrl = LCD_PIN_RST | LCD_PIN_RS;
    MCP23017_WriteReg(MCP_GPIOB, ctrl);

    /* 3. Strobe Enable pin */
    LCD12864_PulseEnable(ctrl);
}

/* Initialize MCP23017 expander and ST7920 LCD in 8-bit parallel mode */
void LCD12864_Init(void)
{
    DEVICE_DELAY_US(50000);     /* Wait for LCD supply voltage to stabilize (> 40ms) */

    /* Configure all pins of Port A and Port B as Outputs */
    MCP23017_WriteReg(MCP_IODIRA, 0x00);
    MCP23017_WriteReg(MCP_IODIRB, 0x00);

    /* Release reset pin by pulling RST high */
    MCP23017_WriteReg(MCP_GPIOB, LCD_PIN_RST);
    DEVICE_DELAY_US(10000);

    /* ST7920 startup initialization sequence */
    LCD12864_SendCmd(0x30);     /* Basic instruction set, 8-bit bus */
    DEVICE_DELAY_US(150);
    LCD12864_SendCmd(0x30);     /* Repeat command for synchronization */
    DEVICE_DELAY_US(50);
    LCD12864_SendCmd(0x0C);     /* Display ON, Cursor OFF, Blink OFF */
    DEVICE_DELAY_US(100);
    LCD12864_SendCmd(0x01);     /* Clear entire display */
    DEVICE_DELAY_US(12000);
    LCD12864_SendCmd(0x06);     /* Cursor moves right after write */
    DEVICE_DELAY_US(100);
}

/*
 * Set cursor position for 12864 text mode:
 * row: 0 to 3, col: 0 to 7 (for 16x16 chars) or 0 to 15 (for 8x16 ASCII)
 */
void LCD12864_SetCursor(uint16_t row, uint16_t col)
{
    static const uint16_t row_addresses[] = {0x80, 0x90, 0x88, 0x98};
    if (row > 3)
    {
        row = 3;
    }
    LCD12864_SendCmd(row_addresses[row] + col);
}

/* Print ASCII string */
void LCD12864_Print(const char * str)
{
    while ( * str)
    {
        LCD12864_SendData((uint16_t)( * str));
        str++;
    }
}

/* Clear display screen */
void LCD12864_Clear(void)
{
    LCD12864_SendCmd(0x01);
    DEVICE_DELAY_US(2000);
}
