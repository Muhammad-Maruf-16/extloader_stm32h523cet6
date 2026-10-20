/**
  ******************************************************************************
  * @file           : Loader_Src.c
  * @brief          : External Loader algorithm functions for STM32CubeProgrammer
  *                   Target MCU : STM32H523CET6
  *                   Target Flash: Macronix Quad-SPI (32 MB) @ 0x90000000
  ******************************************************************************
  */

#include "stm32h5xx.h"
#include <stdint.h>

#define LOADER_OK       1
#define LOADER_FAIL     0

/* Helper routines */
static void OCTOSPI1_GPIO_Config(void)
{
    /* Enable GPIOA and GPIOB clocks */
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN;
    __DSB();

    /* PA1 -> OCTOSPI1_IO3 (AF9) */
    GPIOA->MODER   = (GPIOA->MODER   & ~GPIO_MODER_MODE1_Msk)   | (2U << GPIO_MODER_MODE1_Pos);
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT1;
    GPIOA->OSPEEDR = (GPIOA->OSPEEDR & ~GPIO_OSPEEDR_OSPEED1_Msk) | (3U << GPIO_OSPEEDR_OSPEED1_Pos);
    GPIOA->PUPDR   = (GPIOA->PUPDR   & ~GPIO_PUPDR_PUPD1_Msk);
    GPIOA->AFR[0]  = (GPIOA->AFR[0]  & ~(0xFU << 4))            | (9U << 4);

    /* PA3 -> OCTOSPI1_CLK (AF3) */
    GPIOA->MODER   = (GPIOA->MODER   & ~GPIO_MODER_MODE3_Msk)   | (2U << GPIO_MODER_MODE3_Pos);
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT3;
    GPIOA->OSPEEDR = (GPIOA->OSPEEDR & ~GPIO_OSPEEDR_OSPEED3_Msk) | (3U << GPIO_OSPEEDR_OSPEED3_Pos);
    GPIOA->PUPDR   = (GPIOA->PUPDR   & ~GPIO_PUPDR_PUPD3_Msk);
    GPIOA->AFR[0]  = (GPIOA->AFR[0]  & ~(0xFU << 12))           | (3U << 12);

    /* PA7 -> OCTOSPI1_IO2 (AF10) */
    GPIOA->MODER   = (GPIOA->MODER   & ~GPIO_MODER_MODE7_Msk)   | (2U << GPIO_MODER_MODE7_Pos);
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT7;
    GPIOA->OSPEEDR = (GPIOA->OSPEEDR & ~GPIO_OSPEEDR_OSPEED7_Msk) | (3U << GPIO_OSPEEDR_OSPEED7_Pos);
    GPIOA->PUPDR   = (GPIOA->PUPDR   & ~GPIO_PUPDR_PUPD7_Msk);
    GPIOA->AFR[0]  = (GPIOA->AFR[0]  & ~(0xFU << 28))           | (10U << 28);

    /* PB0 -> OCTOSPI1_IO1 (AF6) */
    GPIOB->MODER   = (GPIOB->MODER   & ~GPIO_MODER_MODE0_Msk)   | (2U << GPIO_MODER_MODE0_Pos);
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT0;
    GPIOB->OSPEEDR = (GPIOB->OSPEEDR & ~GPIO_OSPEEDR_OSPEED0_Msk) | (3U << GPIO_OSPEEDR_OSPEED0_Pos);
    GPIOB->PUPDR   = (GPIOB->PUPDR   & ~GPIO_PUPDR_PUPD0_Msk);
    GPIOB->AFR[0]  = (GPIOB->AFR[0]  & ~(0xFU << 0))            | (6U << 0);

    /* PB1 -> OCTOSPI1_IO0 (AF6) */
    GPIOB->MODER   = (GPIOB->MODER   & ~GPIO_MODER_MODE1_Msk)   | (2U << GPIO_MODER_MODE1_Pos);
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT1;
    GPIOB->OSPEEDR = (GPIOB->OSPEEDR & ~GPIO_OSPEEDR_OSPEED1_Msk) | (3U << GPIO_OSPEEDR_OSPEED1_Pos);
    GPIOB->PUPDR   = (GPIOB->PUPDR   & ~GPIO_PUPDR_PUPD1_Msk);
    GPIOB->AFR[0]  = (GPIOB->AFR[0]  & ~(0xFU << 4))            | (6U << 4);

    /* PB10 -> OCTOSPI1_NCS (AF9) */
    GPIOB->MODER   = (GPIOB->MODER   & ~GPIO_MODER_MODE10_Msk)  | (2U << GPIO_MODER_MODE10_Pos);
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT10;
    GPIOB->OSPEEDR = (GPIOB->OSPEEDR & ~GPIO_OSPEEDR_OSPEED10_Msk)| (3U << GPIO_OSPEEDR_OSPEED10_Pos);
    GPIOB->PUPDR   = (GPIOB->PUPDR   & ~GPIO_PUPDR_PUPD10_Msk);
    GPIOB->AFR[1]  = (GPIOB->AFR[1]  & ~(0xFU << 8))            | (9U << 8);
}

