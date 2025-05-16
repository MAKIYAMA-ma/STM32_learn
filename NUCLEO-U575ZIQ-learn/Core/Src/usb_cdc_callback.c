#include "tusb.h"
#include "main.h"

void tud_cdc_rx_cb(uint8_t itf)
{
    (void) itf;
    uint8_t buf[64];
    uint32_t count = tud_cdc_read(buf, sizeof(buf));

    // Echoback
    tud_cdc_write(buf, count);
    tud_cdc_write_flush();

    log_printf(DBG_LVL_DBG, "%s", buf);
}

void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
    (void) itf;
    (void) rts;

    if (dtr) {
        // CDC Opened
        log_printf(DBG_LVL_DBG, "CDC Opened!\n");
    } else {
        // CDC Closed
        log_printf(DBG_LVL_DBG, "CDC Closed!\n");
    }
}
