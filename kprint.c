#include "kprint.h"
#include <stdarg.h>
#include <stdint.h>

void uart_write(uint8_t c)
{
    *((volatile uint8_t *)0x10000000) = c;
}

uint8_t uart_read()
{
    if ((*((volatile uint8_t *)0x10000005) & 1) == 0) return 0;
    return *((volatile uint8_t *)0x10000000);
}

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

void kprintf(char *format, ...)
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
                case '%': uart_write('%'); break;
                case 'x': kprint_hex(va_arg(v, uint64_t)); break;
                case 'd': kprint_dec(va_arg(v, uint64_t)); break;
                case 's': kprint_str(va_arg(v, char *)); break;
            }
        }
        else
        {
            uart_write(*format);
        }
    } while (*++format);

    va_end(v);
}
