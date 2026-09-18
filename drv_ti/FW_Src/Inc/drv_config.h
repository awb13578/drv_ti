#ifndef DRV_CONFIG_H_
#define DRV_CONFIG_H_

typedef enum {
    I2C_ID_A     = 0,
    //SDA   /* GPIO19 (Pin 69) */
    //DCL   /* GPIO18 (Pin 68) */
    I2C_ID_B     = 1,
    I2C_ID_MAX
} i2c_id_t;

#endif /* DRV_CONFIG_H_ */
