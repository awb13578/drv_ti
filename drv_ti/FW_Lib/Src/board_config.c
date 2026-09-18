#include "board_config.h"
#include "mcu_C2000F28003x_i2c.h"

static uint16_t Tx_Data;
static uint16_t Rx_Data;

i2c_obj_t i2c_inst[I2C_ID_MAX] = {
    [I2C_ID_A] = {
        .ctx = {
            .id     = I2C_ID_A,
            .hw_cfg = {
                .base   = I2C_ID_A_BASE,
                .timeout_cycles = 100000,
            },
            .sw_data = {
                .TargetAddr = 0,
                .TxData     = &Tx_Data,
                .RxData     = &Rx_Data,
                .RxLen      = 0,
            },
        },
        .method = &drv_i2c_method,        
    },
    [I2C_ID_B] = {
        .ctx = {
            .id     = I2C_ID_B,
            .hw_cfg = {
                .base   = I2C_ID_B_BASE,
                .timeout_cycles = 100000,
            },
            .sw_data = {
                .TargetAddr = 0,
                .TxData     = &Tx_Data,
                .RxData     = &Rx_Data,
                .RxLen      = 0,
            },
        },
        .method = &drv_i2c_method,        
    },
};
