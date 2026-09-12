#include "drv_i2c_core.h"

#ifdef ENABLE_I2C

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

static uint16_t i2c_inited = 0;

i2c_status_t drv_i2c_init (void) {
    i2c_obj_t *obj = &i2c_inst[I2C_ID_B];

    if (obj->method && obj->method->init) {
        if (obj->method->init(&obj->ctx) == I2C_OK) {
            i2c_inited = 1;
            return I2C_OK;
        }
    }
    return I2C_ERROR;
}

i2c_status_t drv_i2c_send_message (uint32_t id, uint16_t *Data) {
    if (!i2c_inited || Data == NULL) return I2C_ERROR;
    i2c_obj_t *obj = &i2c_inst[I2C_ID_B];

    if (obj->method && obj->method->send) {
        obj->ctx.sw_data.TargetAddr = (uint16_t)id;
        obj->ctx.sw_data.TxData     = Data;
        return obj->method->send(&obj->ctx);
    }
    return I2C_ERROR;
}

i2c_status_t drv_i2c_receive_message (uint32_t *id, uint16_t *Data) {
    if (!i2c_inited || id == NULL || Data == NULL) return I2C_ERROR;
    i2c_obj_t *obj = &i2c_inst[I2C_ID_B];

    if (obj->method && obj->method->receive) {
        obj->ctx.sw_data.TargetAddr = (uint16_t)(*id);
        obj->ctx.sw_data.RxData     = Data;
        return obj->method->receive(&obj->ctx);
    }
    return I2C_ERROR;
}

#endif /* ENABLE_I2C */
