/* Kept in sync with elekloader.sdk.build.LD_SCRIPT. */
SECTIONS
{
    .boot 0 : { *(.boot) }
    .run  0 : { *(.run) *(.text) *(.text.*) *(.data) *(.data.*) *(.rodata) *(.rodata.*) *(.sdata) *(.sdata.*) }
    .fast 0 : { *(.fast) }
    .bss  0 : { *(.bss) *(.bss.*) *(.sbss) *(.sbss.*) *(COMMON) }
    /DISCARD/ : { *(.comment) *(.note*) *(.gnu.attributes) }
}

