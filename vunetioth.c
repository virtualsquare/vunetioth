#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <vunet.h>
#include <libioth.h>

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
	/* printf("ioth_msocket %d %d\n", rv, errno); */
	if (rv >= 0 && (type & SOCK_NONBLOCK) != 0)
		ioth_fcntl(rv, F_SETFL, O_NONBLOCK);
	return rv;
}

static int vunetioth_ioctl(int fd, unsigned long request, void *addr) {
	if (fd == -1 && addr == NULL) {
		int retval = vunet_ioctl_parms(request);
		if (retval == 0) {
			errno = ENOSYS; return -1;
		} else
			return retval;
	} else {
		int tmpfd, retval;
		switch (request) {
			case FIONREAD:
			case FIONBIO:
				//printf("forward  ioctl\n");
				if (fd == -1)
					return errno = EINVAL, -1;
				else
					return ioth_ioctl(fd, request, addr);
			default:
				//printf("fake netlink socket\n");
				tmpfd = _ioth_socket(AF_NETLINK, SOCK_RAW|SOCK_CLOEXEC, 0);
				if (tmpfd < 0)
					return -1;
				else {
					retval = ioth_ioctl(tmpfd, request, addr);
					ioth_close(tmpfd);
					return retval;
      }
    }
	}
}

static int vunetioth_accept4(int fd, struct sockaddr *addr, socklen_t *addrlen, int flags) {
  return ioth_accept(fd, addr, addrlen);
}

int vunetioth_init(const char *source, unsigned long flags, const char *args, void **private_data) {
  struct ioth *vdestack = ioth_newstacki(source, args);
	/* printf("source %s\nargs %s\n",source,args); */
  if (vdestack != NULL) {
    *private_data = vdestack;
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

