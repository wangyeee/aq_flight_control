#ifndef _i2c_h
#define _i2c_h

#include "aq.h"
#include "stm32f4xx_i2c.h"

#define NO_OF_I2C_SLAVE 2
#define I2C_MAX_TRIALS_NUMBER 10
#define I2C_TIMEOUT 100

typedef enum {
    I2C_DIRECTION_TX = 0,
    I2C_DIRECTION_RX = 1
} i2cDirection_t;

typedef void i2cCallback_t(int);

typedef struct {
    uint8_t interface;
    uint8_t address;
    uint16_t speed;
    i2cCallback_t *callback;
} i2cSlave_t;

typedef struct {
    uint8_t initialized;
    volatile uint8_t dmaRunning;
    volatile i2cSlave_t* activeSlave;
    I2C_TypeDef* i2c;
    DMA_Stream_TypeDef* txStream;
    DMA_Stream_TypeDef* rxStream;
} i2cStruct_t;

extern i2cStruct_t i2cData[];

extern void i2cInit(void);
extern void i2cReadBlock(i2cSlave_t* dev, uint8_t is16Addr, uint16_t addr, int length, volatile uint8_t* data);
extern void i2cWriteBlock(i2cSlave_t* dev, uint8_t is16Addr, uint16_t addr, uint8_t length, uint8_t* data);
extern uint8_t i2cWaitStandbyState(i2cSlave_t* dev);
extern void i2cDMAStreamConfig(DMA_Stream_TypeDef* stream, uint32_t buffer, uint32_t bufferSize, i2cDirection_t direction);
extern i2cSlave_t* i2cRegisterSlaveDevice(I2C_TypeDef* interface, uint8_t address, i2cCallback_t *callback);

//test
extern void i2cReadOneByte(i2cSlave_t* dev, uint16_t reg, volatile uint8_t* data);
extern void i2cWriteOneByte(i2cSlave_t* dev, uint16_t reg, uint8_t data);

#endif /* _i2c_h */
