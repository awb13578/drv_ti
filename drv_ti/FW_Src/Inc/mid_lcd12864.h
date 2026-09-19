#ifndef MID_LCD12864_H_
#define MID_LCD12864_H_

#include <stdint.h>

/* I2C address of MCP23017 with A0, A1, A2 tied to GND */
#define MCP23017_ADDR       0x20

/* MCP23017 Register addresses (IOCON.BANK = 0) */
#define MCP_IODIRA          0x00    /* Direction Port A (0 = Output, 1 = Input) */
#define MCP_IODIRB          0x01    /* Direction Port B (0 = Output, 1 = Input) */
#define MCP_GPIOA           0x12    /* Output latch Port A */
#define MCP_GPIOB           0x13    /* Output latch Port B */

/* Control pin mapping on MCP23017 Port B */
#define LCD_PIN_RS          (1 << 0)    /* GPB0 -> RS (0 = Command, 1 = Data) */
#define LCD_PIN_RW          (1 << 1)    /* GPB1 -> RW (Always 0 for Write) */
#define LCD_PIN_E           (1 << 2)    /* GPB2 -> Enable strobe */
#define LCD_PIN_RST         (1 << 3)    /* GPB3 -> Reset (Hold high) */

/* Public function prototypes */
void LCD12864_Init(void);
void LCD12864_SendCmd(uint16_t cmd);
void LCD12864_SendData(uint16_t data);
void LCD12864_SetCursor(uint16_t row, uint16_t col);
void LCD12864_Print(const char * str);
void LCD12864_Clear(void);

#endif /* MID_LCD12864_H_ */
