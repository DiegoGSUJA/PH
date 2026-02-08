#include<linux/module.h>
#include<linux/kernel.h>

static int __init init_driver(void) {
    printk(KERN_INFO "Skeleton driver loaded\n");
    return 0;
}

static void __exit exit_driver(void) {
    printk(KERN_INFO "Skeleton driver unloaded\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francisco Charte");
MODULE_VERSION("0.1");
MODULE_DESCRIPTION("Sample skeleton of a device driver module");

module_init(init_driver)
module_exit(exit_driver)
