MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 0x100000000  /* Define the FLASH region */
}

SECTIONS
{
    /* Place the .text section at the start of the FLASH region */
    .text : {
        *(.text)  /* All text (code) sections from input files */
    } > FLASH
}

