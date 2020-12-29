/*
 *   VUOS: view OS project
 *   Copyright (C) 2020  Renzo Davoli <renzo@cs.unibo.it>
 *   VirtualSquare team.
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <vunet.h>
#include <ioth.h>
#include <stropt.h>

static int supported_domain (int domain) {
  switch (domain) {
    case AF_INET:
    case AF_INET6:
    case AF_NETLINK:
    case AF_PACKET:
      return 1;
    default:
      return 0;
  }
}

static int supported_ioctl (unsigned long request) {
  return vunet_is_netdev_ioctl(request);
}

static int _ioth_socket(int domain, int type, int protocol) {
  struct ioth *stack = vunet_get_private_data();
	type &= ~SOCK_CLOEXEC;
  int rv = ioth_msocket(stack, domain, type & ~SOCK_NONBLOCK, protocol);
	/* printf("ioth_msocket %p %d %d\n", stack, rv, errno); */
	if (rv >= 0 && (type & SOCK_NONBLOCK) != 0)
		ioth_fcntl(rv, F_SETFL, O_NONBLOCK);
	return rv;
}

static int vunetioth_ioctl (int fd, unsigned long request, void *addr) {
  if (fd == -1) {
    if (addr == NULL) {
      int retval = vunet_ioctl_parms(request);
      if (retval == 0) {
        errno = ENOSYS; return -1;
      } else
        return retval;
    } else {
      int tmpfd = _ioth_socket(AF_INET, SOCK_DGRAM|SOCK_CLOEXEC, 0);
      int retval;
      if (tmpfd < 0)
        return -1;
      else {
        retval = ioth_ioctl(tmpfd, request, addr);
        close(tmpfd);
        return retval;
      }
    }
  } else
    return ioth_ioctl(fd, request, addr);
}


static int vunetioth_accept4(int fd, struct sockaddr *addr, socklen_t *addrlen, int flags) {
  return ioth_accept(fd, addr, addrlen);
}

/* mount -t vunetioth -o "if=vde://" vdestack /dev/net/n1
 *          => ioth_newstackv("vdestack", ["vde//", NULL])
 * ('if=' can be omitted if the option includes ://)
 * mount -t vunetioth -o "vde://" vdestack /dev/net/n1
 *          => ioth_newstackv("vdestack", ["vde//", NULL])
 * mount -t vunetioth -o "vde://,vxvde://" vdestack /dev/net/n2
 *          => ioth_newstackv("vdestack", ["vde//", "vxvde://", NULL])
 * mount -t vunetioth -o "eth0:vde://,eth1:vxvde://,option1" vdestack /dev/net/n3
 *          => ioth_newstackv("vdestack,option1", ["eth0:vde//", "eth1:vxvde://", NULL])
 */
int vunetioth_init(const char *source, unsigned long flags, const char *mntargs, void **private_data) {
  struct ioth *iothstack;
	int tagc;
	if (mntargs == NULL) mntargs = "";
	if ((tagc = stropt(mntargs, NULL, NULL, NULL)) > 0) {
		char buf[strlen(mntargs) + 1];
    char *tags[tagc + 1];
    char *args[tagc + 1];
    char *vnl[tagc];
    char *stack_plus_options;
    int index = 0;
    tags[0] = (char *) source;
    args[0] = NULL;
    stropt(mntargs, tags + 1, args + 1, buf);
    for (int i = 1; i<tagc; i++) {
      if (strstr(tags[i], "://") && args[i] == NULL) {
        vnl[index++] = tags[i];
        tags[i] = STROPTX_DELETED_TAG;
      } else if (strcmp(tags[i],"if") == 0 && args[i] != NULL) {
        vnl[index++] = args[i];
        tags[i] = STROPTX_DELETED_TAG;
      }
    }
    vnl[index] = NULL;
    stack_plus_options = stropt2str(tags, args, ',', '=');
#if 0
		printf("%s\n", stack_plus_options);
		for (int i = 0; vnl[i]; i++)
			printf("%d %s\n", i, vnl[i]);
#endif
		iothstack = ioth_newstackv(stack_plus_options, (const char **) vnl);
		free(stack_plus_options);
	} else
		iothstack = ioth_newstack(source);
  if (iothstack != NULL) {
    *private_data = iothstack;
    return 0;
  } else {
    errno = EINVAL;
    return -1;
  }
}

int vunetioth_fini(void *private_data) {
  ioth_delstack(private_data);
  return 0;
}

struct vunet_operations vunet_ops = {
  .socket = _ioth_socket,
  .bind = ioth_bind,
  .connect = ioth_connect,
  .listen = ioth_listen,
  .accept4 = vunetioth_accept4,
  .getsockname = ioth_getsockname,
  .getpeername = ioth_getpeername,
  .recvmsg = ioth_recvmsg,
  .sendmsg = ioth_sendmsg,
  .getsockopt = ioth_getsockopt,
  .setsockopt = ioth_setsockopt,
  .shutdown = ioth_shutdown,
	.ioctl = vunetioth_ioctl,
  .close = ioth_close,

  .epoll_ctl = epoll_ctl,

  .supported_domain = supported_domain,
  .supported_ioctl = supported_ioctl,

  .init = vunetioth_init,
  .fini = vunetioth_fini,
};

