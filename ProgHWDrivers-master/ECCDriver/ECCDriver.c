/*
    ECCDriver.c 

    ECC - Esqueleto de controlador completo
    Noviembre 2018-2022 - Francisco Charte
    Adaptado por: Diego Gomez Sanchez
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h> // Para copy_to_user y copy_from_user

#define DRIVER_NAME  "ECCDriver"
#define DRIVER_CLASS "ECCDriverClass"
#define NUM_DEVICES  3  /* Número de dispositivos a crear */
#define BUFFER_SIZE  512 /* Tamaño del búfer según la práctica */

/* Estructura de datos global para memoria persistente independiente */
struct ECC_device_data {
    char buffer[BUFFER_SIZE];
    size_t length;
};

static struct ECC_device_data ECC_devices[NUM_DEVICES];

static dev_t major_minor = -1;
static struct cdev ECCcdev[NUM_DEVICES];
static struct class *ECCclass = NULL;

/* ============ Funciones que implementan las operaciones del controlador ============= */

static int ECCopen(struct inode *inode, struct file *file) {
    int minor = iminor(inode);
    pr_info("ECCDriver: Abriendo dispositivo con numero menor: %d\n", minor);
    return 0;
}

/* Cuestión 4: Implementación de ECCread */
static ssize_t ECCread1(struct file *file, char __user *buffer, size_t count, loff_t *f_pos) {
    int minor = iminor(file_inode(file));
    
    if (*f_pos >= ECC_devices[minor].length) return 0;
    
    if (count > ECC_devices[minor].length - *f_pos)
        count = ECC_devices[minor].length - *f_pos;
        
    if (copy_to_user(buffer, ECC_devices[minor].buffer + *f_pos, count))
        return -EFAULT;
        
    *f_pos += count;
    return count;
}


//PROPUESTA EN LECTURA
static ssize_t ECCread2(struct file *file, char __user *buf, size_t count, loff_t *ppos){

   int minor = iminor(file_inode(file));

   char buffer_trans[BUFFER_SIZE];
   
   //Control posicion
   
   if (*ppos > 0) return 0;
   
   if ( count > BUFFER_SIZE ) count = BUFFER_SIZE;
   
   
   //Procesamos cada caracter y dependiendo de que sea guardamos la cifrada en el otro buffer (contenedor)
   
   for ( int i = 0 ; i < count; i++){
   
       char c = ECC_devices[minor].buffer[i];
       
       if ( c == 'a' || c == 'A') buffer_trans[i] = '4';
       else if ( c == 'e' || c == 'E') buffer_trans[i] = '3';
       else if ( c == 'i' || c == 'I') buffer_trans[i] = '2';
       else if ( c == 'o' || c == 'O') buffer_trans[i] = '1';
       else if ( c == 'u' || c == 'U') buffer_trans[i] = '0';
       
   }
   
   
   if ( copy_to_user(buf,buffer_trans,count)) return -EFAULT;
   
   *ppos += count;
   
   printk(KERN_INFO "ECCDriver_ Lectura procesada enviada (%zu bytes)\\n",count);
   
   return count;

}

static ssize_t ECCread_selector(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    // Obtenemos el Minor del archivo que el usuario ha abierto
    int minor = iminor(file_inode(file));

    // Si el usuario abre el dispositivo 0 (/dev/ecc0), lectura normal
    if (minor == 0) {
        return ECCread1(file, buf, count, ppos);
    } 
    // Si el usuario abre el dispositivo 1 (/dev/ecc1), lectura cifrada
    else if (minor == 1) {
        return ECCread2(file, buf, count, ppos);
    }

    return -EINVAL; // Error si es cualquier otro minor
}


/* Cuestión 4 y 5: Implementación de ECCwrite con memset */
static ssize_t ECCwrite(struct file *file, const char __user *buffer, size_t count, loff_t *f_pos) {
    int minor = iminor(file_inode(file));
    
    /* Propuesta personal: Limpieza total del búfer mediante memset previo a cada escritura */
    memset(ECC_devices[minor].buffer, 0, BUFFER_SIZE);
    
    if (count > BUFFER_SIZE) count = BUFFER_SIZE;
    
    if (copy_from_user(ECC_devices[minor].buffer, buffer, count)) {
        return -EFAULT;
    }
    
    ECC_devices[minor].length = count;
    pr_info("ECCDriver: Guardados %zu bytes en el dispositivo%d\n", count, minor);
    
    return count;
}

static int ECCrelease(struct inode *inode, struct file *file) {
    pr_info("ECCrelease");
    return 0;
}

/* ============ Estructura con las operaciones del controlador ============= */

static const struct file_operations ECC_fops = {
    .owner = THIS_MODULE,
    .open = ECCopen,
    .read = ECCread_selector,
    .write = ECCwrite,
    .release = ECCrelease,
};

/* ============ Inicialización del controlador ============= */

static int ECCdev_uevent(const struct device *dev, struct kobj_uevent_env *env) {
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

static int __init init_driver(void) {
    int n_device;
    dev_t id_device;

    if (alloc_chrdev_region(&major_minor, 0, NUM_DEVICES, DRIVER_NAME) < 0) {
        pr_err("Major number assignment failed");
        goto error;
    }

    if((ECCclass = class_create(DRIVER_CLASS)) == NULL) {
        pr_err("Class device registering failed");
        goto error;
    } else
        ECCclass->dev_uevent = ECCdev_uevent; 

    for (n_device = 0; n_device < NUM_DEVICES; n_device++) {
        cdev_init(&ECCcdev[n_device], &ECC_fops);
        id_device = MKDEV(MAJOR(major_minor), MINOR(major_minor) + n_device);
        
        if(cdev_add(&ECCcdev[n_device], id_device, 1) == -1) {
            goto error;
        }

        if(device_create(ECCclass, NULL, id_device, NULL, DRIVER_NAME "%d", n_device) == NULL) {
            goto error;
        }
        
        // Inicializar longitud de datos
        ECC_devices[n_device].length = 0;
        pr_info("Device node /dev/%s%d created\n", DRIVER_NAME, n_device);
    }

    pr_info("ECC driver initialized and loaded\n");
    return 0;

error:
    if(ECCclass) class_destroy(ECCclass);
    if(major_minor != -1) unregister_chrdev_region(major_minor, NUM_DEVICES);
    return -1;
}

static void __exit exit_driver(void) {
    int n_device;
    for (n_device = 0; n_device < NUM_DEVICES; n_device++) {
        device_destroy(ECCclass, MKDEV(MAJOR(major_minor), MINOR(major_minor) + n_device));
        cdev_del(&ECCcdev[n_device]);
    }
    class_destroy(ECCclass);
    unregister_chrdev_region(major_minor, NUM_DEVICES);
    pr_info("ECC driver unloaded\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Francisco Charte");
MODULE_VERSION("0.2");
MODULE_DESCRIPTION("Skeleton of a full device driver module - P1 Diego Gomez");

module_init(init_driver)
module_exit(exit_driver)
