#include "config.h"

#ifdef HAS_DIGITAL_IMU
#ifdef DIMU_HAVE_MAG3110

#include "mag3110.h"
#include "imu.h"
#include "aq_timer.h"
#include "util.h"
#include "ext_irq.h"
#include "string.h"

mag3110Struct_t mag3110Data;

static void mag3110TransferComplete(int unused) {
    mag3110Data.slot = (mag3110Data.slot + 1) % MAG3110_SLOTS;
}

static uint8_t mag3110GetReg(uint8_t reg) {
    uint8_t val;
    i2cReadOneByte(mag3110Data.i2cBus, reg, &val);
    return val;
}

static void mag3110SetReg(uint8_t reg, uint8_t val) {
    i2cWriteOneByte(mag3110Data.i2cBus, reg, val);
}

static void mag3110ScaleMag(int32_t *in, float *out, float divisor) {
    float scale;

    // 1LSB = 1mGauss
    scale = divisor * (1.0f / 1000.0f);  // check scale value, convert to gauss

    out[0] = mag3110Data.magSign[0] * DIMU_ORIENT_MAG_X * scale;
    out[1] = mag3110Data.magSign[1] * DIMU_ORIENT_MAG_Y * scale;
    out[2] = mag3110Data.magSign[2] * DIMU_ORIENT_MAG_Z * scale;
}

static void mag3110CalibMag(float *in, volatile float *out) {
    float a, b, c;
    float x, y, z;

    // bias
    a = +(in[0] + p[IMU_MAG_BIAS_X] + p[IMU_MAG_BIAS1_X]*dImuData.dTemp + p[IMU_MAG_BIAS2_X]*dImuData.dTemp2 + p[IMU_MAG_BIAS3_X]*dImuData.dTemp3);
    b = +(in[1] + p[IMU_MAG_BIAS_Y] + p[IMU_MAG_BIAS1_Y]*dImuData.dTemp + p[IMU_MAG_BIAS2_Y]*dImuData.dTemp2 + p[IMU_MAG_BIAS3_Y]*dImuData.dTemp3);
    c = -(in[2] + p[IMU_MAG_BIAS_Z] + p[IMU_MAG_BIAS1_Z]*dImuData.dTemp + p[IMU_MAG_BIAS2_Z]*dImuData.dTemp2 + p[IMU_MAG_BIAS3_Z]*dImuData.dTemp3);

    // misalignment
    x = a                    + b*p[IMU_MAG_ALGN_XY] + c*p[IMU_MAG_ALGN_XZ];
    y = a*p[IMU_MAG_ALGN_YX] + b                    + c*p[IMU_MAG_ALGN_YZ];
    z = a*p[IMU_MAG_ALGN_ZX] + b*p[IMU_MAG_ALGN_ZY] + c;

    // scale
    x /= (p[IMU_MAG_SCAL_X] + p[IMU_MAG_SCAL1_X]*dImuData.dTemp + p[IMU_MAG_SCAL2_X]*dImuData.dTemp2 + p[IMU_MAG_SCAL3_X]*dImuData.dTemp3);
    y /= (p[IMU_MAG_SCAL_Y] + p[IMU_MAG_SCAL1_Y]*dImuData.dTemp + p[IMU_MAG_SCAL2_Y]*dImuData.dTemp2 + p[IMU_MAG_SCAL3_Y]*dImuData.dTemp3);
    z /= (p[IMU_MAG_SCAL_Z] + p[IMU_MAG_SCAL1_Z]*dImuData.dTemp + p[IMU_MAG_SCAL2_Z]*dImuData.dTemp2 + p[IMU_MAG_SCAL3_Z]*dImuData.dTemp3);

    out[0] = x * imuData.cosRot - y * imuData.sinRot;
    out[1] = y * imuData.cosRot + x * imuData.sinRot;
    out[2] = z;
}

