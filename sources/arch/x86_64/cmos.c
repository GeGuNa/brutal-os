#include "arch/x86_64/cmos.h"
#include "arch/x86_64/asm.h"

static uint8_t from_binary_coded_decimal(uint8_t value)
{
    return (((value) / 16) * 10 + ((value)&0xf));
}

uint8_t cmos_read(enum cmos_regs reg)
{
    asm_out8(CMOS_ADDRESS, reg);
    return asm_in8(CMOS_DATA);
}

bool cmos_is_update(void)
{
    return cmos_read(CMOS_STATUS_A) & 0x80;
}

datetime_t cmos_read_rtc(void)
{
    asm_cli();

    WAIT_FOR(!cmos_is_update());

    datetime_t datetime;

    datetime.second = from_binary_coded_decimal(cmos_read(CMOS_RTC_SECOND));
    datetime.minute = from_binary_coded_decimal(cmos_read(CMOS_RTC_MINUTE));
    datetime.hour = from_binary_coded_decimal(cmos_read(CMOS_RTC_HOUR));
    datetime.day = from_binary_coded_decimal(cmos_read(CMOS_RTC_DAY));
    datetime.month = from_binary_coded_decimal(cmos_read(CMOS_RTC_MONTH));

    // FIXME: Add support for century register
    datetime.year = from_binary_coded_decimal(cmos_read(CMOS_RTC_YEAR)) + 2000;

    asm_sti();

    return datetime;
}