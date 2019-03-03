#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/mm.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL v2");

int filewrite(struct file *fp,char buf[],struct task_struct* p);
int search_Proc(void);


static int __init hello_init(void)
{
	printk(KERN_INFO "HELLO WORLD!\n");
	search_Proc();
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_INFO "GOOD BYE!\n");
}

int search_Proc(void)
{
	char buf[100];
        struct task_struct *p,*task;
        struct list_head  *pos;
        int count=0;
	char* file_proc="file_proc.txt";
	struct file *fp;
        task=&init_task;
		
	fp=filp_open(file_proc,O_RDWR | O_CREAT,0);
	if(IS_ERR(fp))
	{
		printk("open file failed\n");
		return -1;
	}
		
	list_for_each(pos,&task->tasks)
        {
                p=list_entry(pos,struct task_struct,tasks);             
		filewrite(fp,buf,p);
                count++;
        }
		
	filp_close(fp,NULL);
        printk("the process number is %d\n",count);
	return 0;
}

int filewrite(struct file *fp,char buf[],struct task_struct* p)
{
	mm_segment_t fs;
	static off_t pos=0;
	loff_t pos1=pos;

	int size;
	size=sprintf(buf,"%d %s\n",p->pid,p->comm);
	
	fs=get_fs();
	set_fs(KERNEL_DS);
	vfs_write(fp,buf,size,&pos1);
	set_fs(fs);
	
	pos+=size;
	return 0;
}

module_init(hello_init);
module_exit(hello_exit);
	
