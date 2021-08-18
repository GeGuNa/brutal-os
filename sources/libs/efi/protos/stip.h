#pragma once

#include <efi/types.h>

#define CHAR_CARRIAGE_RETURN 0x000D

#define SCAN_NULL 0x0000
#define SCAN_UP 0x0001
#define SCAN_DOWN 0x0002
#define SCAN_RIGHT 0x0003
#define SCAN_LEFT 0x0004
#define SCAN_HOME 0x0005
#define SCAN_END 0x0006
#define SCAN_INSERT 0x0007
#define SCAN_DELETE 0x0008
#define SCAN_PAGE_UP 0x0009
#define SCAN_PAGE_DOWN 0x000A
#define SCAN_F1 0x000B
#define SCAN_F2 0x000C
#define SCAN_F3 0x000D
#define SCAN_F4 0x000E
#define SCAN_F5 0x000F
#define SCAN_F6 0x0010
#define SCAN_F7 0x0011
#define SCAN_F8 0x0012
#define SCAN_F9 0x0013
#define SCAN_F10 0x0014
#define SCAN_F11 0x0015
#define SCAN_F12 0x0016
#define SCAN_ESC 0x0017

#define EFI_SIMPLE_TEXT_INPUT_PROTOCOL_GUID                                            \
    {                                                                                  \
        0x387477c1, 0x69c7, 0x11d2, { 0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b } \
    }

struct _EFI_SIMPLE_TEXT_INPUT;

#define DEF_TEXTI_EFI_FUNC(name, ...) typedef EFIStatus (*EFI_TEXTI_##name)(struct _EFI_SIMPLE_TEXT_INPUT * self __VA_OPT__(, ) __VA_ARGS__)

typedef struct
{
    uint16_t scan_code;
    uint16_t unicode_char;
} EFIInputKey;

DEF_TEXTI_EFI_FUNC(RESET, bool);
DEF_TEXTI_EFI_FUNC(READ_KEY, EFIInputKey *key);

typedef struct _EFI_SIMPLE_TEXT_INPUT
{
    EFI_TEXTI_RESET reset;
    EFI_TEXTI_READ_KEY read_key;
    EFIEvent wait_for_key;
} EFISimpleTextInput;