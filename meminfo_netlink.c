#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("BSD-2-Clause");
MODULE_AUTHOR("uidops");
MODULE_DESCRIPTION("meminfo netlink kernel module");
MODULE_VERSION("0.1");


static int __init meminfo_netlink_init(void)
{
	printk(KERN_INFO "Registering meminfo_netlink\n");
	return 0;
}

static void __exit meminfo_netlink_exit(void)
{
	printk(KERN_INFO "Unregistering meminfo_netlink\n");
}

module_init(meminfo_netlink_init);
module_exit(meminfo_netlink_exit);