static void OCTOSPI1_SetIndirect(void)
{
    OCTOSPI1->CR &= ~OCTOSPI_CR_EN;
    while (OCTOSPI1->CR & OCTOSPI_CR_EN);

    OCTOSPI1->DCR1 = (24U << OCTOSPI_DCR1_DEVSIZE_Pos)
                   | (1U  << OCTOSPI_DCR1_CSHT_Pos);

    OCTOSPI1->DCR2 = (1U  << OCTOSPI_DCR2_PRESCALER_Pos);

    OCTOSPI1->TCR  = (0U  << OCTOSPI_TCR_DCYC_Pos)
                   | (0U  << OCTOSPI_TCR_SSHIFT_Pos);

    OCTOSPI1->CR = (OCTOSPI1->CR & ~OCTOSPI_CR_FMODE_Msk);
    OCTOSPI1->CR |= OCTOSPI_CR_EN;
    while (!(OCTOSPI1->CR & OCTOSPI_CR_EN));
}

static void OCTOSPI1_SetMemoryMapped(void)
{
    OCTOSPI1->CR &= ~OCTOSPI_CR_EN;
    while (OCTOSPI1->CR & OCTOSPI_CR_EN);

    OCTOSPI1->DCR1 = (24U << OCTOSPI_DCR1_DEVSIZE_Pos)
                   | (1U  << OCTOSPI_DCR1_CSHT_Pos);

    OCTOSPI1->DCR2 = (1U  << OCTOSPI_DCR2_PRESCALER_Pos);

    OCTOSPI1->TCR  = (0U  << OCTOSPI_TCR_DCYC_Pos)
                   | (0U  << OCTOSPI_TCR_SSHIFT_Pos);

    OCTOSPI1->CCR = (1U << OCTOSPI_CCR_IMODE_Pos)
                  | (1U << OCTOSPI_CCR_ADMODE_Pos)
                  | (2U << OCTOSPI_CCR_ADSIZE_Pos)
                  | (1U << OCTOSPI_CCR_DMODE_Pos);

    OCTOSPI1->IR = 0x03U;

    OCTOSPI1->CR = (OCTOSPI1->CR & ~OCTOSPI_CR_FMODE_Msk)
                 | (3U << OCTOSPI_CR_FMODE_Pos);

    OCTOSPI1->CR |= OCTOSPI_CR_EN;
    while (!(OCTOSPI1->CR & OCTOSPI_CR_EN));
}

