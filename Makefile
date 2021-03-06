# Hey Emacs, this is a -*- Makefile -*-

# Makefile for AutoQuad Flight Controller firmware
# All paths are relative to Makefile location. Possible make targets:
#  all    build firmware .elf and .bin binaries
#  swd    attempt to flash firmware via ST-Link (linux only)
#  dfu    attempt to flash firmware via DFU-Util
#  sfl    attempt to flash firmware via serial bootloader
#  clean  delete all built objects (not binaries or archives)

PROJ:=aq_gcc_v7.0_hwv
TARGET:=$(PROJ)

TOOLCHAIN_PREFIX:=arm-none-eabi

CC=$(TOOLCHAIN_PREFIX)-gcc
LD=$(TOOLCHAIN_PREFIX)-gcc
OBJCOPY=$(TOOLCHAIN_PREFIX)-objcopy
AS=$(TOOLCHAIN_PREFIX)-as
AR=$(TOOLCHAIN_PREFIX)-ar
GDB=$(TOOLCHAIN_PREFIX)-gdb

PROJ_ROOT?=$(CURDIR)

# Default for the SimpleFC hardware
BRD_VER?=9
BRD_REV?=1

# Un-comment this to enable DIMU for AQ6
#DIMU_VER:=11
DIMU_VER?=0
TARGET:=$(PROJ)$(BRD_VER).$(BRD_REV)

BUILD_TYPE?=Release
DEBUG_DEFS:=

ifeq ($(findstring Debug, $(BUILD_TYPE)), Debug)
BUILD_TYPE=Debug
BUILD_DIR=$(PROJ_ROOT)/build/Debug/obj
BIN_DIR=$(PROJ_ROOT)/build/Debug
# TODO test only
OPTLVL=2
DBG=-g3
DEBUG_DEFS=-DDEBUG_ENABLED
else
BUILD_DIR=$(PROJ_ROOT)/build/Release/obj
BIN_DIR=$(PROJ_ROOT)/build/Release
OPTLVL=2
DBG=
endif

STARTUP:=$(PROJ_ROOT)/src/targets
LINKER_SCRIPT:=$(PROJ_ROOT)/src/targets/stm32f40x_flash.ld

INCLUDE=-I$(PROJ_ROOT)/src
INCLUDE+=-I$(PROJ_ROOT)/src/co_os
INCLUDE+=-I$(PROJ_ROOT)/src/drivers
INCLUDE+=-I$(PROJ_ROOT)/src/drivers/can
INCLUDE+=-I$(PROJ_ROOT)/src/drivers/disk
INCLUDE+=-I$(PROJ_ROOT)/src/drivers/usb
INCLUDE+=-I$(PROJ_ROOT)/src/math
INCLUDE+=-I$(PROJ_ROOT)/src/radio
INCLUDE+=-I$(PROJ_ROOT)/src/targets
INCLUDE+=-I$(PROJ_ROOT)/lib/CMSIS/Device/ST/STM32F4xx/Include
INCLUDE+=-I$(PROJ_ROOT)/lib/CMSIS/Include
INCLUDE+=-I$(PROJ_ROOT)/lib/STM32F4xx_StdPeriph_Driver/inc
INCLUDE+=-I$(PROJ_ROOT)/lib/mavlink/include/autoquad

SRC_DIR=$(PROJ_ROOT)/src
SRC_DIR+=$(PROJ_ROOT)/src/co_os
SRC_DIR+=$(PROJ_ROOT)/src/drivers
SRC_DIR+=$(PROJ_ROOT)/src/drivers/can
SRC_DIR+=$(PROJ_ROOT)/src/drivers/disk
SRC_DIR+=$(PROJ_ROOT)/src/drivers/usb
SRC_DIR+=$(PROJ_ROOT)/src/math
SRC_DIR+=$(PROJ_ROOT)/src/radio
SRC_DIR+=$(PROJ_ROOT)/src/targets
SRC_DIR+=$(PROJ_ROOT)/lib/STM32F4xx_StdPeriph_Driver/src
SRC_DIR+=$(PROJ_ROOT)/lib/CMSIS/DSP_Lib/Source/BasicMathFunctions
SRC_DIR+=$(PROJ_ROOT)/lib/CMSIS/DSP_Lib/Source/ComplexMathFunctions
SRC_DIR+=$(PROJ_ROOT)/lib/CMSIS/DSP_Lib/Source/FastMathFunctions
SRC_DIR+=$(PROJ_ROOT)/lib/CMSIS/DSP_Lib/Source/MatrixFunctions
SRC_DIR+=$(PROJ_ROOT)/lib/CMSIS/DSP_Lib/Source/SupportFunctions
SRC_DIR+=$(PROJ_ROOT)/lib/CMSIS/DSP_Lib/Source/CommonTables
SRC_DIR+=$(PROJ_ROOT)/lib/CMSIS/DSP_Lib/Source/ControllerFunctions
SRC_DIR+=$(PROJ_ROOT)/lib/CMSIS/DSP_Lib/Source/FilteringFunctions
SRC_DIR+=$(PROJ_ROOT)/lib/CMSIS/DSP_Lib/Source/StatisticsFunctions
SRC_DIR+=$(PROJ_ROOT)/lib/CMSIS/DSP_Lib/Source/TransformFunctions

