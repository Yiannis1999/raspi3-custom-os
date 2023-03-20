#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uart.h"
#include "sd.h"
#include "fat.h"

// get the end of bss segment from linker
extern unsigned char __end;

void main()
{
    uart_init();
    
    if (sd_init() == SD_OK)
    {
        if (fat_getpartition())
        {
            char name[11] = "COUNTER";
            unsigned int cluster = fat_getcluster(name);
            if (cluster)
            {
                char *buffer = fat_readfile(cluster);
                uart_dump(buffer);
                int counter = atoi(buffer) + 1;
                sprintf(buffer, "%d", counter);
                fat_writefile(cluster, buffer);
                fat_resize(name, strlen(buffer));
                buffer = fat_readfile(cluster);
                uart_dump(buffer);
            }
        }
    }

    while (1)
    {
        uart_putc(uart_getc());
    }
}
