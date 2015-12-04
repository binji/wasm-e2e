#include "test/wasm.h"

#include <stdio.h>

extern "C" void print(int value) {
  printf("%d\n", value);
}

static void writeByte(unsigned char value) {
  printf("%3d", value);
}

extern "C" void flipBuffer(const void* vp, int w, int h) {
  const unsigned char* p = (const unsigned char*)vp;
  printf("P3\n");
  printf("%d %d\n", w, h);
  printf("255\n");
  printf("# This is a PPM file, redirect stdout to view.\n");
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      if (i != 0) printf("  ");
      writeByte(*p++);
      printf(" ");
      writeByte(*p++);
      printf(" ");
      writeByte(*p++);
      p++;
    }
    printf("\n");
    printf("\n");
  }
  printf("# This is a PPM file, redirect stdout to view.\n");
}
