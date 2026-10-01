#ifndef MID_LCD2004_H_
#define MID_LCD2004_H_

#include <stdint.h>

#define LCD_ADDR            0x27    /* Address of I2C PCF8574 */

void LCD2004_Init(void);
void LCD2004_SendCmd(uint16_t cmd);
void LCD2004_SendData(uint16_t data);
void LCD2004_SetCursor(uint16_t row, uint16_t col);
void LCD2004_Print(const char *str);
void LCD2004_Clear(void);

#endif /* MID_LCD2004_H_ */
