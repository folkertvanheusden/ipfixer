#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "logging.h"


int create_udp_listen_socket(const int port)
{
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == -1) {
		dolog(ll_error, "create_udp_listen_socket: cannot create socket: %s", strerror(errno));
		return -1;
	}

	dolog(ll_debug, "create_udp_listen_socket: created socket %d", fd);

	struct sockaddr_in bind_i { 0 };

	bind_i.sin_family      = AF_INET;
	bind_i.sin_port        = htons(port);
	bind_i.sin_addr.s_addr = INADDR_ANY;

	int rc = bind(fd, reinterpret_cast<const sockaddr *>(&bind_i), sizeof bind_i);
	if (rc == -1) {
		dolog(ll_error, "create_udp_listen_socket: cannot bind socket %d to port %d: %s", fd, port, strerror(errno));
		close(fd);
		return -1;
	}

        int reuse_addr = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse_addr, sizeof(reuse_addr)) == -1) {
		dolog(ll_error, "create_udp_listen_socket: cannot set socket %d to 're-use address': %s", fd, strerror(errno));
		close(fd);
		return -1;
	}

	return fd;
}

uint16_t get_net_short(const uint8_t *const p)
{
	return (p[0] << 8) | p[1];
}

uint32_t get_net_long(const uint8_t *const p)
{
	return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
}

uint64_t get_net_long_long(const uint8_t *const p)
{
	uint64_t out = 0;

	for(int i=0; i<8; i++) {
		out <<= 8;
		out |= p[i];
	}

	return out;
}