vpath %.c $(SRC_DIR)
vpath %.s $(STARTUP)

# Startup file
ASRC=startup_stm32f4xx.s

# Project Source Files
SRC=main_ctl.c
SRC+=esc32.c
SRC+=gimbal.c
SRC+=motors.c
SRC+=pwm.c
SRC+=1wire.c
SRC+=analog.c
SRC+=aq_timer.c
SRC+=can.c
SRC+=canCalib.c
SRC+=canOSD.c
SRC+=canSensors.c
SRC+=canUart.c
SRC+=digital.c
SRC+=ext_irq.c
SRC+=fpu.c
SRC+=rcc.c
SRC+=rtc.c
SRC+=sdio.c
SRC+=serial.c
SRC+=spi.c
SRC+=i2c.c
SRC+=usb.c
SRC+=usbd_cdc_msc_core.c
SRC+=usbd_core.c
SRC+=usbd_desc.c
SRC+=usbd_ioreq.c
SRC+=usbd_msc_bot.c
SRC+=usbd_msc_data.c
SRC+=usbd_msc_scsi.c
SRC+=usbd_req.c
SRC+=usbd_storage_msd.c
SRC+=usb_bsp.c
SRC+=usb_core.c
SRC+=usb_dcd.c
SRC+=usb_dcd_int.c
SRC+=util.c
SRC+=config.c
SRC+=flash.c
SRC+=arch.c
SRC+=core.c
SRC+=event.c
SRC+=flag.c
SRC+=kernelHeap.c
SRC+=mbox.c
SRC+=mm.c
SRC+=mutex.c
SRC+=port.c
SRC+=queue.c
SRC+=sem.c
SRC+=serviceReq.c
SRC+=task.c
SRC+=time.c
SRC+=timer.c
SRC+=utility.c
SRC+=ff.c
SRC+=control.c
SRC+=pid.c
SRC+=gps.c
SRC+=ublox.c
SRC+=adc.c
SRC+=d_imu.c
SRC+=eeprom.c
SRC+=hmc5983.c
SRC+=max21100.c
SRC+=mpu6000.c
SRC+=ms5611.c
SRC+=mag3110.c
SRC+=imu.c
SRC+=alt_ukf.c
SRC+=nav.c
SRC+=nav_ukf.c
SRC+=filer.c
SRC+=logger.c
SRC+=algebra.c
SRC+=wmm.c
SRC+=calib.c
SRC+=compass.c
SRC+=rotations.c
SRC+=srcdkf.c
SRC+=signaling.c
SRC+=cyrf6936.c
SRC+=dsm.c
SRC+=futaba.c
SRC+=grhott.c
SRC+=mlinkrx.c
SRC+=ppm.c
SRC+=radio.c
SRC+=spektrum.c
SRC+=aq_init.c
SRC+=run.c
SRC+=supervisor.c
SRC+=aq_mavlink.c
SRC+=comm.c
SRC+=command.c
SRC+=telemetry.c
SRC+=rc.c

# DSP library
SRC+=arm_copy_f32.c
SRC+=arm_mat_init_f32.c
SRC+=arm_mat_sub_f32.c
SRC+=arm_scale_f32.c
SRC+=arm_fill_f32.c
SRC+=arm_mat_inverse_f32.c
SRC+=arm_mat_trans_f32.c
SRC+=arm_std_f32.c
SRC+=arm_mat_add_f32.c
SRC+=arm_mat_mult_f32.c
SRC+=arm_mean_f32.c

SRC+=system_stm32f4xx.c
SRC+=syscalls.c

