#include "PGDriver.h"

static dev_t pg_dev_node = -1;
static struct cdev pg_cdevs[NUM_DEVICES];
static struct class *pg_class = NULL;

int pg_uevent(const struct device *dev, struct kobj_uevent_env *env) {
    int minor = MINOR(dev->devt);
    add_uevent_var(env, "DEVMODE=%#o", (minor == 0) ? 0600 : 0666);
    return 0;
}

static int __init pg_init(void) {
    int i;
    if (alloc_chrdev_region(&pg_dev_node, 0, NUM_DEVICES, DRIVER_NAME) < 0) return -1;

    pg_class = class_create(DRIVER_CLASS);
    if (IS_ERR(pg_class)) goto error;
    pg_class->dev_uevent = pg_uevent;

    for (i = 0; i < NUM_DEVICES; i++) {
        cdev_init(&pg_cdevs[i], &pg_fops);
        dev_t id = MKDEV(MAJOR(pg_dev_node), i);
        if(cdev_add(&pg_cdevs[i], id, 1) < 0) goto error;
        device_create(pg_class, NULL, id, NULL, "PGDriver%d", i);
    }

    pr_info("PGDriver: Cargado. Nodos listos en /dev/pg_nodeX\n");
    return 0;

error:
    if (pg_class) class_destroy(pg_class);
    if (pg_dev_node != -1) unregister_chrdev_region(pg_dev_node, NUM_DEVICES);
    return -1;
}

static void __exit pg_exit(void) {
    int i;
    for (i = 0; i < NUM_DEVICES; i++) {
        device_destroy(pg_class, MKDEV(MAJOR(pg_dev_node), i));
        cdev_del(&pg_cdevs[i]);
    }
    class_destroy(pg_class);
    unregister_chrdev_region(pg_dev_node, NUM_DEVICES);
    pr_info("PGDriver: Descargado\n");
}

module_init(pg_init);
module_exit(pg_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Diego & Gabriel");
