#include "newlib_func.h"

int strncmp(const char* s1, const char* s2, size_t n)
{
  for (size_t i = 0; i < n; i++) {
    if (s1[i] - s2[i])
      return s1[i] - s2[i];
  }
  return 0;
}

int atoi(const char* str)
{
  int v = 0;
  char c;
  while ((c = *(str++))) {
    if (c == ' ')
      continue;
    if ('0' <= c && c <= '9') {
      v *= 10;
      v += c - '0';
      continue;
    }
    break;
  }
  return v;
}
