SECTIONS
{
  . = 0x0000100000000000 + SIZEOF_HEADERS;
  .text ALIGN(0x1000): {
    *(.text);
  }
  .data ALIGN(0x1000): {
    *(.data);
  }
  .got ALIGN(0x1000): {
    *(.got);
  }
}
