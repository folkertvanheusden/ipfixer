#include <errno.h>
#include <netdb.h>
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

void put_net_long(uint8_t *const p, const uint32_t v)
{
	p[0] = v >> 24;
	p[1] = v >> 16;
	p[2] = v >>  8;
	p[3] = v;
}

void put_net_long_long(uint8_t *const p, const uint64_t v)
{
	p[0] = v >> 56;
	p[1] = v >> 48;
	p[2] = v >> 40;
	p[3] = v >> 32;
	p[4] = v >> 24;
	p[5] = v >> 16;
	p[6] = v >>  8;
	p[7] = v;
}

int connect_to(const std::string & host, const int portnr)
{
	struct addrinfo hints = { 0 };
	hints.ai_family    = AF_UNSPEC;   // Allow IPv4 or IPv6
	hints.ai_socktype  = SOCK_STREAM;
	hints.ai_flags     = AI_PASSIVE;  // For wildcard IP address
	hints.ai_protocol  = 0;	          // Any protocol
	hints.ai_canonname = nullptr;
	hints.ai_addr      = nullptr;
	hints.ai_next      = nullptr;

	char portnr_str[8] = { 0 };
	snprintf(portnr_str, sizeof portnr_str, "%d", portnr);

	struct addrinfo *result = nullptr;
	int rc = getaddrinfo(host.c_str(), portnr_str, &hints, &result);
	if (rc != 0) {
		dolog(ll_warning, "connect_to: problem resolving \"%s\": %s", host.c_str(), gai_strerror(rc));

		return false;
	}

	for(struct addrinfo *rp = result; rp != nullptr; rp = rp->ai_next) {
		int fd = socket(rp -> ai_family, rp -> ai_socktype, rp -> ai_protocol);
		if (fd == -1)
			continue;  // TODO: log message?

		if (connect(fd, rp -> ai_addr, rp -> ai_addrlen) == 0) {
			freeaddrinfo(result);

			return fd;
		}

		close(fd);
	}

	freeaddrinfo(result);

	return -1;
}

ssize_t WRITE(int fd, const uint8_t *whereto, size_t len)
{
	ssize_t cnt=0;

	while(len > 0) {
		ssize_t rc = write(fd, whereto, len);

		if (rc == -1) {
			if (errno == EAGAIN) {
				dolog(ll_warning, "WRITE: %s", strerror(errno));
				continue;
			}

			return -1;
		}
		else if (rc == 0)
			return -1;
		else {
			whereto += rc;
			len -= rc;
			cnt += rc;
		}
	}

	return cnt;
}
