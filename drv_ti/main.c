//#############################################################################
//
// FILE:   empty_driverlib_main.c
//
//! \addtogroup driver_example_list
//! <h1>Empty Project Example</h1> 
//!
//! This example is an empty project setup for Driverlib development.
//!
//
//#############################################################################
//
//
// $Copyright:
// Copyright (C) 2026 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//#############################################################################

//
// Included Files
//
#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "c2000ware_libraries.h"

#include "drv_i2c_core.h"
//
// User Code
//

i2c_status_t status;
i2c_status_t status2;

#define LCD_ADDR            0x27    /* Dia chi I2C cua LCD (PCF8574T mac dinh la 0x27 khi hien thi A0-A2 ho) */

#define LCD_BACKLIGHT       0x08    /* Bit 3: Den nen (1 = Bat, 0 = Tat) */
#define LCD_ENABLE          0x04    /* Bit 2: Xung EN */
#define LCD_RW              0x00    /* Bit 1: Ghi (luon giu muc 0) */
#define LCD_RS_CMD          0x00    /* Bit 0: RS = 0 khi gui Lenh */
#define LCD_RS_DATA         0x01    /* Bit 0: RS = 1 khi gui Du lieu (Ky tu) */

static void LCD_I2C_Write(uint16_t byte_data)
{
    status = drv_i2c_send_message(LCD_ADDR, &byte_data);
}

// Gui 4-bit (High/Low Nibble) bang 2 lan phat 1-byte de tao xung EN (1 -> 0)
static void LCD_SendNibble(uint16_t nibble, uint16_t rs_mode)
{
    uint16_t data_en_high = nibble | rs_mode | LCD_BACKLIGHT | LCD_ENABLE;
    uint16_t data_en_low  = nibble | rs_mode | LCD_BACKLIGHT;

    // Bat chan EN len 1
    LCD_I2C_Write(data_en_high);
    DEVICE_DELAY_US(10);

    // Ha chan EN ve 0 de chot du lieu
    LCD_I2C_Write(data_en_low);
    DEVICE_DELAY_US(50);
}

// Gui 1 byte Lenh (RS = 0)
void LCD_SendCmd(uint16_t cmd)
{
    LCD_SendNibble(cmd & 0xF0, LCD_RS_CMD);         // Gui 4 bit cao
    LCD_SendNibble((cmd << 4) & 0xF0, LCD_RS_CMD);  // Gui 4 bit thap

    if (cmd == 0x01 || cmd == 0x02)
    {
        DEVICE_DELAY_US(2000);                      // Lenh Clear/Home can doi > 1.52ms
    }
}

// Gui 1 byte Ky tu hien thi (RS = 1)
void LCD_SendData(uint16_t data)
{
    LCD_SendNibble(data & 0xF0, LCD_RS_DATA);        // Gui 4 bit cao
    LCD_SendNibble((data << 4) & 0xF0, LCD_RS_DATA); // Gui 4 bit thap
}

// Khoi tao LCD1602 vao che do 4-bit
void LCD_Init(void)
{
    DEVICE_DELAY_US(50000); // Cho LCD on dinh nguon (> 40ms)

    // Chuoi khoi dong bat buoc ve 8-bit theo datasheet
    LCD_SendNibble(0x30, LCD_RS_CMD);
    DEVICE_DELAY_US(5000);
    LCD_SendNibble(0x30, LCD_RS_CMD);
    DEVICE_DELAY_US(200);
    LCD_SendNibble(0x30, LCD_RS_CMD);
    DEVICE_DELAY_US(200);

    // Chuyen han sang che do 4-bit
    LCD_SendNibble(0x20, LCD_RS_CMD);
    DEVICE_DELAY_US(1000);

    // Cau hinh hoat dong
    LCD_SendCmd(0x28); // 4-bit mode, 2 dong, ma tran 5x8
    LCD_SendCmd(0x0C); // Bat man hinh, tat con tro, tat nhap nhay
    LCD_SendCmd(0x06); // Tu dong tang con tro sang phai
    LCD_SendCmd(0x01); // Xoa toan bo man hinh
    DEVICE_DELAY_US(2000);
}

// Di chuyen con tro: row (0: hang 1, 1: hang 2), col (0 -> 15)
void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint16_t addr = (row == 0) ? (0x80 + col) : (0xC0 + col);
    LCD_SendCmd(addr);
}

// In chuoi ky tu
void LCD_Print(const char *str)
{
    while (*str)
    {
        LCD_SendData((uint16_t)*str++);
    }
}

//
// Main
//
void main(void)
{


    //
    // Initialize device clock and peripherals
    //
    Device_init();
    
    //
    // Disable pin locks and enable internal pull-ups.
    //
    Device_initGPIO();

    //
    // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    //
    Interrupt_initModule();

    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    //
    Interrupt_initVectorTable();

    //
    // PinMux and Peripheral Initialization
    //
    Board_init();

    
    drv_i2c_init();


    LCD_Init();

    //
    // C2000Ware Library initialization
    //
    C2000Ware_libraries_init();

    //
    // Enable Global Interrupt (INTM) and real time interrupt (DBGM)
    //
    EINT;
    ERTM;



    LCD_SetCursor(0, 0);
    LCD_Print("Hello C2000!");

    LCD_SetCursor(1, 0);
    LCD_Print("I2C LCD Driver");
uint16_t data = 0x00;
uint16_t data2 = 0x01;
while(1)
{
    status2 = drv_i2c_send_message(0x27, &data);
    DEVICE_DELAY_US(500000);
    status2 = drv_i2c_send_message(0x27, &data2);
    DEVICE_DELAY_US(500000);
}
}

//
// End of File
//
