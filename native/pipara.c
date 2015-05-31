#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <regex.h>

#include "pipara.h"

typedef struct {
    int gpio_base;
    int pcb_revision;
    int *pins;
} pipara_gpio_info_t;

static pipara_gpio_info_t gpio_info;

int rev1_pins[] = { 4, 17, 18, 21, 22, 23, 24, 25 };
int rev2_pins[] = { 4, 17, 18, 27, 22, 23, 24, 25 };
int *pins;

// I/O access
volatile unsigned *gpio;

#define BCM2708_PERI_BASE	0x20000000
#define BCM2709_PERI_BASE	0x3f000000
#define BCM2708_GPIO_BASE	(BCM2708_PERI_BASE + 0x200000)
#define BCM2709_GPIO_BASE	(BCM2709_PERI_BASE + 0x200000)

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

#define GET_GPIO_MODE(g) (*(gpio+((g)/10))>>(((g)%10)*3))&7

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0

#define GET_GPIO(g) (*(gpio+13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH

#define GPIO_PULL *(gpio+37) // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio+38) // Pull up/pull down clock

static int get_gpio_info(void);

/**
 *
 */
int pipara_setup(void)
{
    get_gpio_info();
    // printf("0x%08X, %d\n", gpio_info.gpio_base, gpio_info.pcb_revision);

    int mem_fd;
    if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0)
    {
        return -1;
    }

    void *gpio_map;
    gpio_map = mmap(
        NULL,
        BLOCK_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        mem_fd,
        gpio_info.gpio_base
    );

    close(mem_fd);

    if (gpio_map == MAP_FAILED)
    {
        return -2;
    }

    gpio = (volatile unsigned *)gpio_map;

    if (gpio_info.pcb_revision == 1)
    {
        pins = rev1_pins;
    }
    else 
    {
        pins = rev2_pins;
    }

    for (int i = 0; i < 8; i++)
    {
        int gpio_pin = pins[i];
        int gpio_mode = GET_GPIO_MODE(gpio_pin);
        //printf("%d: GPIO%d ... %d\n", i, gpio_pin, gpio_mode);
        if (gpio_mode != 1)
        {
            INP_GPIO(gpio_pin);
            OUT_GPIO(gpio_pin);
        }
    }

    return 0;
}

/**
 *
 */
int pipara_write(int8_t d)
{
    int gpio_set = 0;
    int gpio_clr = 0;

    for (int i = 0; i < 8; i++)
    {
        if ((d >> i) & 1)
        {
            gpio_set |= 1 << pins[i];
        }
        else
        {
            gpio_clr |= 1 << pins[i];
        }
    }
    GPIO_SET = gpio_set;
    GPIO_CLR = gpio_clr;

    return 0;
}

static int get_gpio_info(void)
{
    FILE *fp;
    fp = fopen("/proc/cpuinfo", "r");
    if (fp == NULL)
        return 1;

    regex_t reg;
    if (regcomp(&reg, "^([^\t]+)[ \t]*: (.*)$", REG_EXTENDED | REG_NEWLINE) != 0)
        return 2;

    char line[256];
    char cpuinfo_hardware[20];
    char cpuinfo_revision[20];
    while (fgets(line, 256, fp) != NULL)
    {
        char key[32];
        char val[1024];
        regmatch_t match[3];
        int size = sizeof(match) / sizeof(regmatch_t);
        if (regexec(&reg, line, size, match, 0) != REG_NOMATCH)
        {
            int key_so = match[1].rm_so;
            int key_eo = match[1].rm_eo;
            size_t key_sz = key_eo - key_so;
            int val_so = match[2].rm_so;
            int val_eo = match[2].rm_eo;
            size_t val_sz = val_eo - val_so;
            if (key_so != -1 && key_eo != -1 && val_so != -1 && val_eo != -1)
            {
                strncpy(key, line + key_so, key_sz);
                key[key_sz] = '\0';
                if (strcmp("Hardware", key) == 0)
                {
                    strncpy(cpuinfo_hardware, line + val_so, val_sz);
                    cpuinfo_hardware[val_sz] = '\0';
                }
                if (strcmp("Revision", key) == 0)
                {
                    strncpy(cpuinfo_revision, line + val_so, val_sz);
                    cpuinfo_revision[val_sz] = '\0';
                }
            }
        }
    }

    //printf("%s,%s\n",cpuinfo_hardware, cpuinfo_revision);

    if (strcmp("BCM2709", cpuinfo_hardware) == 0)  /* Raspberry Pi 2 */
    {
        gpio_info.gpio_base = BCM2709_GPIO_BASE;
        gpio_info.pcb_revision = 2;
    }
    else if (strcmp("BCM2708", cpuinfo_hardware) == 0)  /* Raspberry Pi */
    {
        gpio_info.gpio_base = BCM2708_GPIO_BASE;
        if (strcmp("0002", cpuinfo_revision) == 0 || strcmp("0003", cpuinfo_revision) == 0)
        {
            gpio_info.pcb_revision = 1;
        }
        else
        {
            gpio_info.pcb_revision = 2;
        }
    }

    regfree(&reg);
    fclose(fp);
}

