#include <math.h>

long pow(long n, int exponent) {
  long ret = n;
  if (exponent == 0) return 1;
  while(--exponent > 0) ret *= n;
  return ret;
}
