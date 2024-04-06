/* SPDX-License-Identifier: BSD-2-Clause WITH Linux-syscall-note */

#ifndef _UAPI_LINUX_MEMINFO_H
#define _UAPI_LINUX_MEMINFO_H

/* Adding event notification support elements */
#define MEMINFO_GENL_FAMILY_NAME "meminfo"
#define MEMINFO_GENL_VERSION 0x01
#define MEMINFO_GENL_MCGRP_GROUP_NAME "mcgrp"

/* Attributes of meminfo_genl_family */
enum meminfo_genl_attr {
	MEMINFO_GENL_ATTR_UNSPEC,
	MEMINFO_GENL_ATTR_DATA,
	__MEMINFO_GENL_ATTR_MAX,
};

#define MEMINFO_GENL_ATTR_MAX (__MEMINFO_GENL_ATTR_MAX - 1)

/* Commands supported by the meminfo_genl_family */
enum meminfo_genl_cmd {
	MEMINFO_GENL_CMD_UNSPEC,
	MEMINFO_GENL_CMD_COMMAND, /* Test command */
	__MEMINFO_GENL_CMD_MAX,
};

#define MEMINFO_GENL_CMD_MAX (__MEMINFO_GENL_CMD_MAX - 1)

#endif /* _UAPI_LINUX_MEMINFO_H */