static void Flash_WaitBusy(void)
{
    uint8_t status;
    do {
        while (OCTOSPI1->SR & OCTOSPI_SR_BUSY);
        OCTOSPI1->DLR = 0U;
        OCTOSPI1->CCR = (1U << OCTOSPI_CCR_IMODE_Pos)
                      | (1U << OCTOSPI_CCR_DMODE_Pos);
        OCTOSPI1->CR  = (OCTOSPI1->CR & ~OCTOSPI_CR_FMODE_Msk)
                      | (1U << OCTOSPI_CR_FMODE_Pos);
        OCTOSPI1->IR  = 0x05U;

        while (!(OCTOSPI1->SR & OCTOSPI_SR_FTF) &&
               !(OCTOSPI1->SR & OCTOSPI_SR_TCF));
        status = *(__IO uint8_t *)(&OCTOSPI1->DR);
        while (!(OCTOSPI1->SR & OCTOSPI_SR_TCF));
        OCTOSPI1->FCR = OCTOSPI_FCR_CTCF;
        OCTOSPI1->CR  = (OCTOSPI1->CR & ~OCTOSPI_CR_FMODE_Msk);
    } while (status & 0x01U);
}

static void Flash_WriteEnable(void)
{
    while (OCTOSPI1->SR & OCTOSPI_SR_BUSY);
    OCTOSPI1->DLR = 0U;
    OCTOSPI1->CCR = (1U << OCTOSPI_CCR_IMODE_Pos);
    OCTOSPI1->CR  = (OCTOSPI1->CR & ~OCTOSPI_CR_FMODE_Msk);
    OCTOSPI1->IR  = 0x06U;

    while (!(OCTOSPI1->SR & OCTOSPI_SR_TCF));
    OCTOSPI1->FCR = OCTOSPI_FCR_CTCF;
}

static void OCTOSPI1_SendCmd(uint8_t cmd)
{
    while (OCTOSPI1->SR & OCTOSPI_SR_BUSY);
    OCTOSPI1->DLR = 0U;
    OCTOSPI1->CCR = (1U << OCTOSPI_CCR_IMODE_Pos);
    OCTOSPI1->CR  = (OCTOSPI1->CR & ~OCTOSPI_CR_FMODE_Msk);
    OCTOSPI1->IR  = cmd;
    while (!(OCTOSPI1->SR & OCTOSPI_SR_TCF));
    OCTOSPI1->FCR = OCTOSPI_FCR_CTCF;
}

/* =========================================================================
 * MANDATORY EXTERNAL LOADER API FUNCTIONS
 * ========================================================================= */

/**
  * @brief  Initialize OctoSPI, GPIOs, and Flash memory
  * @retval 1 = SUCCESS, 0 = FAIL
  */
__attribute__((used))
int Init(void)
{
    __disable_irq();

    OCTOSPI1_GPIO_Config();

    /* Enable OCTOSPI1 clock on AHB4 */
    RCC->AHB4ENR |= RCC_AHB4ENR_OCTOSPI1EN;
    __DSB();

    OCTOSPI1_SetIndirect();

    /* Send EQIO (0x35) to enter Quad mode on Macronix */
    OCTOSPI1_SendCmd(0x35);
    for (volatile uint32_t i = 0; i < 2000; i++);

    /* Configure into Memory-Mapped mode by default */
    OCTOSPI1_SetMemoryMapped();

    return LOADER_OK;
}

/**
  * @brief  Write a data buffer to external flash
  * @param  Address: Start address (0x90000000 + offset or raw offset)
  * @param  Size   : Number of bytes to write
  * @param  buffer : Pointer to data
  * @retval 1 = SUCCESS, 0 = FAIL
  */
