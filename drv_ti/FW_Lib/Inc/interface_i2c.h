#ifndef INTERFACE_I2C_H_
#define INTERFACE_I2C_H_

#include "_config_lib.h"
#include "drv_config.h"
#include <stdint.h>
#include <stddef.h>
#include "device.h"
#include "board.h"

#define I2C_RX_BUFFER_SIZE 16

typedef enum {
	I2C_OK,
	I2C_ERROR,
	I2C_BUSY,
    I2C_NACK,
	I2C_TIMEOUT,
} i2c_status_t;

typedef struct {
    uint32_t base;
    uint32_t timeout_cycles;
} i2c_hw_cfg_t;

typedef struct {
    uint16_t        TargetAddr;
    uint16_t        *TxData;
    uint16_t        *RxData;
    uint16_t        RxLen;
} i2c_sw_data_t;

typedef struct {
	i2c_id_t id;
	i2c_hw_cfg_t hw_cfg;
	i2c_sw_data_t sw_data;
} i2c_ctx_t;

typedef struct {
	i2c_status_t (*init) (i2c_ctx_t *ctx);
	i2c_status_t (*send) (i2c_ctx_t *ctx);
	i2c_status_t (*receive) (i2c_ctx_t *ctx);
} i2c_method_t;

typedef struct {
    i2c_ctx_t ctx;
   	const i2c_method_t *method;
} i2c_obj_t;

extern const i2c_method_t drv_i2c_method;

extern i2c_obj_t i2c_inst[I2C_ID_MAX];

#endif /* INTERFACE_I2C_H_ */
