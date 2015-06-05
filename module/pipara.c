#include <linux/module.h>
#include <linux/init.h>

MODULE_AUTHOR("Tettoon");
MODULE_DESCRIPTION("PiPara Module");
MODULE_LICENSE("Apache License 2.0");

int pipara_init(void);
void pipara_exit(void);
int pipara_write(int8_t);

int pipara_init(void)
{
    printk(KERN_ALERT "driver loaded\n");
    return 0;
}

void pipara_exit(void)
{
    printk(KERN_ALERT "driver unloaded\n");
}

int pipara_write(int8_t d)
{
    return 0;
}

module_init(pipara_init);
module_exit(pipara_exit);

