#include "DGDriver.h"

/* Memoria estática para los dispositivos */
static struct dg_device_data dg_devices[NUM_DEVICES];

/* LECTURA NORMAL (Minor 0) */
static ssize_t dg_read_normal(struct file *file, char __user *buffer, size_t count, loff_t *f_pos) {
    int minor = iminor(file_inode(file));
    if (*f_pos >= dg_devices[minor].length) return 0;
    
    if (count > dg_devices[minor].length - *f_pos)
        count = dg_devices[minor].length - *f_pos;
        
    if (copy_to_user(buffer, dg_devices[minor].buffer + *f_pos, count))
        return -EFAULT;
        
    *f_pos += count;
    return count;
}

/* LECTURA LEET (Minor 1) */
static ssize_t dg_read_leet(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    char tmp[BUFFER_SIZE];
    if (*ppos > 0) return 0;
    if (count > BUFFER_SIZE) count = BUFFER_SIZE;

    for (int i = 0; i < count; i++) {
        char c = dg_devices[0].buffer[i]; // Basado en lo escrito en el minor 0
        switch (c) {
            case 'a': case 'A': tmp[i] = '4'; break;
            case 'e': case 'E': tmp[i] = '3'; break;
            case 'i': case 'I': tmp[i] = '2'; break;
            case 'o': case 'O': tmp[i] = '1'; break;
            case 'u': case 'U': tmp[i] = '0'; break;
            default:            tmp[i] = c;   break;
        }
    }
    if (copy_to_user(buf, tmp, count)) return -EFAULT;
    *ppos += count;
    return count;
}

/* LECTURA BINARIA (Minor 2) */
static ssize_t dg_read_bin(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    char bin_buf[BUFFER_SIZE];
    int pos = 0;
    size_t data_len = dg_devices[1].length;

    if (*ppos > 0) return 0;

    for (int i = 0; i < data_len && pos + 8 < BUFFER_SIZE; i++) {
        char c = dg_devices[1].buffer[i];
        for (int j = 7; j >= 0; j--) {
            bin_buf[pos++] = ((c >> j) & 1) ? '1' : '0';
        }
        if (pos < BUFFER_SIZE) bin_buf[pos++] = ' ';
    }

    if (count > pos) count = pos;
    if (copy_to_user(buf, bin_buf, count)) return -EFAULT;
    *ppos += count;
    return count;
}

/* Dispatcher de lectura */
static ssize_t dg_read_selector(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    int minor = iminor(file_inode(file));
    switch(minor) {
        case 0: return dg_read_normal(file, buf, count, ppos);
        case 1: return dg_read_leet(file, buf, count, ppos);
        case 2: return dg_read_bin(file, buf, count, ppos);
        default: return -EINVAL;
    }
}

static ssize_t dg_write(struct file *file, const char __user *buffer, size_t count, loff_t *f_pos) {
    int minor = iminor(file_inode(file));
    memset(dg_devices[minor].buffer, 0, BUFFER_SIZE);
    if (count > BUFFER_SIZE) count = BUFFER_SIZE;
    if (copy_from_user(dg_devices[minor].buffer, buffer, count)) return -EFAULT;
    dg_devices[minor].length = count;
    return count;
}

const struct file_operations dg_fops = {
    .owner = THIS_MODULE,
    .read = dg_read_selector,
    .write = dg_write,
};
