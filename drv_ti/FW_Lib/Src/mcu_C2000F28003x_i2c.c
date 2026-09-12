#include "mcu_C2000F28003x_i2c.h"

#ifdef ENABLE_I2C

static i2c_status_t mcu_i2c_init (i2c_ctx_t *ctx) {
    if (ctx == NULL || ctx->hw_cfg.base == 0) return I2C_ERROR;
    uint32_t base = ctx->hw_cfg.base;
    I2C_disableModule(base);
    DEVICE_DELAY_US(10);
    I2C_enableModule(base);
    return I2C_OK;
}

static i2c_status_t mcu_i2c_send_message (i2c_ctx_t * ctx) {
    if (ctx == NULL || ctx->sw_data.TxData == NULL) return I2C_ERROR;
    uint32_t base = ctx->hw_cfg.base;
    uint32_t timeout = ctx->hw_cfg.timeout_cycles;
    
    //Wait for idle
    while (I2C_isBusBusy(base)) {
        if (--timeout == 0) {
            I2C_disableModule(base);
            DEVICE_DELAY_US(10);
            I2C_enableModule(base);
            if (I2C_isBusBusy(base)) return I2C_BUSY;
            break;
        }
    }
    I2C_setTargetAddress(base, ctx->sw_data.TargetAddr);
    I2C_setDataCount(base, TxLen);

    // Flush and load TX FIFO first
    HWREGH(base + I2C_O_FFTX) &= ~I2C_FFTX_TXFFRST;
    HWREGH(base + I2C_O_FFTX) |= I2C_FFTX_TXFFRST;

    uint16_t i;
    for (i = 0; i < TxLen; i++) {
        timeout = ctx->hw_cfg.timeout_cycles;
        while (I2C_getTxFIFOStatus(base) == I2C_FIFO_TX16) {
            if (--timeout == 0) return I2C_TIMEOUT;
        }
        I2C_putData(base, ctx->sw_data.TxData[i]);
    }

    // Set Controller Send Mode (TRX = 1: Write) and set START & STOP bits atomically
    I2C_setConfig(base, I2C_CONTROLLER_SEND_MODE);
    HWREGH(base + I2C_O_MDR) |= (I2C_MDR_STT | I2C_MDR_STP);

    timeout = ctx->hw_cfg.timeout_cycles;
    while (I2C_getStopConditionStatus(base)) {
        if (I2C_getStatus(base) & I2C_STS_NO_ACK) {
            I2C_clearStatus(base, I2C_STS_NO_ACK);
            I2C_sendStopCondition(base);
            HWREGH(base + I2C_O_FFTX) &= ~I2C_FFTX_TXFFRST;
            HWREGH(base + I2C_O_FFTX) |= I2C_FFTX_TXFFRST;
            while (I2C_getStopConditionStatus(base)) {
                if (--timeout == 0) break;
            }
            return I2C_NACK;
        }
        if (--timeout == 0) return I2C_TIMEOUT;
    }

    //Send Stop
    if (I2C_getStatus(base) & I2C_STS_NO_ACK) {
        I2C_clearStatus(base, I2C_STS_NO_ACK);
        I2C_sendStopCondition(base);
        HWREGH(base + I2C_O_FFTX) &= ~I2C_FFTX_TXFFRST;
        HWREGH(base + I2C_O_FFTX) |= I2C_FFTX_TXFFRST;
        return I2C_NACK;
    }

    return I2C_OK;
}

static i2c_status_t mcu_i2c_receive_message (i2c_ctx_t *ctx) {
    if (ctx == NULL || ctx->sw_data.RxData == NULL || ctx->sw_data.RxLen == 0) return I2C_ERROR;
    uint32_t base = ctx->hw_cfg.base;
    uint32_t timeout = ctx->hw_cfg.timeout_cycles;

    // Wait for idle
    while (I2C_isBusBusy(base)) {
        if (--timeout == 0) {
            I2C_disableModule(base);
            DEVICE_DELAY_US(10);
            I2C_enableModule(base);
            if (I2C_isBusBusy(base)) return I2C_BUSY;
            break;
        }
    }

    I2C_setTargetAddress(base, ctx->sw_data.TargetAddr);
    I2C_setDataCount(base, ctx->sw_data.RxLen);

    //Flush RX FIFO
    HWREGH(base + I2C_O_FFRX) &= ~I2C_FFRX_RXFFRST;
    HWREGH(base + I2C_O_FFRX) |= I2C_FFRX_RXFFRST;

    // Set Controller Receive  Mode (TRX = 0) and set START & STOP bits atomically
    I2C_setConfig(base, I2C_CONTROLLER_RECEIVE_MODE);
    HWREGH(base + I2C_O_MDR) |= (I2C_MDR_STT | I2C_MDR_STP);

    uint16_t i;
    for (i = 0; i < ctx->sw_data.RxLen; i++) {
        timeout = ctx->hw_cfg.timeout_cycles;
        while (I2C_getRxFIFOStatus(base) == I2C_FIFO_RXEMPTY) {
            // Kiểm tra lỗi NACK từ Slave khi gửi địa chỉ
            if (I2C_getStatus(base) & I2C_STS_NO_ACK) {
                I2C_clearStatus(base, I2C_STS_NO_ACK);
                I2C_sendStopCondition(base);
                HWREGH(base + I2C_O_FFRX) &= ~I2C_FFRX_RXFFRST;
                HWREGH(base + I2C_O_FFRX) |= I2C_FFRX_RXFFRST;
                while (I2C_getStopConditionStatus(base)) {
                    if (--timeout == 0) break;
                }
                return I2C_NACK;
            }
            if (--timeout == 0) return I2C_TIMEOUT;
        }
        ctx->sw_data.RxData[i] = I2C_getData(base);
    }

    //Send Stop
    timeout = ctx->hw_cfg.timeout_cycles;
    while (I2C_getStopConditionStatus(base)) {
        if (--timeout == 0) return I2C_TIMEOUT;
    }

    return I2C_OK;
}

const i2c_method_t drv_i2c_method = {
		.init		= mcu_i2c_init,
		.send		= mcu_i2c_send_message,
		.receive 	= mcu_i2c_receive_message,
};

#endif /* ENABLE_I2C */
