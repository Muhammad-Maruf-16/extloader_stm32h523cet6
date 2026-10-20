#ifndef __DEV_INF_H
#define __DEV_INF_H

#include <stdint.h>

#define MCU_FLASH       1
#define NAND_FLASH      2
#define NOR_FLASH       3
#define SRAM            4
#define PSRAM           5
#define PC_CARD         6
#define SPI_FLASH       7
#define I2C_FLASH       8
#define SDRAM           9
#define I2C_EEPROM      10

#define SECTOR_NUM      10

struct DeviceSectors {
    uint32_t SectorNumber;
    uint32_t SectorSize;
};

struct StorageInfo {
    char          DeviceName[100];
    uint16_t      DeviceType;
    uint16_t      Reserved1;
    uint32_t      DeviceStartAddress;
    uint32_t      DeviceSize;
    uint32_t      PageSize;
    uint8_t       EraseValue;
    uint8_t       Reserved2[3];
    struct DeviceSectors Sectors[SECTOR_NUM];
};

#endif /* __DEV_INF_H */
