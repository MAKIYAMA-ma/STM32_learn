#include "setting_flash.h"

#define SETTINGS_ADDR_START ((uint32_t)_user_settings_start)
#define SETTINGS_SIZE       (256)

#define QUADWORD_SIZE       16           // 128ビット = 16バイト

extern uint32_t _user_settings_start;

static HAL_StatusTypeDef Flash_ErasePage(uint32_t address)
{
    HAL_StatusTypeDef status;
    FLASH_EraseInitTypeDef eraseInit;
    uint32_t pageError;

    HAL_FLASH_Unlock();

    eraseInit.TypeErase   = FLASH_TYPEERASE_PAGES;
    eraseInit.Page        = (address - FLASH_BASE) / FLASH_PAGE_SIZE;
    eraseInit.NbPages     = 1;
    eraseInit.Banks       = FLASH_BANK_1;

    status = HAL_FLASHEx_Erase(&eraseInit, &pageError);

    HAL_FLASH_Lock();
    return status;
}

HAL_StatusTypeDef Settings_Write(const uint8_t *data, size_t len)
{
    if (len > SETTINGS_SIZE) return HAL_ERROR;

    HAL_FLASH_Unlock();
    if (Flash_ErasePage(SETTINGS_ADDR_START) != HAL_OK) return HAL_ERROR;

    for (size_t i = 0; i < len; i += QUADWORD_SIZE) {
        uint64_t val = 0;
        memcpy(&val, &data[i], (len - i >= QUADWORD_SIZE) ? QUADWORD_SIZE: len - i);
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, SETTINGS_ADDR_START + i, val) != HAL_OK) {
            HAL_FLASH_Lock();
            return HAL_ERROR;
        }
    }

    HAL_FLASH_Lock();
    return HAL_OK;
}

void Settings_Read(uint8_t *data, size_t len)
{
    if (len > SETTINGS_SIZE) len = SETTINGS_SIZE;
    memcpy(data, (void*)SETTINGS_ADDR_START, len);
}
