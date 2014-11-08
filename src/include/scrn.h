#ifndef __SCRN_H
#define __SCRN_H

void cls();
void putch(char c);
void putint(int val, int b);
void puts(unsigned char *str);
void printf(const char *format, ...);
void settextcolor(unsigned char forecolor, unsigned char backcolor);
void init_video();

#endif
