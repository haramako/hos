SECTIONS
{
. = 0x0000100000000000;
. += SIZEOF_HEADERS;
.text : {
  *(.text)
}
. += 0x1000;
.data : {
  *(.data)
}
.got : {
  *(.got)
}
}
