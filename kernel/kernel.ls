/* os.ls */
OUTPUT_FORMAT("binary");

SECTIONS {
    .head 0x0 : {
        LONG(64 * 1024)         /* 0 : size(stack+.data+heap) */
        LONG(0x69726148)        /* 4 : "Hari" */
        LONG(0)                 /* 8 : mmarea*/
        LONG(0x310000)          /* 12 : stack$B=i4|CM(B & .data$BE>Aw@h(B */
        LONG(SIZEOF(.data))     /* 16 : size of .data */
        LONG(LOADADDR(.data))   /* 20 : size of .data */
        LONG(0xE9000000)        /* 24 : E9000000 */
        LONG(HariMain - 0x20)   /* 28 : entry - 0x20 */
        LONG(0x01)              /* 32 : heap$BNN0h3+;O%"%I%l%9(B */
    }

    .text    : {*(.text)}

    .data 0x310000 : AT ( ADDR(.text) + SIZEOF(.text) ) {
        *(.data)
        *(.rodata*)
        *(.bss)
    }

    /DISCARD/ : { *(.eh_frame) }
}

