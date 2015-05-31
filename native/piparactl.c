#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pipara.h"

void usage();

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        usage();
        return 1;
    }

    int data;
    int count = sscanf(argv[1], "%X", &data);
    if (count < 1 || count == EOF)
    {
        printf("Invalid argument: %s\n", argv[1]);
        return 2;
    }
    
    printf("Output value: %02X (%d)\n", data, data);

    int rc = pipara_setup();
    if (rc != 0)
    {
        printf("pipara_setup() returned %d\n", rc);
    }

    pipara_write(data);

    return 0;
}

void usage()
{
    puts("Usage: piparactl <value>");
    puts("  value ... A hexadecimal value for 8-bit parallel output.");
    puts("            ex. e5");
}

