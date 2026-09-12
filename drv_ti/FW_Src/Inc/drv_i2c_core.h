#ifndef DRV_I2C_CORE_H_
#define DRV_I2C_CORE_H_

#include "interface_i2c.h"

#ifdef ENABLE_I2C

i2c_status_t drv_i2c_init (void);
i2c_status_t drv_i2c_send_message (uint32_t id ,uint16_t *Data);
i2c_status_t drv_i2c_receive_message (uint32_t *id ,uint16_t *Data);

#endif /* ENABLE_I2C */
#endif /* DRV_I2C_CORE_H_ */