__attribute__((used))
int Write(uint32_t Address, uint32_t Size, uint8_t* buffer)
{
    if (Size == 0 || buffer == 0) return LOADER_OK;

    uint32_t flash_addr = (Address >= 0x90000000UL) ? (Address - 0x90000000UL) : Address;

    OCTOSPI1_SetIndirect();

    uint32_t written = 0;
    while (written < Size)
    {
        uint32_t cur_flash   = flash_addr + written;
        uint16_t page_offset = cur_flash & 0xFF;
        uint16_t space_left  = 256 - page_offset;
        uint16_t chunk       = (Size - written > space_left) ? space_left : (Size - written);

        Flash_WaitBusy();
        Flash_WriteEnable();

        while (OCTOSPI1->SR & OCTOSPI_SR_BUSY);
        OCTOSPI1->DLR = chunk - 1U;
        OCTOSPI1->CCR = (1U << OCTOSPI_CCR_IMODE_Pos)
                      | (1U << OCTOSPI_CCR_ADMODE_Pos)
                      | (2U << OCTOSPI_CCR_ADSIZE_Pos)
                      | (1U << OCTOSPI_CCR_DMODE_Pos);
        OCTOSPI1->CR  = (OCTOSPI1->CR & ~OCTOSPI_CR_FMODE_Msk);
        OCTOSPI1->IR  = 0x02U;
        OCTOSPI1->AR  = cur_flash;

        for (uint16_t i = 0; i < chunk; i++)
        {
            while (!(OCTOSPI1->SR & OCTOSPI_SR_FTF) &&
                   !(OCTOSPI1->SR & OCTOSPI_SR_TCF));
            *(__IO uint8_t *)(&OCTOSPI1->DR) = buffer[written + i];
        }
        while (!(OCTOSPI1->SR & OCTOSPI_SR_TCF));
        OCTOSPI1->FCR = OCTOSPI_FCR_CTCF;
        Flash_WaitBusy();

        written += chunk;
    }

    OCTOSPI1_SetMemoryMapped();
    return LOADER_OK;
}

/**
  * @brief  Erase sectors covering the range
  * @param  EraseStartAddress: Start address
  * @param  EraseEndAddress  : End address
  * @retval 1 = SUCCESS, 0 = FAIL
  */
__attribute__((used))
int SectorErase(uint32_t EraseStartAddress, uint32_t EraseEndAddress)
{
    uint32_t start = (EraseStartAddress >= 0x90000000UL) ? (EraseStartAddress - 0x90000000UL) : EraseStartAddress;
    uint32_t end   = (EraseEndAddress   >= 0x90000000UL) ? (EraseEndAddress   - 0x90000000UL) : EraseEndAddress;

    start &= ~0xFFFUL;

    OCTOSPI1_SetIndirect();

    for (uint32_t addr = start; addr <= end; addr += 4096)
    {
        Flash_WaitBusy();
        Flash_WriteEnable();

        while (OCTOSPI1->SR & OCTOSPI_SR_BUSY);
        OCTOSPI1->DLR = 0U;
        OCTOSPI1->CCR = (1U << OCTOSPI_CCR_IMODE_Pos)
                      | (1U << OCTOSPI_CCR_ADMODE_Pos)
                      | (2U << OCTOSPI_CCR_ADSIZE_Pos);
        OCTOSPI1->CR  = (OCTOSPI1->CR & ~OCTOSPI_CR_FMODE_Msk);
        OCTOSPI1->IR  = 0x20U;
        OCTOSPI1->AR  = addr;

        while (!(OCTOSPI1->SR & OCTOSPI_SR_TCF));
        OCTOSPI1->FCR = OCTOSPI_FCR_CTCF;
        Flash_WaitBusy();

        if (addr + 4096 < addr) break;
    }

    OCTOSPI1_SetMemoryMapped();
    return LOADER_OK;
}

/**
  * @brief  Mass/Chip erase
  * @retval 1 = SUCCESS, 0 = FAIL
  */
