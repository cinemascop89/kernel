#include <stdio.h>

struct mem_tag {
  void* next;
  int size;
};

typedef struct bit_test {
  unsigned int a : 1;
  unsigned int b : 2;
} bit_test_t;

long pow(long n, int exponent) {
  long ret = n;
  if (exponent == 0) return 1;
  while(--exponent > 0) ret *= n;
  return ret;
}

void putint(int val) {
  static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
  int b = 10, tot_digits = 0, orig_val = val, curr_digit;

  if (val < 0) {
    val = -val;
  }
  do {
    tot_digits++;
  } while ((val /= b) > 0);
  curr_digit = tot_digits;
  do {
    printf("%d, %d\n",curr_digit, orig_val/pow(b, curr_digit - 1) - orig_val/pow(b, curr_digit)*b);
  } while (--curr_digit);

}

int main() {

  printf("size: %d\n", sizeof(struct bit_test));
  bit_test_t test;
  test.a = 1;
  test.b = 2;
  printf("content: 0x%x\n", test);
  return 0;
}
