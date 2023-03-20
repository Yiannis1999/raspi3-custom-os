#include "gpio.h"
#include "mbox.h"
#include "delays.h"

/* PL011 UART registers */
#define UART0_DR ((volatile unsigned int *)(MMIO_BASE + 0x00201000))
#define UART0_FR ((volatile unsigned int *)(MMIO_BASE + 0x00201018))
#define UART0_IBRD ((volatile unsigned int *)(MMIO_BASE + 0x00201024))
#define UART0_FBRD ((volatile unsigned int *)(MMIO_BASE + 0x00201028))
#define UART0_LCRH ((volatile unsigned int *)(MMIO_BASE + 0x0020102C))
#define UART0_CR ((volatile unsigned int *)(MMIO_BASE + 0x00201030))
#define UART0_IMSC ((volatile unsigned int *)(MMIO_BASE + 0x00201038))
#define UART0_ICR ((volatile unsigned int *)(MMIO_BASE + 0x00201044))

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart_init()
{
    register unsigned int r;

    /* initialize UART */
    *UART0_CR = 0; // turn off UART0

    /* set up clock for consistent divisor values */
    mbox[0] = 9 * 4;
    mbox[1] = MBOX_REQUEST;
    mbox[2] = MBOX_TAG_SETCLKRATE; // set clock rate
    mbox[3] = 12;
    mbox[4] = 8;
    mbox[5] = 2;       // UART clock
    mbox[6] = 4000000; // 4Mhz
    mbox[7] = 0;       // clear turbo
    mbox[8] = MBOX_TAG_LAST;
    mbox_call(MBOX_CH_PROP);

    /* map UART0 to GPIO pins */
    r = *GPFSEL1;
    r &= ~((7 << 12) | (7 << 15)); // gpio14, gpio15
    r |= (4 << 12) | (4 << 15);    // alt0
    *GPFSEL1 = r;
    *GPPUD = 0; // enable pins 14 and 15
    wait_cycles(150);
    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    wait_cycles(150);
    *GPPUDCLK0 = 0; // flush GPIO setup

    *UART0_ICR = 0x7FF; // clear interrupts
    *UART0_IBRD = 2;    // 115200 baud
    *UART0_FBRD = 0xB;
    *UART0_LCRH = 0b11 << 5; // 8n1
    *UART0_CR = 0x301;       // enable Tx, Rx, FIFO
}

/**
 * Send a character
 */
void uart_putc(char c)
{
    /* wait until we can send */
    while (*UART0_FR & 0x20)
        asm volatile("nop");
    /* write the character to the buffer */
    *UART0_DR = c;
}

/**
 * Receive a character
 */
char uart_getc()
{
    char r;
    /* wait until something is in the buffer */
    while (*UART0_FR & 0x10)
        asm volatile("nop");
    /* read it and return */
    r = (char)(*UART0_DR);
    /* convert carrige return to newline */
    return r == '\r' ? '\n' : r;
}

/**
 * Send a string
 */
void uart_puts(char *s)
{
    while (*s)
    {
        /* convert newline to carrige return + newline */
        if (*s == '\n')
            uart_putc('\r');
        uart_putc(*s++);
    }
}

/**
 * Send a character in hexadecimal
 */
void uart_puthex2(char c)
{
    const char hex[] = "0123456789ABCDEF";
    // Convert the high nibble to a hexadecimal character
    uart_putc(hex[(c >> 4) & 0xF]);
    // Convert the low nibble to a hexadecimal character
    uart_putc(hex[c & 0xF]);
}

/**
 * Send an integer in hexadecimal
 */
void uart_puthex8(unsigned int u)
{
    const char hex[] = "0123456789ABCDEF";
    // Iterate through each nibble from left to right
    for (int i = 7; i >= 0; i--)
        uart_putc(hex[(u >> (4 * i)) & 0xF]);
}

/**
 * Dump memory
 */
void uart_dump(unsigned char *p)
{
    // Loop over 512 bytes of memory, printing 16 bytes at a time
    for (int i = 0; i < 512; i += 16)
    {
        // Print the starting address of the current line of memory in hexadecimal
        uart_puthex8((unsigned long)p);
        uart_puts(": ");
        // Print the hexadecimal representation of 16 bytes of memory, 2 characters per byte
        for (int j = 0; j < 16; j++)
        {
            unsigned char c = p[i + j];
            uart_puthex2(c);
            uart_putc(' ');
            // Print an extra space after every fourth byte
            if (j % 4 == 3)
                uart_putc(' ');
        }
        // Print the ASCII representation of 16 bytes of memory, replacing non-printable characters with a period
        for (int j = 0; j < 16; j++)
        {
            unsigned char c = p[i + j];
            uart_putc(c < 32 || c >= 127 ? '.' : c);
        }
        uart_putc('\r');
        uart_putc('\n');
        // Move the pointer to the next line of memory
        p += 16;
    }
}
