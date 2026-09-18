#ifndef MCU_C2000F28003X_I2C_H_
#define MCU_C2000F28003X_I2C_H_

#include "interface_i2c.h"
#include "driverlib.h"
#include "device.h"
#include "board.h"

#ifdef ENABLE_I2C

#define TxLen 2

extern i2c_obj_t i2c_inst[I2C_ID_MAX];

#endif /* ENABLE_I2C */
#endif /* MCU_C2000F28003X_I2C_H_ */