# Standard Peripheral Source Files
SRC+=misc.c
SRC+=stm32f4xx_adc.c
SRC+=stm32f4xx_dbgmcu.c
SRC+=stm32f4xx_spi.c
SRC+=stm32f4xx_can.c
SRC+=stm32f4xx_pwr.c
SRC+=stm32f4xx_syscfg.c
SRC+=stm32f4xx_dma.c
SRC+=stm32f4xx_gpio.c
SRC+=stm32f4xx_tim.c
SRC+=stm32f4xx_rcc.c
SRC+=stm32f4xx_usart.c
SRC+=stm32f4xx_exti.c
SRC+=stm32f4xx_rtc.c
SRC+=stm32f4xx_flash.c
SRC+=stm32f4xx_i2c.c
SRC+=stm32f4xx_sdio.c
# SRC+=stm32f4xx_dac.c
# SRC+=stm32f4xx_fmc.c
# SRC+=stm32f4xx_lptim.c
# SRC+=stm32f4xx_spdifrx.c
# SRC+=stm32f4xx_fmpi2c.c
# SRC+=stm32f4xx_ltdc.c
# SRC+=stm32f4xx_dcmi.c
# SRC+=stm32f4xx_fsmc.c
# SRC+=stm32f4xx_cec.c
# SRC+=stm32f4xx_qspi.c
# SRC+=stm32f4xx_crc.c
# SRC+=stm32f4xx_dma2d.c
# SRC+=stm32f4xx_hash.c
# SRC+=stm32f4xx_cryp.c
# SRC+=stm32f4xx_dsi.c
# SRC+=stm32f4xx_hash_md5.c
# SRC+=stm32f4xx_rng.c
# SRC+=stm32f4xx_wwdg.c
# SRC+=stm32f4xx_cryp_aes.c
# SRC+=stm32f4xx_hash_sha1.c
# SRC+=stm32f4xx_cryp_des.c
# SRC+=stm32f4xx_sai.c
# SRC+=stm32f4xx_cryp_tdes.c
# SRC+=stm32f4xx_flash_ramfunc.c
# SRC+=stm32f4xx_iwdg.c

CDEFS=-DUSE_STDPERIPH_DRIVER
CDEFS+=-DSTM32F4XX
CDEFS+=-DSTM32F40_41xxx
CDEFS+=-DHSE_VALUE=25000000
CDEFS+=-D__FPU_PRESENT=1
CDEFS+=-D__FPU_USED=1
CDEFS+=-DARM_MATH_CM4
CDEFS+=-DBOARD_VERSION=$(BRD_VER)
CDEFS+=-DBOARD_REVISION=$(BRD_REV)
CDEFS+=-DDIMU_VERSION=$(DIMU_VER)
CDEFS+=$(DEBUG_DEFS)
ifeq ($(findstring 2, $(MAV_VER)), 2)
CDEFS+=-DMAVLINK_V2
endif

MCUFLAGS=-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant -finline-functions -Wdouble-promotion -std=gnu99
COMMONFLAGS=-O$(OPTLVL) $(DBG) -Wall -ffunction-sections -fdata-sections
CFLAGS=$(COMMONFLAGS) $(MCUFLAGS) $(INCLUDE) $(CDEFS)

LDLIBS=-lm -lc -lgcc
LDFLAGS=$(MCUFLAGS) -u _scanf_float -u _printf_float -fno-exceptions -Wl,--gc-sections,-T$(LINKER_SCRIPT)

OBJ = $(SRC:%.c=$(BUILD_DIR)/%.o)
DEP := $(OBJ:.o=.d)

.PHONY: clean all elf bin
.IGNORE: sym

all: elf

ifeq ($(findstring clean, $(MAKECMDGOALS)),)
-include $(DEP)
endif

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)
	@echo [CC] $(notdir $<)
	@$(CC) $(CFLAGS) -MMD -MP -MF $(@:%.o=%.d) -MT $(@) -c -o $@ $<

#bin: elf

elf: $(OBJ)
	@echo [AS] $(notdir $(STARTUP)/$(ASRC))
	@$(AS) -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -o $(ASRC:%.s=$(BUILD_DIR)/%.o) $(STARTUP)/$(ASRC)
	@echo [LD] $(notdir $(BIN_DIR)/$(TARGET).elf)
	@$(LD) $(LDFLAGS) -o $(BIN_DIR)/$(TARGET).elf $(OBJ) $(ASRC:%.s=$(BUILD_DIR)/%.o) $(LDLIBS)
	@echo [BIN] $(notdir $(BIN_DIR)/$(TARGET).bin)
	@$(OBJCOPY) -O binary $(BIN_DIR)/$(TARGET).elf $(BIN_DIR)/$(TARGET).bin

sym:
	@echo $(INCLUDE)
	@echo $(CDEFS)
	@echo $(OBJ)
	@echo $(DEP)

clean:
	@echo [RM] Objects
	@rm -f $(OBJ)
	@rm -f $(DEP)
	@rm -f $(ASRC:%.s=$(BUILD_DIR)/%.o)
	@echo [RM] Binaries
	@rm -f $(BIN_DIR)/$(TARGET).elf
	@rm -f $(BIN_DIR)/$(TARGET).bin

# Flash firmware via ST-Link
# more at https://github.com/texane/stlink
swd: all
	@st-flash --reset write $(BIN_DIR)/$(TARGET).bin 0x8000000

# Flash firmware via DFU util, need to put stm32 into bootloader mode
# more at http://wiki.openmoko.org/wiki/Dfu-util
dfu: all
	@dfu-util -a 0 -d 0483:df11 -s 0x08000000:leave -R -D $(BIN_DIR)/$(TARGET).bin

# Flash firmware via USART1, neet to put stm32 into bootloader mode
# more at https://sourceforge.net/p/stm32flash/wiki/Home/
sfl: all
	@stm32flash -b 115200 -w $(BIN_DIR)/$(TARGET).bin -s 0x08000000 -v /dev/ttyUSB1
