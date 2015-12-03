#include "test/wasm.h"

#include <stdio.h>

extern "C" void print(int value) {
  printf("%d\n", value);
}
