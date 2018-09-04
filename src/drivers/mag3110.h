#ifndef _mag3110_h
#define _mag3110_h

#include "i2c.h"

#define MAG3110_BYTES       6
#define MAG3110_SLOT_SIZE ((MAG3110_BYTES+sizeof(int)-1) / sizeof(int) * sizeof(int))
//#define MAG3110_SLOTS       1
#define MAG3110_SLOTS       2  // 40Hz bandwidth
#define MAG3110_RETRIES     5

typedef struct {
    i2cSlave_t *i2cBus;
    volatile uint8_t rxBuf[MAG3110_SLOT_SIZE * MAG3110_SLOTS];
    volatile uint8_t slot;
    float rawMag[3];
    float mag[3];
    float magSign[3];
    volatile uint32_t lastUpdate;
    uint8_t readCmd;
    uint8_t enabled;
    uint8_t initialized;
} mag3110Struct_t;

extern mag3110Struct_t mag3110Data;

extern void mag3110PreInit(void);
extern uint8_t mag3110Init(void);
extern void mag3110Decode(void);
extern void mag3110Enable(void);
extern void mag3110Disable(void);

#endif
