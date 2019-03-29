#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h> //printk()
#include<linux/errno.h>
#include<linux/types.h>
#include<linux/proc_fs.h>

//copy_from,to_user
// #include <asm/uaccess.h>
#include <linux/uaccess.h>

#include<linux/slab.h>

#include<linux/mm.h> //remap_pfn_range
//#include<linux/mman.h> //private_mapping_ok
#define BUFF_SIZE 128
#define DEV_NAME "MyDevice"
MODULE_LICENSE("GPL");

//Method declarations
int mod_open(struct inode *,struct file *);
int mod_release(struct inode *,struct file *);
static ssize_t mod_read(struct file *,char *,size_t ,loff_t *);
// ssize_t mod_write(struct file *,char *,size_t ,loff_t *);
static ssize_t mod_write(struct file *, const char *, size_t, loff_t *);
int mod_mmap(struct file *, struct vm_area_struct *);


void mod_exit(void);
int mod_init(void);

//Structure that declares the usual file access functions

struct file_operations mod_fops = {
    read: mod_read,
    write: mod_write,
    open: mod_open,
    release: mod_release,
    mmap: mod_mmap
};


static int mod_page_mkwrite(struct vm_fault *vmf) {

    // struct vm_area_struct *vma = vmf->vma;

    printk(KERN_ALERT "mhasan->Call");

    return 0;

}

static const struct vm_operations_struct mod_mem_ops = {
    .page_mkwrite	= mod_page_mkwrite,
};


module_init(mod_init);
module_exit(mod_exit);

char *read_buf;
char *write_buf;

static int Major;
//static int Device_Open = 0;
int buffsize = 0;

int mod_init(void)
{
    Major = register_chrdev(0,DEV_NAME,&mod_fops);
    if(Major < 0)
    {
        printk(KERN_ALERT "Can not register %s. No major number alloted",DEV_NAME);
        return Major;
    }
    //allocate memory to buffers
    read_buf = kmalloc(BUFF_SIZE, GFP_KERNEL);
    write_buf = kmalloc(BUFF_SIZE, GFP_KERNEL);

    if(!read_buf || !write_buf)
    {
        mod_exit();
        return -ENOMEM;
    }
    //reset buffers
    memset(read_buf,0, BUFF_SIZE);
    memset(write_buf,0, BUFF_SIZE);
    printk(KERN_INFO "I was assigned major number %d. To talk to\n", Major);
        printk(KERN_INFO "the driver, create a dev file with\n");
        printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n",DEV_NAME, Major);
        printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
        printk(KERN_INFO "the device file.\n");
        printk(KERN_INFO "Remove the device file and module when done.\n");
    return 0;


}

void mod_exit(void)
{
    unregister_chrdev(Major,"memory");
    if(read_buf) kfree(read_buf);
    if(write_buf) kfree(write_buf);
    printk(KERN_INFO "removing module\n");
}

int mod_mmap(struct file *filp, struct vm_area_struct *vma)
{
    printk(KERN_INFO "mhasan PRINT \n");

    size_t size = vma->vm_end - vma->vm_start;
    vma->vm_ops = &mod_mem_ops;

    vma->vm_pgoff = virt_to_phys(read_buf)>>PAGE_SHIFT;

    /* Remap-pfn-range will mark the range VM_IO */
    if (remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff, size, vma->vm_page_prot)) {
        return -EAGAIN;
    }
    printk(KERN_INFO "VMA Open. Virt_addr: %lx, phy_addr: %lx\n",vma->vm_start, vma->vm_pgoff<<PAGE_SHIFT);

    // vma->vm_flags &= ~VM_WRITE;
    // vma->vm_page_prot = phys_mem_access_prot(filp, vma->vm_pgoff,
	// 					 PAGE_SIZE,
	// 					 VM_READ);


    // printk(KERN_INFO "VMA PAGE PROT \n");

    return 0;
}


static ssize_t mod_read(struct file *filp, char *buf, size_t len, loff_t *f_pos)
{
    ssize_t bytes;
    if(buffsize < len)
        bytes = buffsize;
    else
        bytes = len;
    printk(KERN_INFO "Buffer size availabe: %d\n", buffsize);
    printk(KERN_INFO "VMA Open. read buffer initial: %lx\n",read_buf);

    if(bytes == 0)
        return 0;

    // memcpy(read_buf, "tw", strlen(tw));
    // strcpy (read_buf,"copy successful");
    // memset (read_buf,'-',2);

    int retval = copy_to_user(buf,read_buf,bytes);
    if(retval)
    {
        printk(KERN_INFO "copy_to_user fail");
        return -EFAULT;
    }
    else
    {
        printk(KERN_INFO "copy_to_user succeeded\n");
        buffsize -= bytes;
        return bytes;
    }
}

static ssize_t mod_write( struct file *filp, const char *buf, size_t len, loff_t *f_pos)
{

    printk(KERN_INFO "mhasan WRITE \n");


    memset(read_buf,0,BUFF_SIZE);
    memset(write_buf,0,BUFF_SIZE);
    if(len > BUFF_SIZE)
    {
        printk(KERN_ALERT "Buffer not available. Writing only %d bytes.\n",BUFF_SIZE);
        len = BUFF_SIZE;
    }
    printk(KERN_INFO "User space msg size: %d\n",len);
    int retval = copy_from_user(read_buf,buf,len);
    printk(KERN_INFO "read %d bytes as: %s\n", retval,read_buf);

//  memcpy(write_buf,read_buf,len);
//  printk(KERN_INFO "written: %s\n", write_buf);
    buffsize = len;
    return len;
}

int mod_open(struct inode *inode, struct file *filp){return 0;}
int mod_release(struct inode *inode, struct file *filp) {return 0;}
