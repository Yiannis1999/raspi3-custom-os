void uart_init();
void uart_putc(char c);
char uart_getc();
void uart_puts(char *s);
void uart_puthex2(char c);
void uart_puthex8(unsigned int u);
void uart_dump(unsigned char *p);
