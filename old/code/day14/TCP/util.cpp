#include "util.h"

#include <cstdio>
#include <cstdlib>

void ErrorIf(bool condition, const char *errmsg) {
  if (condition) {
    perror(errmsg);
    exit(EXIT_FAILURE);
  }
}