#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

static struct dentry *dir, *inputdir, *ptreedir;
static struct task_struct *curr;
static struct debugfs_blob_wrapper blob;

static ssize_t write_pid_to_input(struct file *fp, 
                                const char __user *user_buffer, 
                                size_t length, 
                                loff_t *position)
{
    pid_t input_pid;

    sscanf(user_buffer, "%u", &input_pid);
	printk("%d", input_pid);
        
	// Find task_struct using input_pid. Hint: pid_task
	curr = pid_task(find_get_pid(input_pid), PIDTYPE_PID);
	printk("found task_struct using input_pid");
        // Tracing process tree from input_pid to init(1) process

        // Make Output Format string: process_command (process_id)
	blob.size += sprintf(blob.data+blob.size, "%s (%d)\n", curr->comm, curr->pid);
	printk("made output format");

    return length;
}

static const struct file_operations dbfs_fops = {
        .write = write_pid_to_input,
};

static int __init dbfs_module_init(void)
{
        // Implement init module code
	static char buffer[100000];
	blob.data = buffer;
	blob.size = 0;

        dir = debugfs_create_dir("ptree", NULL);
        
        if (!dir) {
                printk("Cannot create dir\n");
                return -1;
        }
	printk("dir created\n");

        inputdir = debugfs_create_file("input", 0666, dir, blob.data, &dbfs_fops);
	if (!inputdir) {
                printk("Cannot create input dir\n");
                return -1;
        }
	printk("inputdir created\n");
        ptreedir = debugfs_create_blob("ptree", 0666, dir, &blob);
	if (!ptreedir) {
                printk("Cannot create ptree dir\n");
                return -1;
        }
	printk("ptreedir created\n"); 
	// Find suitable debugfs API

	
	printk("dbfs_ptree module initialize done\n");

        return 0;
}

static void __exit dbfs_module_exit(void)
{
        // Implement exit module code
	debugfs_remove_recursive(dir);
	
	printk("dbfs_ptree module exit\n");
}

module_init(dbfs_module_init);
module_exit(dbfs_module_exit);
