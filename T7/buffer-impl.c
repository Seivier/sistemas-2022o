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
static char *buffer;
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

    /* Allocating memory for the buffer */
    buffer = kmalloc(MAX_SIZE*MAX_BUFFER_SIZE, GFP_KERNEL);
    if (buffer==NULL) {
        buffer_exit();
        return -ENOMEM;
    }
    memset(buffer, 0, MAX_SIZE*MAX_BUFFER_SIZE);

    curr_size = kmalloc(MAX_BUFFER_SIZE * sizeof(ssize_t), GFP_KERNEL);
    if (curr_size==NULL) {
        buffer_exit();
        return -ENOMEM;
    }
    memset(curr_size, 0, MAX_BUFFER_SIZE * sizeof(ssize_t));
    printk("<1>Inserting buffer module with %d bytes\n", MAX_SIZE*MAX_BUFFER_SIZE);
    return 0;
}

void buffer_exit(void) {
    /* Freeing the major number */
    unregister_chrdev(buffer_major, "buffer");

    /* Freeing buffer syncread */
    if (buffer) {
        kfree(buffer);
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
    printk("<1>open for %s\n", mode);
    return 0;
}

static int buffer_release(struct inode *inode, struct file *filp) {
    printk("<1>release \n");
    return 0;
}

static ssize_t buffer_read(struct file *filp, char *buf, size_t count, loff_t *f_pos) {
    m_lock(&mutex);
    /* 
    Implementación: 
    Asumimos que la segunda vez que se lee siempre se debe enviar la señal de final de archivo,
    es decir, que siempre se lee un count < MAX_SIZE
    */
    if(*f_pos!=0) {
        m_unlock(&mutex);
        return 0;
    }
    /* mientras no hay nada en el buffer, se espera */
    while(curr_size[read_pos] == 0) {
       if(c_wait(&cond, &mutex)) {
            printk("<1>buffer_read: interrupted\n");
            m_unlock(&mutex);
            return -EINTR;
       }
       printk("<1>buffer_read: awoken \n");
    }

    /* en caso de overflow */
    if (count>curr_size[read_pos]-*f_pos) {
        count= curr_size[read_pos]-*f_pos;
    }

    printk("<1>buffer_read: reading %d bytes at %d\n", (int)count, (int)*f_pos);

    if(copy_to_user(buf, buffer+*f_pos+MAX_SIZE*read_pos, count)!=0) {
        printk("<1>buffer_read: copy_to_user failed\n");
        m_unlock(&mutex);
        return -EFAULT;
    }

    *f_pos+= count;
    curr_size[read_pos]= 0;
    read_pos = (read_pos+1)%MAX_BUFFER_SIZE;
    c_broadcast(&cond);
    m_unlock(&mutex);
    return count;
}

static ssize_t buffer_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos) {
    loff_t last;
    m_lock(&mutex);

    /* mientras el buffer esta lleno, se espera. Esto se verifica cuando el puntero para guardar esta sobre algo con informacion */
    while(curr_size[write_pos] != 0) {
        if(c_wait(&cond, &mutex)) {
            printk("<1>buffer_write: interrupted\n");
            m_unlock(&mutex);
            return -EINTR;
        }
        printk("<1>buffer_write: awoken\n");
    }

    /* solo por si acaso, aunque para el caso de write no hay overflow */
    last = *f_pos + count;
    if (last>MAX_SIZE) {
        count -= last-MAX_SIZE;
    }
    printk("<1>write %d bytes at %d\n", (int)count, (int)*f_pos);

    if(copy_from_user(buffer+*f_pos+MAX_SIZE*write_pos, buf, count)!=0) {
        printk("<1>buffer_write: copy_from_user failed\n");
        m_unlock(&mutex);
        return -EFAULT;
    }

    *f_pos+= count;
    curr_size[write_pos]= (int)*f_pos;
    write_pos = (write_pos+1)%MAX_BUFFER_SIZE;
    c_broadcast(&cond);
    m_unlock(&mutex);
    return count;
}
