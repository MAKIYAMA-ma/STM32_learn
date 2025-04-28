#include "main.h"
#include "setting_flash.h"
#include <string.h>
#include <stdio.h>

extern const uint32_t _user_settings_start;
#define SETTINGS_ADDR_START ((uint32_t)&_user_settings_start)
#define SETTINGS_SIZE       (256)

#define QUADWORD_SIZE       16           // 128ビット = 16バイト

static uint32_t CalculateChecksum(UserSetting_t *setting);

UserSetting_t g_user_setting;

const static UserSetting_t user_settig_def = {
    0x0123, // uint32_t version;
    "sam board", // uint8_t  node_name[16];
    DBG_LVL_DBG, // uint8_t  dbg_lvl;
    0, // uint8_t  red_led_int;
    0x00, // uint32_t checksum;
};

static uint32_t CalculateChecksum(UserSetting_t *setting)
{
    uint32_t sum = 0;
    uint8_t *p = (uint8_t*)setting;
    for (size_t i = 0; i < sizeof(UserSetting_t) - sizeof(uint32_t); i++) {
        sum += p[i];
    }
    return sum;
}

uint8_t InitUserSetting(void)
{
    if(LoadUserSetting(&g_user_setting) == HAL_ERROR) {
        memcpy(&g_user_setting, &user_settig_def, sizeof(UserSetting_t));
        g_user_setting.checksum = CalculateChecksum(&g_user_setting);
        return -1;
    } else {
        return 0;
    }
}

HAL_StatusTypeDef SaveUserSetting(UserSetting_t *setting)
{
    HAL_FLASH_Unlock();

    // erase
    FLASH_EraseInitTypeDef erase = {
        .TypeErase   = FLASH_TYPEERASE_PAGES,
        .Page        = (SETTINGS_ADDR_START - FLASH_BASE) / FLASH_PAGE_SIZE,
        .NbPages     = 1,
        .Banks       = FLASH_BANK_2
    };
    uint32_t pageError = 0;
    if (HAL_FLASHEx_Erase(&erase, &pageError) != HAL_OK) {
        HAL_FLASH_Lock();
        return HAL_ERROR;
    }

    // calculate checksum
    setting->checksum = CalculateChecksum(setting);

    // write
    uint8_t *data = (uint8_t *)setting;
    size_t len = (size_t)sizeof(UserSetting_t);
    for (size_t i = 0; i < len; i += QUADWORD_SIZE) {
        uint8_t val[16];
        memset(val, 0, sizeof(val));
        memcpy(val, &data[i], (len - i >= QUADWORD_SIZE) ? QUADWORD_SIZE: len - i);
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, SETTINGS_ADDR_START + i, (uint32_t)val) != HAL_OK) {
            HAL_FLASH_Lock();
            return HAL_ERROR;
        }
    }

    HAL_FLASH_Lock();
    return HAL_OK;
}

HAL_StatusTypeDef LoadUserSetting(UserSetting_t *setting)
{
    memcpy(setting, (void*)SETTINGS_ADDR_START, sizeof(UserSetting_t));
    uint32_t calc = CalculateChecksum(setting);
    return (calc == setting->checksum) ? HAL_OK : HAL_ERROR;
}
