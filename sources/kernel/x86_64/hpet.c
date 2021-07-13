#include "kernel/x86_64/hpet.h"
#include "kernel/mmio.h"
#include "kernel/x86_64/memory/mmap.h"

static uintptr_t hpet_base = 0;
static uint64_t hpet_clock = 0;

void hpet_initialize(Handover *handover)
{
    struct acpi_hpet *hpet_table = acpi_find_hpet(handover->rsdp);
    hpet_base = mmap_phys_to_io(hpet_table->address);

    hpet_clock = mmio_read64(hpet_base + HPET_GENERAL_CAPABILITIES) >> 32;

    mmio_write64(hpet_base + HPET_GENERAL_CONFIGUATION, 0);
    mmio_write64(hpet_base + HPET_MAIN_COUNTER_VALUE, 0);
    mmio_write64(hpet_base + HPET_GENERAL_CONFIGUATION, 1);
}

void hpet_sleep(int ms)
{
    uint64_t target = mmio_read64(hpet_base + HPET_MAIN_COUNTER_VALUE) + (ms * 1000000000000) / hpet_clock;
    WAIT_FOR(mmio_read64(hpet_base + HPET_MAIN_COUNTER_VALUE) >= target);
}