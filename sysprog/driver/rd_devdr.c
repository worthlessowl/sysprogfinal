#include <linux/init.h> 
#include <linux/module.h> 
#include <linux/device.h> 
#include <linux/kernel.h>
#include <linux/fs.h>   
#include <asm/uaccess.h> 

#define  DEVICE_NAME "rdcdevdrowlver"    // /dev/<value>
#define  CLASS_NAME  "rdc"          // The device class

MODULE_LICENSE("GPL");            
MODULE_AUTHOR("Ardhi Putra Pratama Modified By Muhammad Rizqi Agung Prabowo");    
MODULE_DESCRIPTION("Le simple Linux char driver");
MODULE_VERSION("0.1");        
static int    majorNumber;                  
static char   message[256] = {0};           
static short  size_of_message;         
static int    numberOpens = 0;              
static struct class*  classptr  = NULL;     
static struct device* dvcptr = NULL;        

// The prototype functions for the character driver
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};


static int dev_open(struct inode *inodep, struct file *filep){
   numberOpens++;
   printk(KERN_INFO "RDcdevdrowlver: Device has been opened %d time(s)\n", numberOpens);
   return 0;
}

static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "RDcdevdrowlver: Device successfully closed\n");
   return 0;
}


static int __init rdchardrv_init(void){
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0) return majorNumber;
   // Register the device class
   classptr = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(classptr)){
           unregister_chrdev(majorNumber, DEVICE_NAME);
           return PTR_ERR(classptr);
   }


   // Register the device driver
   dvcptr = device_create(classptr, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(dvcptr)){
          class_destroy(classptr);
          unregister_chrdev(majorNumber, DEVICE_NAME);
          return PTR_ERR(dvcptr);
   }
   printk(KERN_INFO "RDcdevdrowlver: device class created correctly\n");
   return 0;
}


static void __exit rdchardrv_exit(void){
    device_destroy(classptr, MKDEV(majorNumber, 0));      
    class_unregister(classptr);                          
    class_destroy(classptr);                              
    unregister_chrdev(majorNumber, DEVICE_NAME);              
    printk(KERN_INFO "RDcdevdrowlver: Goodbye from the LKM!\n");
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
    // precaution
    size_t bytesToCopy = len >= size_of_message ? size_of_message: len;
    size_t bytesNotCopied = 0;
    if(!bytesToCopy) return 0;

bytesNotCopied = raw_copy_to_user(buffer, message,
bytesToCopy);
    if(bytesNotCopied){
         return -EFAULT;
    }
    size_of_message = 0;
    return bytesToCopy;
}


static ssize_t dev_write(struct file *filep,
const char *buffer, size_t len, loff_t *offset){

    const size_t maxLen = 256 - 1;
    size_t bytesToCopy = len >= maxLen ? maxLen: len;
    size_t bytesNotCopied = 0;

    bytesNotCopied = raw_copy_from_user(message, buffer,bytesToCopy);
    size_of_message = bytesToCopy - bytesNotCopied;

    if(bytesNotCopied)
            return -EFAULT;
    return bytesToCopy;
}

module_init(rdchardrv_init);
module_exit(rdchardrv_exit);