#ifdef DIMU_MAG3110_INT_PORT
void mag3110IntHandler(void) {
    if (mag3110Data.enabled)
        i2cReadBlock(mag3110Data.i2cBus, 0, mag3110Data.readCmd, MAG3110_BYTES, &(mag3110Data.rxBuf[mag3110Data.slot * MAG3110_SLOT_SIZE]));
}
#endif

void mag3110PreInit(void) {
    memset(&mag3110Data, 0, sizeof(mag3110Struct_t));
    mag3110Data.i2cBus = i2cRegisterSlaveDevice(DIMU_MAG3110_I2C, DIMU_MAG3110_I2C_ADDR, mag3110TransferComplete);
}

uint8_t mag3110Init(void) {
    int i = MAG3110_RETRIES;

    if (mag3110Data.i2cBus == NULL)
        return 0;  // not initialized

    switch ((int)p[IMU_FLIP]) {
    case 1:
        mag3110Data.magSign[0] =  1.0f;
        mag3110Data.magSign[1] = -1.0f;
        mag3110Data.magSign[2] = -1.0f;
        break;
    case 2:
        mag3110Data.magSign[0] = -1.0f;
        mag3110Data.magSign[1] =  1.0f;
        mag3110Data.magSign[2] = -1.0f;
        break;
    case 0:
    default:
        mag3110Data.magSign[0] = 1.0f;
        mag3110Data.magSign[1] = 1.0f;
        mag3110Data.magSign[2] = 1.0f;
        break;
    }

    // wait for a valid response
    while (--i && mag3110GetReg(0x07) != 0xC4)
        delay(100);

    if (i > 0) {
        mag3110SetReg(0x11, 0x80); // Auto MRST
        delay(10);
        mag3110SetReg(0x10, 0x01); // ODR 80Hz, full 16bit active mode
        delay(10);

        mag3110Data.readCmd = 0x01;  // OUT_X_MSB

#ifdef DIMU_MAG3110_INT_PORT
#ifndef STM32F4_DEFECTS
        // External Interrupt line for data ready
        extRegisterCallback(DIMU_MAG3110_INT_PORT, DIMU_MAG3110_INT_PIN, EXTI_Trigger_Rising, 1, GPIO_PuPd_NOPULL, mag3110IntHandler);
#endif
#endif
        mag3110Data.initialized = 1;
    } else {
        mag3110Data.initialized = 0;
    }

    return mag3110Data.initialized;
}

// debug only
void mag3110Decode(void) {
    volatile uint8_t *d = mag3110Data.rxBuf;
    int32_t mag[3];
    float divisor;
    int i;

    if (mag3110Data.enabled) {
        mag[0] = 0;
        mag[1] = 0;
        mag[2] = 0;

        divisor = (float) MAG3110_SLOTS;
        for (i = 0; i < MAG3110_SLOTS; i++) {
            int j = i * MAG3110_SLOT_SIZE;

            // check if we are in the middle of a transaction for this slot
            if (i == mag3110Data.slot && i2cData[mag3110Data.i2cBus->interface].dmaRunning) {
                divisor -= 1.0f;
            } else {
                mag[0] += (int16_t)__rev16(*(uint16_t *)&d[j+1]);
                mag[1] += (int16_t)__rev16(*(uint16_t *)&d[j+3]);
                mag[2] += (int16_t)__rev16(*(uint16_t *)&d[j+5]);
            }
        }

        divisor = 1.0f / divisor;

        mag3110ScaleMag(mag, mag3110Data.rawMag, divisor);
        mag3110CalibMag(mag3110Data.rawMag, mag3110Data.mag);

        mag3110Data.lastUpdate = timerMicros();
    }
}

void mag3110Enable(void) {
    if (mag3110Data.initialized)
        mag3110Data.enabled = 1;
}

void mag3110Disable(void) {
    mag3110Data.enabled = 0;
}

#endif
#endif
