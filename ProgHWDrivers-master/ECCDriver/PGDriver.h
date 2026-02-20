#ifndef PG_DRIVER_H
#define PG_DRIVER_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#define DRIVER_NAME  "PGDriver"
#define DRIVER_CLASS "PGDriverClass"
#define NUM_DEVICES  3   /* Dispositivos: 0 (Normal), 1 (Leet), 2 (Binario) */
#define BUFFER_SIZE  512

/* Estructura para la memoria de cada nodo */
struct pg_device_data {
    char buffer[BUFFER_SIZE];
    size_t length;
};

/* Prototipos exportados */
extern const struct file_operations pg_fops;
int pg_uevent(const struct device *dev, struct kobj_uevent_env *env);

#endif
