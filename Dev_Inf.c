#include "Dev_Inf.h"

__attribute__((section(".Dev_Info"), used))
struct StorageInfo const StorageInfo = {
    "MX25L_STM32H523CET6",          // Device Name and Description (max 100 bytes)
    NOR_FLASH,                      // Device Type (3 = NOR_FLASH)
    0,                              // Reserved
    0x90000000,                     // Device Start Address (OctoSPI MM base)
    0x02000000,                     // Total Size of Device in Bytes (32 MBytes)
    0x00000100,                     // Programming Page Size in Bytes (256 Bytes)
    0xFF,                           // Initial Content of Erased Memory
    { 0, 0, 0 },                    // Reserved
    {
        { 8192, 4096 },             // 8192 sectors of 4096 Bytes = 32 MBytes
        { 0, 0 }                    // End of sector definitions
    }
};
