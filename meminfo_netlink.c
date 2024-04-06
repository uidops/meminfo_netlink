#include <linux/module.h>
#include <linux/kernel.h>
#include <net/genetlink.h>

#include <uapi/linux/meminfo.h>

MODULE_LICENSE("BSD-2-Clause");
MODULE_AUTHOR("uidops");
MODULE_DESCRIPTION("meminfo netlink kernel module");
MODULE_VERSION("0.1");


enum meminfo_genl_multicast_groups {
    MEMINFO_GENL_MCGRP_GROUP = 0,
};


static const struct genl_multicast_group meminfo_genl_mcgrps[] = {
    [MEMINFO_GENL_MCGRP_GROUP] = { .name = MEMINFO_GENL_MCGRP_GROUP_NAME, },
};


static const struct nla_policy meminfo_genl_policy[MEMINFO_GENL_ATTR_MAX + 1] = {
	[MEMINFO_GENL_ATTR_DATA] = { .type = NLA_U32, },
};

static struct genl_family meminfo_gnl_family;


static int meminfo_command(struct genl_info *info, struct sk_buff *msg)
{
	int id = 0;
	if (!info->attrs[MEMINFO_GENL_ATTR_DATA])
		return -EINVAL;

	id = nla_get_u32(info->attrs[MEMINFO_GENL_ATTR_DATA]);
	id = id << 1;

	if (nla_put_u32(msg, MEMINFO_GENL_ATTR_DATA, id))
		return -EMSGSIZE;

	return 0;
}


static int (*cmd_cb[MEMINFO_GENL_CMD_MAX + 1])(struct genl_info *, struct sk_buff *) = {
	[MEMINFO_GENL_CMD_COMMAND] = meminfo_command,
};


static int meminfo_genl_cmd_doit(struct sk_buff *skb, struct genl_info *info)
{
	struct sk_buff *msg;
	void *hdr;
	int cmd = info->genlhdr->cmd;
	int ret = -EMSGSIZE;

	msg = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
	if (!msg)
		return -ENOMEM;

	hdr = genlmsg_put_reply(msg, info, &meminfo_gnl_family, 0, cmd);
	if (!hdr)
		goto out_free_msg;

	ret = cmd_cb[cmd](info, msg);
	if (ret)
		goto out_cancel_msg;

	genlmsg_end(msg, hdr);

	return genlmsg_reply(msg, info);

out_cancel_msg:
	genlmsg_cancel(msg, hdr);
out_free_msg:
	nlmsg_free(msg);

	return ret;
}


static const struct genl_small_ops meminfo_genl_ops[] = {
	{
		.cmd = MEMINFO_GENL_CMD_COMMAND,
		.validate = GENL_DONT_VALIDATE_STRICT | GENL_DONT_VALIDATE_DUMP,
		.doit = meminfo_genl_cmd_doit,
	},
};


static struct genl_family meminfo_gnl_family = {
	.name       = MEMINFO_GENL_FAMILY_NAME,
	.version    = MEMINFO_GENL_VERSION,
	.maxattr    = MEMINFO_GENL_ATTR_MAX,
	.policy     = meminfo_genl_policy,
	.small_ops  = meminfo_genl_ops,
	.n_small_ops = ARRAY_SIZE(meminfo_genl_ops),
	.resv_start_op = MEMINFO_GENL_CMD_MAX + 1,
	.mcgrps = meminfo_genl_mcgrps,
	.n_mcgrps = ARRAY_SIZE(meminfo_genl_mcgrps),
};


static int __init meminfo_netlink_init(void)
{
	int ret = 0;
	printk(KERN_INFO "Registering meminfo_netlink\n");

	ret = genl_register_family(&meminfo_gnl_family);
	if (ret) {
		pr_err("Registering generic family faild\n");
	}

	return 0;
}

static void __exit meminfo_netlink_exit(void)
{
	int ret = 0;
	printk(KERN_INFO "Unregistering meminfo_netlink\n");

	ret = genl_unregister_family(&meminfo_gnl_family);
	if (ret) {
		pr_err("Unregistering generic family faild\n");
	}

}

module_init(meminfo_netlink_init);
module_exit(meminfo_netlink_exit);
