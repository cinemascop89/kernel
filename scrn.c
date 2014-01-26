#include <system.h>
#include <str.h>
#include <stdarg.h>
#include <math.h>

unsigned short *textmemptr; // text pointer
int attrib = 0x0f; // background and foreground colors
int csr_x = 0, csr_y = 0; // x and y cursor coordinates

void scroll(void) {
  unsigned blank, temp;

  blank = 0x20 | (attrib << 8); // set space char and give it backcolor

  if (csr_y >= 25) {
    temp = csr_y - 25 + 1;
    memcpy(textmemptr, textmemptr + temp * 80, (25 - temp) * 80 * 2);

    memsetw(textmemptr + (25 - temp) * 80, blank, 80);
    csr_y = 25 - 1;
  }
}

void move_csr(void) {
  unsigned temp;

  temp = csr_y * 80 + csr_x;

  outportb(0x3d4, 14);
  outportb(0x3d5, temp >> 8);
  outportb(0x3d4, 15);
  outportb(0x3d5, temp);
}

void cls() {
  unsigned blank;
  int i;

  blank = 0x20 | (attrib << 8);

  for(i = 0; i < 25; i++)
    memsetw(textmemptr + i * 80, blank, 80);

  csr_x = 0;
  csr_y = 0;
  move_csr();
}

void putch(unsigned char c) {
  unsigned short *where;
  unsigned att = attrib << 8;

  if (c == 0x08) {
    if (csr_x != 0) csr_x--;
    textmemptr[csr_y * 80 + csr_x] = ' ';
  } else if (c == 0x09) {
    csr_x = (csr_x + 8) & ~(8 - 1);
  } else if (c == '\r') {
    csr_x = 0;
  } else if (c == '\n') {
    csr_x = 0;
    csr_y++;
  } else if (c >= ' ') {
    where = textmemptr + (csr_y * 80 + csr_x);
    *where = c | att;
    csr_x++;
  }

  if (csr_x >= 80) {
    csr_x = 0;
    csr_y++;
  }

  scroll();
  move_csr();
}

void puts(unsigned char *text) {
  int i;
  for(i = 0; i < strlen(text); i++) {
    putch(text[i]);
  }
}

void putint(int val, int b) {
  static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
  char result[30];
  int i=0, j;

  if (val < 0) {
    val = -val;
  }
  int n1 = val;
  if (n1 == 0) {
    putch('0');
    return;
  }
  while(n1 != 0) {
    result[i++] = digits[n1 % b];
    n1 /= b;
  }
  for (j=i-1; j>=0; j--) {
    putch(result[j]);
  }
}

void putfloat(float val) {

}

void printf(const char *format, ...) {
  int i = 0, curr_arg = 0, arg_count = 0;
  char c;
  while(format[i] != '\0') {
    if (format[i] == '%' && format[i+1] != '%') {
      arg_count++;
    }
    i++;
  }
  va_list args;
  va_start(args, arg_count);
  i = 0;
  while(format[i] != '\0') {
    c = format[i];
    if (c == '%' && format[i+1] != '%') {
      switch (format[i+1]) {
      case 's':
        puts(va_arg(args, char*));
        break;
      case 'd':
        putint(va_arg(args, int), 10);
        break;
      case 'l':
        putint(va_arg(args, long), 10);
        break;
      case 'x':
        putint(va_arg(args, int), 16);
        break;
      case 'b':
        putint(va_arg(args, int), 2);
        break;
      case 'c':
        putch(va_arg(args, char));
        break;
      case 'f':
        putfloat(va_arg(args, float));
        break;
      case '%':
        putch('%');
      }
      curr_arg++;
      i++;
    } else {
      putch(c);
    }
    i++;
  }
  va_end(args);
}
void settextcolor(unsigned char forecolor, unsigned char backcolor) {
  attrib = (backcolor << 4) | (forecolor & 0x0f);
}

void init_video(void) {
  textmemptr = (unsigned short *)0xb8000;
  cls();
}
