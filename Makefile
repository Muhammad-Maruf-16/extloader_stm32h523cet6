CC = arm-none-eabi-gcc
SIZE = arm-none-eabi-size
READELF = arm-none-eabi-readelf

TARGET = MX25L_STM32H523CET6.stldr

SRCS = Dev_Inf.c Loader_Src.c
OBJS = $(SRCS:.c=.o)

CMSIS_INC1 = ../tsamota stm32h523cet6/Drivers/CMSIS/Device/ST/STM32H5xx/Include
CMSIS_INC2 = ../tsamota stm32h523cet6/Drivers/CMSIS/Include

CFLAGS = -mcpu=cortex-m33 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard \
         -Os -Wall -fno-builtin -ffunction-sections -fdata-sections \
         -DSTM32H523xx -I. -I"$(CMSIS_INC1)" -I"$(CMSIS_INC2)"

LDFLAGS = -mcpu=cortex-m33 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard \
          -T linker.ld -nostartfiles -nodefaultlibs -Wl,--gc-sections \
          -Wl,-Map=$(TARGET).map

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)
	@echo "Build successful: $@"
	$(SIZE) $@
	@echo "Program Headers:"
	$(READELF) -l $@

Dev_Inf.o: Dev_Inf.c Dev_Inf.h
	$(CC) $(CFLAGS) -c Dev_Inf.c -o Dev_Inf.o

Loader_Src.o: Loader_Src.c Dev_Inf.h
	$(CC) $(CFLAGS) -c Loader_Src.c -o Loader_Src.o

install: $(TARGET)
	cp $(TARGET) /home/maruf/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/ExternalLoader/
	echo "1" | sudo -S cp $(TARGET) /opt/st/stm32cubeide_2.2.0_2/plugins/com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.linux64_2.2.500.202603051304/tools/bin/ExternalLoader/
	@echo "Installed $(TARGET) to STM32CubeProgrammer and STM32CubeIDE successfully!"

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).map

.PHONY: all clean install

