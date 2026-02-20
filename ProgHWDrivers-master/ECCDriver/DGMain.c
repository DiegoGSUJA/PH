#include "DGDriver.h"

static dev_t dg_dev_node = -1;
static struct cdev dg_cdevs[NUM_DEVICES];
static struct class *dg_class = NULL;

int dg_uevent(const struct device *dev, struct kobj_uevent_env *env) {
    int minor = MINOR(dev->devt);
    add_uevent_var(env, "DEVMODE=%#o", (minor == 0) ? 0600 : 0666);
    return 0;
}

static int __init dg_init(void) {
    int i;
    if (alloc_chrdev_region(&dg_dev_node, 0, NUM_DEVICES, DRIVER_NAME) < 0) return -1;

    dg_class = class_create(DRIVER_CLASS);
    if (IS_ERR(dg_class)) goto error;
    dg_class->dev_uevent = dg_uevent;

    for (i = 0; i < NUM_DEVICES; i++) {
        cdev_init(&dg_cdevs[i], &dg_fops);
        dev_t id = MKDEV(MAJOR(dg_dev_node), i);
        if(cdev_add(&dg_cdevs[i], id, 1) < 0) goto error;
        device_create(dg_class, NULL, id, NULL, "DGDriver%d", i);
    }

    pr_info("DGDriver: Cargado. Nodos listos en /dev/dg_nodeX\n");
    return 0;

error:
    if (dg_class) class_destroy(dg_class);
    if (dg_dev_node != -1) unregister_chrdev_region(dg_dev_node, NUM_DEVICES);
    return -1;
}

static void __exit dg_exit(void) {
    int i;
    for (i = 0; i < NUM_DEVICES; i++) {
        device_destroy(dg_class, MKDEV(MAJOR(dg_dev_node), i));
        cdev_del(&dg_cdevs[i]);
    }
    class_destroy(dg_class);
    unregister_chrdev_region(dg_dev_node, NUM_DEVICES);
    pr_info("DGDriver: Descargado\n");
}

module_init(dg_init);
module_exit(dg_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Diego & Gabriel");
