#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/string.h>


#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("hhchung @ NCTU");
//MODULE_DESCRIPTION("sys module");
//MODULE_ALIAS("sysplugin")


#define MYMSG(fmt, ...) pr_info(fmt "\n", ##__VA_ARGS__)
#define DEBUG(fmt, ...) pr_info("[DEBUG]" fmt "\n", ##__VA_ARGS__)
#define SHM_ADDR 0x0
#define SHM_SIZE 0x1000
//create a node @ /dev/SYS_PLUGIN
#define DEVICE_NAME "SYS_PLUGIN"
#define CLASS_NAME  "SYS_PLUGIN_CLASS"
static struct class* sysClass = NULL;
static struct device* sysDevice = NULL;


static int     mydriver_open(struct inode *, struct file *);
static int     mydriver_release(struct inode *, struct file *);
static ssize_t mydriver_read(struct file *, char *, size_t, loff_t *);
static ssize_t mydriver_write(struct file *, const char *, size_t, loff_t *);





//for character device testing
static struct file_operations fops = {
    .open = mydriver_open,
    .read = mydriver_read,
    .write = mydriver_write,
    .release = mydriver_release,
};
static int majorNum;
static int minorNum;
static volatile void *base;







static int __init mydriver_platform_init(void){
    MYMSG("mydriver init");
       
    majorNum = register_chrdev(0, DEVICE_NAME, &fops);
    if(majorNum < 0 ){
        DEBUG("Fail to register a major number");
        return majorNum; 
    } 
    
    MYMSG("Register %s with major number=%d", DEVICE_NAME, majorNum);
    sysClass = class_create(THIS_MODULE, CLASS_NAME);
    if(IS_ERR(sysClass)){
        unregister_chrdev(majorNum, DEVICE_NAME);
        DEBUG("Fail to register class %s", CLASS_NAME);
        return PTR_ERR(sysClass);   
    }
    
    sysDevice = device_create(sysClass, NULL, MKDEV(majorNum, 0), NULL,  DEVICE_NAME);
    if(IS_ERR(sysDevice)){
        class_destroy(sysClass);
        unregister_chrdev(majorNum, DEVICE_NAME);
        DEBUG("Fail to create device %s", DEVICE_NAME);
        return PTR_ERR(sysDevice);
    }

    MYMSG("Success to create device class");
    return 0;
}

static void __exit mydriver_platform_exit(void){
	pr_info("mydriver exit\n");
    device_destroy(sysClass, MKDEV(majorNum, 0));
    class_unregister(sysClass);
    class_destroy(sysClass);
    unregister_chrdev(majorNum, DEVICE_NAME);
    MYMSG("Success remove %s", DEVICE_NAME);
}


static int mydriver_open(struct inode *inode, struct file *file){
    DEBUG("call open operation");
    if( base == NULL )
        base = ioremap(SHM_ADDR, 0x1000);
    DEBUG("map address GPA %p to GVA %p", (void *)SHM_ADDR, base); 
    return 0;    
}



static ssize_t mydriver_read(struct file *file, char *buf, size_t len, loff_t *off){
    int err;
    err = copy_to_user(buf, base, len);
    if(err == 0 ){
        DEBUG("success read [%s]", base); 
        return 0;
    }else{
        DEBUG("Fail to read data from %p", SHM_ADDR);
        return -EFAULT;
    }
}

static ssize_t mydriver_write(struct file *file, const char *buf, size_t len, loff_t *off){
    if( len < SHM_SIZE ){
        memcpy(base, buf, len);
        return len;
    }else{
        DEBUG("Write %d bytes but only has %d bytes can be written", len, SHM_SIZE);
        return -EFAULT;
    }
} 

static int mydriver_release(struct inode *inode, struct file *file){
    iounmap(base);
    base = NULL;
    DEBUG("close %s", DEVICE_NAME);
    return 0;
}
module_init(mydriver_platform_init)
module_exit(mydriver_platform_exit)
