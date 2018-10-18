#include "i2c.h"
#include "stddef.h"
#include "util.h"

i2cStruct_t i2cData[3];
static i2cSlave_t slaves[NO_OF_I2C_SLAVE];
static uint8_t interfaces;

#ifdef I2C_I2C1
static void i2c1Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    if (i2cData[0].initialized)
        return;

    RCC_APB1PeriphClockCmd(I2C_I2C1_CLOCK, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    GPIO_PinAFConfig(I2C_I2C1_SCL_PORT, I2C_I2C1_SCL_SOURCE, I2C_I2C1_SCL_AF);
    GPIO_PinAFConfig(I2C_I2C1_SDA_PORT, I2C_I2C1_SDA_SOURCE, I2C_I2C1_SDA_AF);
    GPIO_InitStructure.GPIO_Pin = I2C_I2C1_SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(I2C_I2C1_SCL_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = I2C_I2C1_SDA_PIN;
    GPIO_Init(I2C_I2C1_SDA_PORT, &GPIO_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = I2C_I2C1_DMA_TX_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = I2C_I2C1_DMA_RX_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);

    RCC_AHB1PeriphClockCmd(I2C_I2C1_DMA_CLOCK, ENABLE);
    DMA_ClearFlag(I2C_I2C1_DMA_TX, I2C_I2C1_DMA_TX_FLAGS);
    DMA_Cmd(I2C_I2C1_DMA_TX, DISABLE);
    DMA_DeInit(I2C_I2C1_DMA_TX);

    DMA_InitStructure.DMA_Channel = I2C_I2C1_DMA_TX_CHANNEL;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&(I2C_I2C1->DR));  // 0x40005410

    /* These parameters will be configured durig communication */;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)0;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize = 0xFFFF;

    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(I2C_I2C1_DMA_TX, &DMA_InitStructure);

    DMA_InitStructure.DMA_Channel = I2C_I2C1_DMA_RX_CHANNEL;
    DMA_ClearFlag(I2C_I2C1_DMA_RX, I2C_I2C1_DMA_RX_FLAGS);
    DMA_Cmd(I2C_I2C1_DMA_RX, DISABLE);
    DMA_DeInit(I2C_I2C1_DMA_RX);
    DMA_Init(I2C_I2C1_DMA_RX, &DMA_InitStructure);

    DMA_ITConfig(I2C_I2C1_DMA_TX, DMA_IT_TC, ENABLE);
    DMA_ITConfig(I2C_I2C1_DMA_RX, DMA_IT_TC, ENABLE);

    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 400000; // Hz

    I2C_Init(I2C_I2C1, &I2C_InitStructure);
    I2C_Cmd(I2C_I2C1, ENABLE);

    interfaces++;
    i2cData[0].initialized = 1;
    i2cData[0].i2c = I2C_I2C1;
    i2cData[0].txStream = I2C_I2C1_DMA_TX;
    i2cData[0].rxStream = I2C_I2C1_DMA_RX;
}
#endif

#ifdef I2C_I2C2
static void i2c2Init(void) {
    if (i2cData[1].initialized)
        return;
    interfaces++;
    i2cData[1].initialized = 1;
    i2cData[1].i2c = I2C_I2C2;
    i2cData[1].txStream = I2C_I2C2_DMA_TX;
    i2cData[1].rxStream = I2C_I2C2_DMA_RX;
}
#endif

#ifdef I2C_I2C3
static void i2c3Init(void) {
    if (i2cData[2].initialized)
        return;
    interfaces++;
    i2cData[2].initialized = 1;
    i2cData[2].i2c = I2C_I2C3;
    i2cData[2].txStream = I2C_I2C3_DMA_TX;
    i2cData[2].rxStream = I2C_I2C3_DMA_RX;
}
#endif

static void i2cDefaultCallback(int unused) {
    (void) unused;
}

