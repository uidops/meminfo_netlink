#include <linux/netlink.h>
#include <linux/genetlink.h>
#include <linux/meminfo.h>
#include <sys/socket.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define GENLMSG_DATA(gh) ((void *)(NLMSG_DATA(gh) + GENL_HDRLEN))
#define NLA_DATA(na) ((void *)((char*)(na) + NLA_HDRLEN))

#define BUFLEN (1 << 12)

struct raw_netlink_generic_metadata {
	struct nlmsghdr nlmh;
	struct genlmsghdr genlmh;
	unsigned char attrs[256];
};


int
main(void)
{
	int fd = 0;
	uint8_t *buf = NULL;
	struct sockaddr_nl sa = {0};
	struct raw_netlink_generic_metadata req = {0};
	struct nlattr *nla = NULL;
	struct nlmsghdr *nlmh = NULL;
	uint32_t family_id = 0;
	uint32_t data = 0;

	fd = socket(AF_NETLINK, SOCK_RAW | SOCK_NONBLOCK, NETLINK_GENERIC);
	if (fd == -1)
		err(EXIT_FAILURE, "socket()");

	sa.nl_family = AF_NETLINK;
	sa.nl_pad = 0;
	sa.nl_pid = getpid();
	sa.nl_groups = 0;

	if (bind(fd, (struct sockaddr *) &sa, sizeof(struct sockaddr_nl)) == -1) {
		close(fd);
		err(EXIT_FAILURE, "bind()");
	}

	req.nlmh.nlmsg_len = NLMSG_LENGTH(GENL_HDRLEN);
	req.nlmh.nlmsg_type = GENL_ID_CTRL;
	req.nlmh.nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
	req.nlmh.nlmsg_seq = 1;
	req.nlmh.nlmsg_pid = getpid();

	req.genlmh.cmd = CTRL_CMD_GETFAMILY;
	req.genlmh.version = 2;
	req.genlmh.reserved = 0;

	nla = (struct nlattr *) GENLMSG_DATA(&req);
	nla->nla_len = strlen(MEMINFO_GENL_FAMILY_NAME) + 1 + NLA_HDRLEN;
	nla->nla_type = CTRL_ATTR_FAMILY_NAME;
	strcpy(NLA_DATA(nla), MEMINFO_GENL_FAMILY_NAME);
	
	req.nlmh.nlmsg_len += NLA_ALIGN(nla->nla_len);

	if (send(fd, (void *) &req, req.nlmh.nlmsg_len, 0) == -1) {
		close(fd);
		err(EXIT_FAILURE, "send(CTRL_CMD_GETFAMILY)");
	}

	buf = calloc(BUFLEN, sizeof(uint8_t));
	if (buf == NULL) {
		close(fd);
		err(EXIT_FAILURE, "calloc()");
	}

	while (recv(fd, buf, BUFLEN, 0) > 0) {
		nlmh = (struct nlmsghdr *) buf;
		nla = GENLMSG_DATA(nlmh);
		if (nlmh->nlmsg_type == NLMSG_ERROR && family_id == 0) {
			free(buf);
			close(fd);
			errx(EXIT_FAILURE, "meminfo netlink is not implemented");
		}

		while ((int64_t)((char *) nla - (char *) nlmh) < nlmh->nlmsg_len - NLA_HDRLEN) {
			if (nla->nla_type == CTRL_ATTR_FAMILY_ID) {
				family_id = *((uint32_t *) NLA_DATA(nla));
				break;
			}

			nla = (struct nlattr *) ((char *) nla + NLA_ALIGN(nla->nla_len));
		}
	}

    req.nlmh.nlmsg_len = NLMSG_LENGTH(GENL_HDRLEN);
    req.nlmh.nlmsg_type = family_id;
    req.nlmh.nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
    req.nlmh.nlmsg_seq = 2;

    req.genlmh.cmd = MEMINFO_GENL_CMD_COMMAND;
    req.genlmh.version = MEMINFO_GENL_VERSION;

    nla = (struct nlattr *) GENLMSG_DATA(&req);
    nla->nla_len = sizeof(uint32_t) + NLA_HDRLEN;
    nla->nla_type = MEMINFO_GENL_ATTR_DATA;
	
	data = 10;
    memcpy(NLA_DATA(nla), &data, sizeof(uint32_t));

    req.nlmh.nlmsg_len += NLA_ALIGN(nla->nla_len);

    if (send(fd, (void *) &req, req.nlmh.nlmsg_len, 0) == -1) {
        free(buf);
        close(fd);
        err(EXIT_FAILURE, "send(MEMINFO_GENL_CMD_COMMAND)");
    }

    while (recv(fd, buf, BUFLEN, 0) > 0) {
        nlmh = (struct nlmsghdr *) buf;
        nla = GENLMSG_DATA(nlmh);
        while ((int64_t)((char *) nla - (char *) nlmh) < nlmh->nlmsg_len - NLA_HDRLEN) {
            if (nla->nla_type == MEMINFO_GENL_ATTR_DATA) {
                data = *((uint32_t *) NLA_DATA(nla));
                break;
            }

            nla = (struct nlattr *) ((char *) nla + NLA_ALIGN(nla->nla_len));
        }
    }

	printf("%d\n", data);

    free(buf);
    close(fd);
	return EXIT_SUCCESS;
}