__attribute__((used))
int MassErase(void)
{
    OCTOSPI1_SetIndirect();

    Flash_WaitBusy();
    Flash_WriteEnable();

    while (OCTOSPI1->SR & OCTOSPI_SR_BUSY);
    OCTOSPI1->DLR = 0U;
    OCTOSPI1->CCR = (1U << OCTOSPI_CCR_IMODE_Pos);
    OCTOSPI1->CR  = (OCTOSPI1->CR & ~OCTOSPI_CR_FMODE_Msk);
    OCTOSPI1->IR  = 0xC7U;

    while (!(OCTOSPI1->SR & OCTOSPI_SR_TCF));
    OCTOSPI1->FCR = OCTOSPI_FCR_CTCF;
    Flash_WaitBusy();

    OCTOSPI1_SetMemoryMapped();
    return LOADER_OK;
}

/**
  * @brief  Read data from external flash
  * @param  Address: Start address
  * @param  Size   : Number of bytes to read
  * @param  buffer : Destination buffer
  * @retval 1 = SUCCESS, 0 = FAIL
  */
__attribute__((used))
int Read(uint32_t Address, uint32_t Size, uint8_t* buffer)
{
    if (Size == 0 || buffer == 0) return LOADER_OK;

    uint32_t flash_addr = (Address >= 0x90000000UL) ? Address : (0x90000000UL + Address);

    if (!(((OCTOSPI1->CR & OCTOSPI_CR_FMODE_Msk) == (3U << OCTOSPI_CR_FMODE_Pos)) &&
          (OCTOSPI1->CR & OCTOSPI_CR_EN)))
    {
        OCTOSPI1_SetMemoryMapped();
    }

    const uint8_t *src = (const uint8_t *)flash_addr;
    for (uint32_t i = 0; i < Size; i++)
    {
        buffer[i] = src[i];
    }

    return LOADER_OK;
}

/**
  * @brief  Compute 32-bit checksum
  * @param  StartAddress: Start address
  * @param  Size        : Size in bytes
  * @param  InitVal     : Initial checksum value
  * @retval Computed checksum
  */
__attribute__((used))
uint32_t CheckSum(uint32_t StartAddress, uint32_t Size, uint32_t InitVal)
{
    uint32_t flash_addr = (StartAddress >= 0x90000000UL) ? StartAddress : (0x90000000UL + StartAddress);

    if (!(((OCTOSPI1->CR & OCTOSPI_CR_FMODE_Msk) == (3U << OCTOSPI_CR_FMODE_Pos)) &&
          (OCTOSPI1->CR & OCTOSPI_CR_EN)))
    {
        OCTOSPI1_SetMemoryMapped();
    }

    const uint8_t *src = (const uint8_t *)flash_addr;
    for (uint32_t i = 0; i < Size; i++)
    {
        InitVal += src[i];
    }
    return InitVal;
}

/**
  * @brief  Verify flash content against RAM buffer
  * @param  MemoryAddr    : Flash address
  * @param  RAMAddress    : RAM address of reference data
  * @param  Size          : Size in bytes
  * @param  missalignement: Address misalignment (unused)
  * @retval 1 = MATCH, (MemoryAddr + mismatch_offset) = MISMATCH
  */
__attribute__((used))
int Verify(uint32_t MemoryAddr, uint32_t RAMAddress, uint32_t Size, uint32_t missalignement)
{
    uint32_t flash_addr = (MemoryAddr >= 0x90000000UL) ? MemoryAddr : (0x90000000UL + MemoryAddr);

    if (!(((OCTOSPI1->CR & OCTOSPI_CR_FMODE_Msk) == (3U << OCTOSPI_CR_FMODE_Pos)) &&
          (OCTOSPI1->CR & OCTOSPI_CR_EN)))
    {
        OCTOSPI1_SetMemoryMapped();
    }

    const uint8_t *src = (const uint8_t *)flash_addr;
    const uint8_t *ram = (const uint8_t *)RAMAddress;

    for (uint32_t i = 0; i < Size; i++)
    {
        if (src[i] != ram[i])
        {
            return (int)(MemoryAddr + i);
        }
    }
    return LOADER_OK;
}
