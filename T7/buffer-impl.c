/* Necessary includes for device drivers */
#include <linux/init.h>
/* #include <linux/config.h> */
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include <linux/uaccess.h> /* copy_from/to_user */

#include "kmutex.h"

MODULE_LICENSE("Dual BSD/GPL");

/* Declaration of functions */
static int buffer_open(struct inode *inode, struct file *filp);
static int buffer_release(struct inode *inode, struct file *filp);
static ssize_t buffer_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
static ssize_t buffer_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
void buffer_exit(void);
int buffer_init(void);

/* Structure that declares the usual file */
/* access functions */
static struct file_operations buffer_fops = {
  read: buffer_read,
  write: buffer_write,
  open: buffer_open,
  release: buffer_release
};

/* Declaration of the init and exit functions */
module_init(buffer_init);
module_exit(buffer_exit);

/* Global variables of the driver */
/* Major number */
static int buffer_major = 61;
/* Buffer to store data */
#define MAX_SIZE 80
#define MAX_BUFFER_SIZE 3   
static char **buffer;
static ssize_t *curr_size;
static int read_pos;
static int write_pos;

static KMutex mutex;
static KCondition cond;

int buffer_init(void) {
    int rc;

    /* Registering device */
    rc = register_chrdev(buffer_major, "buffer", &buffer_fops);
    if (rc < 0) {
        printk("<1>buffer: cannot obtain major number %d\n", buffer_major);
        return rc;
    }

    read_pos= 0;
    write_pos= 0;
    m_init(&mutex);
    c_init(&cond);

    buffer = kmalloc(MAX_BUFFER_SIZE * sizeof(char*), GFP_KERNEL);
    if (buffer==NULL) {
        buffer_exit();
        return -ENOMEM;
    }
    /* Allocating syncread_buffer */
    for(ssize_t i=0; i<MAX_BUFFER_SIZE; i++) {
        buffer[i] = kmalloc(MAX_SIZE, GFP_KERNEL);
        if (buffer[i]==NULL) {
            buffer_exit();
            return -ENOMEM;
        }
        memset(buffer[i], 0, MAX_SIZE);
    }
    curr_size = kmalloc(MAX_BUFFER_SIZE * sizeof(ssize_t), GFP_KERNEL);
    if (curr_size==NULL) {
        buffer_exit();
        return -ENOMEM;
    }
    memset(curr_size, 0, MAX_BUFFER_SIZE * sizeof(ssize_t));
    printk("<1>Inserting syncread module\n");
    return 0;
}

void buffer_exit(void) {
    /* Freeing the major number */
    unregister_chrdev(buffer_major, "buffer");

    /* Freeing buffer syncread */
    if (buffer) {
        for(ssize_t i=0; i<MAX_BUFFER_SIZE; i++) {
            if (buffer[i]) {
                kfree(buffer[i]);
            }
        }
        kfree(buffer_buffer);
    }
    if (curr_size) {
        kfree(curr_size);
    }

    printk("<1>Removing buffer module\n");
}

static int buffer_open(struct inode *inode, struct file *filp) {
    char *mode= filp->f_mode & FMODE_WRITE ? "write" :
                filp->f_mode & FMODE_READ ? "read" :
                "unknown";
    printk("<1>open %p for %s\n", filp, mode);
    return 0;
}

static int buffer_release(struct inode *inode, struct file *filp) {
    printk("<1>release %p\n", filp);
    return 0;
}

static ssize_t buffer_read(struct file *filp, char *buf, size_t count, loff_t *f_pos) {
    ssize_t rc;
    m_lock(&mutex);
    if(*f_pos!=0) {
        m_unlock(&mutex);
        return 0;
    }
    /* mientras no hay nada en el buffer, se espera */
    while(curr_size[read_pos]==0) {
       if(c_wait(&cond, &mutex)) {
            printk("<1>buffer_read: interrupted\n");
            m_unlock(&mutex);
            return -EINTR;
       }
    }

    if (count>curr_size[read_pos]-*f_pos) {
        count= curr_size[read_pos]-*f_pos;
    }

    printk("<1>buffer_read: reading %d bytes at %d\n", (int)count, (int)*f_pos);

    if(copy_to_user(buf, buffer[read_pos]+*f_pos, count)!=0) {
        printk("<1>buffer_read: copy_to_user failed\n");
        m_unlock(&mutex);
        return -EFAULT;
    }

    *f_pos+= count;
    curr_size[read_pos]= 0;
    read_pos = (read_pos+1)%MAX_BUFFER_SIZE;
    m_unlock(&mutex);
    c_broadcast(&cond);
    return count;
}

static ssize_t buffer_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos) {
    ssize_t rc;
    loff_t last;
    m_lock(&mutex);

    /* mientras el buffer esta lleno, se espera */
    while(curr_size[write_pos]!=0) {
        if(c_wait(&cond, &mutex)) {
            printk("<1>buffer_write: interrupted\n");
            m_unlock(&mutex);
            return -EINTR;
        }
    }

    last = *f_pos + count;
    if (last>MAX_SIZE) {
        count -= last-MAX_SIZE;
    }
    printk("<1>write %d bytes at %d\n", (int)count, (int)*f_pos);

    if(copy_from_user(buffer[write_pos]+*f_pos, buf, count)!=0) {
        printk("<1>buffer_write: copy_from_user failed\n");
        m_unlock(&mutex);
        return -EFAULT;
    }

    *f_pos+= count;
    curr_size[write_pos]= *f_pos;
    write_pos = (write_pos+1)%MAX_BUFFER_SIZE;
    m_unlock(&mutex);
    c_broadcast(&cond);
    return count;
}