void i2cInit(void) {
    if (interfaces)
        return;
    interfaces = 0;
#ifdef I2C_I2C1
    i2c1Init();
#endif
#ifdef I2C_I2C2
    i2c2Init();
#endif
#ifdef I2C_I2C3
    i2c3Init();
#endif
}

void i2cDMAStreamConfig(DMA_Stream_TypeDef* stream, uint32_t data, uint32_t length, i2cDirection_t direction) {
    stream->CR &= ~(uint32_t)DMA_SxCR_EN;  // enable write access
    stream->NDTR = length;
    stream->M0AR = data;
    if (direction == I2C_DIRECTION_TX) {
        stream->CR |= 0x00000040;  // 0b0000 0000 0000 0000 0000 0000 0100 0000, DIR = 01, m2p
    } else {
        stream->CR &= 0xFFFFFF3E;  // 0b1111 1111 1111 1111 1111 1111 0011 1110, DIR = 00, p2m
    }
}

void i2cReadBlock(i2cSlave_t* dev, uint8_t is16Addr, uint16_t addr, int length, volatile uint8_t* data) {
    if (length < 2) {
        i2cReadOneByte(dev, addr, data);
        return;
    }
    while (I2C_GetFlagStatus(i2cData[dev->interface].i2c, I2C_FLAG_BUSY))
        yield(1);

    I2C_GenerateSTART(i2cData[dev->interface].i2c, ENABLE);
    while (!I2C_CheckEvent(i2cData[dev->interface].i2c, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(i2cData[dev->interface].i2c, dev->address, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(i2cData[dev->interface].i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    if (is16Addr) {
        // 16 bit address for eeprom
        I2C_SendData(i2cData[dev->interface].i2c, (uint8_t) ((addr & 0xFF00) >> 8));
        while (!I2C_CheckEvent(i2cData[dev->interface].i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    }
    I2C_SendData(i2cData[dev->interface].i2c, (uint8_t) (addr & 0x00FF));
    while (I2C_GetFlagStatus(i2cData[dev->interface].i2c, I2C_FLAG_BTF) == RESET);

    I2C_GenerateSTART(i2cData[dev->interface].i2c, ENABLE);
    while (!I2C_CheckEvent(i2cData[dev->interface].i2c, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(i2cData[dev->interface].i2c, dev->address, I2C_Direction_Receiver);
    while (!I2C_CheckEvent(i2cData[dev->interface].i2c, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    i2cData[dev->interface].activeSlave = dev;
    i2cData[dev->interface].dmaRunning = 1;
    i2cDMAStreamConfig(i2cData[dev->interface].rxStream, (uint32_t) data, (uint16_t) length, I2C_DIRECTION_RX);

    I2C_DMALastTransferCmd(i2cData[dev->interface].i2c, ENABLE);
    DMA_Cmd(i2cData[dev->interface].rxStream, ENABLE);
    I2C_DMACmd(i2cData[dev->interface].i2c, ENABLE);
}

void i2cWriteBlock(i2cSlave_t* dev, uint8_t is16Addr, uint16_t addr, uint8_t length, uint8_t* data) {
    if (length < 2) {
        i2cWriteOneByte(dev, addr, *data);
        return;
    }
    while (I2C_GetFlagStatus(i2cData[dev->interface].i2c, I2C_FLAG_BUSY));
    /*!< Send START condition */
    I2C_GenerateSTART(i2cData[dev->interface].i2c, ENABLE);
    while (!I2C_CheckEvent(i2cData[dev->interface].i2c, I2C_EVENT_MASTER_MODE_SELECT));

    /*!< Send slave address for write */
    I2C_Send7bitAddress(i2cData[dev->interface].i2c, dev->address, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(i2cData[dev->interface].i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    if (is16Addr) {
        // 16 bit address for eeprom
        I2C_SendData(i2cData[dev->interface].i2c, (uint8_t) ((addr & 0xFF00) >> 8));
        while (!I2C_CheckEvent(i2cData[dev->interface].i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    }
    I2C_SendData(i2cData[dev->interface].i2c, (uint8_t) (addr & 0x00FF));
    while (!I2C_CheckEvent(i2cData[dev->interface].i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTING));

#if 1
    i2cDMAStreamConfig(i2cData[dev->interface].txStream, (uint32_t) data, length, I2C_DIRECTION_TX);
    i2cData[dev->interface].dmaRunning = 1;
    i2cData[dev->interface].activeSlave = dev;

    /* Enable the DMA Tx Stream */
    DMA_Cmd(i2cData[dev->interface].txStream, ENABLE);

    /* Enable the I2C peripheral DMA requests */
    I2C_DMACmd(i2cData[dev->interface].i2c, ENABLE);
#else
    while (length) {
        I2C_SendData(i2cData[dev->interface].i2c, *data);
        while (!I2C_CheckEvent(i2cData[dev->interface].i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
        data++;
        length--;
    }
    I2C_GenerateSTOP(i2cData[dev->interface].i2c, ENABLE);
#endif
}

void i2cWriteBlock_obsolete(i2cSlave_t* dev, uint8_t* pBuffer, uint16_t WriteAddr, uint8_t NumByteToWrite) {
    /*!< While the bus is busy */
    while (I2C_GetFlagStatus(i2cData[dev->interface].i2c, I2C_FLAG_BUSY));

    /*!< Send START condition */
    I2C_GenerateSTART(i2cData[dev->interface].i2c, ENABLE);

    /*!< Test on EV5 and clear it */
    while (!I2C_CheckEvent(i2cData[dev->interface].i2c, I2C_EVENT_MASTER_MODE_SELECT));

    /*!< Send EEPROM address for write */
    I2C_Send7bitAddress(i2cData[dev->interface].i2c, dev->address, I2C_Direction_Transmitter);

    i2cData[dev->interface].activeSlave = dev;

    /*!< Test on EV6 and clear it */
    while (!I2C_CheckEvent(i2cData[dev->interface].i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    /*!< Test on EV8 and clear it */
    while (!I2C_CheckEvent(i2cData[dev->interface].i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTING));

    /* Configure the DMA Tx Channel with the buffer address and the buffer size */
    i2cDMAStreamConfig(i2cData[dev->interface].rxStream, (uint32_t) pBuffer, NumByteToWrite, I2C_DIRECTION_TX);
    i2cData[dev->interface].dmaRunning = 1;

    /* Enable the DMA Tx Stream */
    DMA_Cmd(i2cData[dev->interface].txStream, ENABLE);

    /* Enable the I2C peripheral DMA requests */
    I2C_DMACmd(i2cData[dev->interface].i2c, ENABLE);
}

uint8_t i2cWaitStandbyState(i2cSlave_t* dev) {
    __IO uint16_t tmpSR1 = 0;
    __IO uint32_t trials = 0;

    /*!< While the bus is busy */
    while (I2C_GetFlagStatus(i2cData[dev->interface].i2c, I2C_FLAG_BUSY));

    /* Keep looping till the slave acknowledge his address or maximum number
       of trials is reached (this number is defined by sEE_MAX_TRIALS_NUMBER define
       in STM324x7I_eval_i2c_ee.h file) */
    while (1) {
        /*!< Send START condition */
        I2C_GenerateSTART(i2cData[dev->interface].i2c, ENABLE);

        /*!< Test on EV5 and clear it */
        while (!I2C_CheckEvent(i2cData[dev->interface].i2c, I2C_EVENT_MASTER_MODE_SELECT));

        /*!< Send EEPROM address for write */
        I2C_Send7bitAddress(i2cData[dev->interface].i2c, dev->address, I2C_Direction_Transmitter);

        /* Wait for ADDR flag to be set (Slave acknowledged his address) */
        do {
            /* Get the current value of the SR1 register */
            tmpSR1 = i2cData[dev->interface].i2c->SR1;
        } while ((tmpSR1 & (I2C_SR1_ADDR | I2C_SR1_AF)) == 0);
        /* Keep looping till the Address is acknowledged or the AF flag is
           set (address not acknowledged at time) */

        /* Check if the ADDR flag has been set */
        if (tmpSR1 & I2C_SR1_ADDR) {
            /* Clear ADDR Flag by reading SR1 then SR2 registers (SR1 have already
               been read) */
            (void)i2cData[dev->interface].i2c->SR2;

            /*!< STOP condition */
            I2C_GenerateSTOP(i2cData[dev->interface].i2c, ENABLE);

            /* Exit the function */
            return 0;
        } else {
            /*!< Clear AF flag */
            I2C_ClearFlag(i2cData[dev->interface].i2c, I2C_FLAG_AF);
        }
        yield(1);  // EEPROM is slow
        /* Check if the maximum allowed number of trials has bee reached */
        if (trials++ == I2C_MAX_TRIALS_NUMBER) {
            /* If the maximum number of trials has been reached, exit the function */
            I2C_GenerateSTOP(i2cData[dev->interface].i2c, ENABLE);
            return 1;
        }
    }
}

i2cSlave_t* i2cRegisterSlaveDevice(I2C_TypeDef* interface, uint8_t address, i2cCallback_t *callback) {
    uint8_t i, t = 255;
#ifdef I2C_I2C1
    if (interface == I2C1) {
        i2c1Init();
        t = 0;
    }
#endif
#ifdef I2C_I2C2
    else if (interface == I2C2) {
        i2c2Init();
        t = 1;
    }
#endif
#ifdef I2C_I2C3
    else if (interface == I2C3) {
        i2c3Init();
        t = 2;
    }
#endif
    if (t == 255) {
        return NULL;
    }

    for (i = 0; i < NO_OF_I2C_SLAVE; i++) {
        i2cSlave_t* s = &(slaves[i]);
        if (s->address == 0) {
            // current i2c slave address is not used, register new one
            s->address = address;
            s->interface = t;
            if (callback)
                s->callback = callback;
            else
                s->callback = i2cDefaultCallback;
            return s;
        } else if (s->address == address && s->interface == t) {
            // TODO handle I2C address conflict
            s = NULL;
        } else {
            s = NULL;
        }
    }
    return NULL;
}

#ifdef I2C_I2C1_DMA_RX_HANDLER
void I2C_I2C1_DMA_RX_HANDLER(void) {
    /* Check if the DMA transfer is complete */
    if (DMA_GetFlagStatus(I2C_I2C1_DMA_RX, I2C_I2C1_DMA_RX_FLAG_TCIF) != RESET) {
        i2cCallback_t *dmaCompleted;
        /*!< Send STOP Condition */
        I2C_GenerateSTOP(I2C_I2C1, ENABLE);    
        /* Disable the DMA Rx Stream and Clear TC Flag */  
        DMA_Cmd(I2C_I2C1_DMA_RX, DISABLE);
        DMA_ClearFlag(I2C_I2C1_DMA_RX, I2C_I2C1_DMA_RX_FLAG_TCIF);
        I2C_AcknowledgeConfig(I2C_I2C1, ENABLE);
        i2cData[0].dmaRunning = 0;
        dmaCompleted = i2cData[0].activeSlave->callback;
        i2cData[0].activeSlave = NULL;
        dmaCompleted(0);
    }
}
#endif /* I2C_I2C1_DMA_RX_HANDLER */

#ifdef I2C_I2C1_DMA_TX_HANDLER
void I2C_I2C1_DMA_TX_HANDLER(void) {
    /* Check if the DMA transfer is complete */
    if (DMA_GetFlagStatus(I2C_I2C1_DMA_TX, I2C_I2C1_DMA_TX_FLAG_TCIF) != RESET) {
        i2cCallback_t *dmaCompleted;
        /* Disable the DMA Tx Stream and Clear TC flag */  
        DMA_Cmd(I2C_I2C1_DMA_TX, DISABLE);
        DMA_ClearFlag(I2C_I2C1_DMA_TX, I2C_I2C1_DMA_TX_FLAG_TCIF);
        /*!< Wait till all data have been physically transferred on the bus */
        while (!I2C_GetFlagStatus(I2C_I2C1, I2C_FLAG_BTF));
        /*!< Send STOP condition */
        I2C_GenerateSTOP(I2C_I2C1, ENABLE);
        i2cData[0].dmaRunning = 0;
        dmaCompleted = i2cData[0].activeSlave->callback;
        i2cData[0].activeSlave = NULL;
        dmaCompleted(0);
    }
}
#endif /* I2C_I2C1_DMA_TX_HANDLER */

#ifdef I2C_I2C2_DMA_RX_HANDLER
void I2C_I2C2_DMA_RX_HANDLER(void) {
    /* Check if the DMA transfer is complete */
    if (DMA_GetFlagStatus(I2C_I2C2_DMA_RX, I2C_I2C2_DMA_RX_FLAG_TCIF) != RESET) {
        i2cCallback_t *dmaCompleted;
        /*!< Send STOP Condition */
        I2C_GenerateSTOP(I2C_I2C2, ENABLE);    
        /* Disable the DMA Rx Stream and Clear TC Flag */  
        DMA_Cmd(I2C_I2C2_DMA_RX, DISABLE);
        DMA_ClearFlag(I2C_I2C2_DMA_RX, I2C_I2C1_DMA_RX_FLAG_TCIF);
        i2cData[1].dmaRunning = 0;
        dmaCompleted = i2cData[1].activeSlave->callback;
        i2cData[1].activeSlave = NULL;
        dmaCompleted(0);
    }
}
#endif /* I2C_I2C2_DMA_RX_HANDLER */

#ifdef I2C_I2C2_DMA_TX_HANDLER
void I2C_I2C2_DMA_TX_HANDLER(void) {
    /* Check if the DMA transfer is complete */
    if (DMA_GetFlagStatus(I2C_I2C2_DMA_TX, I2C_I2C2_DMA_TX_FLAG_TCIF) != RESET) {
        i2cCallback_t *dmaCompleted;
        /* Disable the DMA Tx Stream and Clear TC flag */  
        DMA_Cmd(I2C_I2C2_DMA_TX, DISABLE);
        DMA_ClearFlag(I2C_I2C2_DMA_TX, I2C_I2C2_DMA_TX_FLAG_TCIF);
        /*!< Wait till all data have been physically transferred on the bus */
        while (!I2C_GetFlagStatus(I2C_I2C2, I2C_FLAG_BTF));
        /*!< Send STOP condition */
        I2C_GenerateSTOP(I2C_I2C2, ENABLE);
        i2cData[1].dmaRunning = 0;
        dmaCompleted = i2cData[1].activeSlave->callback;
        i2cData[1].activeSlave = NULL;
        dmaCompleted(0);
    }
}
#endif /* I2C_I2C2_DMA_TX_HANDLER */

#ifdef I2C_I2C3_DMA_RX_HANDLER
void I2C_I2C3_DMA_RX_HANDLER(void) {
    /* Check if the DMA transfer is complete */
    if (DMA_GetFlagStatus(I2C_I2C3_DMA_RX, I2C_I2C3_DMA_RX_FLAG_TCIF) != RESET) {
        i2cCallback_t *dmaCompleted;
        /*!< Send STOP Condition */
        I2C_GenerateSTOP(I2C_I2C3, ENABLE);    
        /* Disable the DMA Rx Stream and Clear TC Flag */  
        DMA_Cmd(I2C_I2C3_DMA_RX, DISABLE);
        DMA_ClearFlag(I2C_I2C3_DMA_RX, I2C_I2C3_DMA_RX_FLAG_TCIF);
        i2cData[2].dmaRunning = 0;
        dmaCompleted = i2cData[2].activeSlave->callback;
        i2cData[2].activeSlave = NULL;
        dmaCompleted(0);
    }
}
#endif /* I2C_I2C3_DMA_RX_HANDLER */

#ifdef I2C_I2C3_DMA_TX_HANDLER
void I2C_I2C3_DMA_TX_HANDLER(void) {
    /* Check if the DMA transfer is complete */
    if (DMA_GetFlagStatus(I2C_I2C3_DMA_TX, I2C_I2C3_DMA_TX_FLAG_TCIF) != RESET) {
        i2cCallback_t *dmaCompleted;
        /* Disable the DMA Tx Stream and Clear TC flag */  
        DMA_Cmd(I2C_I2C3_DMA_TX, DISABLE);
        DMA_ClearFlag(I2C_I2C3_DMA_TX, I2C_I2C3_DMA_TX_FLAG_TCIF);
        /*!< Wait till all data have been physically transferred on the bus */
        while (!I2C_GetFlagStatus(I2C_I2C3, I2C_FLAG_BTF));
        /*!< Send STOP condition */
        I2C_GenerateSTOP(I2C_I2C3, ENABLE);
        i2cData[2].dmaRunning = 0;
        dmaCompleted = i2cData[2].activeSlave->callback;
        i2cData[2].activeSlave = NULL;
        dmaCompleted(0);
    }
}
#endif /* I2C_I2C3_DMA_TX_HANDLER */

// mag3110 get reg
void i2cReadOneByte(i2cSlave_t* dev, uint16_t reg, volatile uint8_t* data) {
    while (I2C_GetFlagStatus(i2cData[dev->interface].i2c, I2C_FLAG_BUSY));
    I2C_GenerateSTART(i2cData[dev->interface].i2c, ENABLE);
    while (!I2C_CheckEvent(i2cData[dev->interface].i2c, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(i2cData[dev->interface].i2c, dev->address, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(i2cData[dev->interface].i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    I2C_SendData(i2cData[dev->interface].i2c, reg);
    while (!I2C_CheckEvent(i2cData[dev->interface].i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_GenerateSTART(i2cData[dev->interface].i2c, ENABLE);
    while (!I2C_CheckEvent(i2cData[dev->interface].i2c, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(i2cData[dev->interface].i2c, dev->address, I2C_Direction_Receiver);
    while (!I2C_CheckEvent(i2cData[dev->interface].i2c, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    I2C_AcknowledgeConfig(i2cData[dev->interface].i2c, DISABLE);
    (void)i2cData[dev->interface].i2c->SR2;
    I2C_GenerateSTOP(i2cData[dev->interface].i2c, ENABLE);
    while (I2C_GetFlagStatus(i2cData[dev->interface].i2c, I2C_FLAG_RXNE) == RESET);
    data[0] = I2C_ReceiveData(i2cData[dev->interface].i2c);
    while (i2cData[dev->interface].i2c->CR1 & I2C_CR1_STOP);
    I2C_AcknowledgeConfig(i2cData[dev->interface].i2c, ENABLE);
}

// mag3110 set reg
void i2cWriteOneByte(i2cSlave_t* dev, uint16_t reg, uint8_t data) {
    I2C_GenerateSTART(i2cData[dev->interface].i2c, ENABLE);
    while(!I2C_CheckEvent(i2cData[dev->interface].i2c, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(i2cData[dev->interface].i2c, dev->address, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(i2cData[dev->interface].i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    I2C_SendData(i2cData[dev->interface].i2c, reg);
    while(!I2C_CheckEvent(i2cData[dev->interface].i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_SendData(i2cData[dev->interface].i2c, data);
    while(!I2C_CheckEvent(i2cData[dev->interface].i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_GenerateSTOP(i2cData[dev->interface].i2c, ENABLE);
}
