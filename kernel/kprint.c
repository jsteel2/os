#include "kprint.h"
#include "stdint.h"
#include "stdarg.h"
#include "sbi.h"
#include "uart.h"
#include <libfdt.h>

void kprint_char(char c)
{
    uart_write(c);
}

void kprint_hex(uint64_t n)
{
    int remainder = n % 16;

    if (n >= 16) kprint_hex(n / 16);

    if (remainder < 10) kprint_char(remainder + '0');
    else kprint_char(remainder - 10 + 'A');
}

void kprint_dec(uint64_t n)
{
    if (n >= 10) kprint_dec(n / 10);

    kprint_char(n % 10 + '0');
}

void kprint_str(char *s)
{
    do kprint_char(*s); while (*++s);
}

void kprintf(const char *format, ...)
{
    va_list v;
    va_start(v, format);

    do
    {
        if (*format == '%')
        {
            format++;
            switch (*format)
            {
                case '%': kprint_char('%'); break;
                case 'x': kprint_hex(va_arg(v, u64)); break;
                case 'c': kprint_char(va_arg(v, int)); break;
                case 'd': kprint_dec(va_arg(v, u64)); break;
                case 's': kprint_str(va_arg(v, char *)); break;
                default: kprintf("invalid format %%%c", *format); break;
            }
        }
        else
        {
            kprint_char(*format);
        }
    } while (*++format);

    va_end(v);
}
