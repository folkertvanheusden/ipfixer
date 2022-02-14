#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "ipfix.h"
#include "logging.h"
#include "net.h"


int main(int argc, char *argv[])
{
	setlog("test.log", ll_debug, ll_debug);

	int fd = create_udp_listen_socket(9995);

	struct pollfd fds[] { { fd, POLLIN, 0 } };

	for(;;) {
		int rcp = poll(fds, 1, 500);

		if (rcp == 0)  // timeout
			continue;

		if (rcp == -1) {
			dolog(ll_error, "main: problem polling for UDP packets: %s", strerror(errno));
			break;
		}

		uint8_t buffer[65527] { 0 };
		int rrc = recv(fd, buffer, sizeof buffer, 0);

		if (rrc == -1) {
			dolog(ll_error, "main: problem receiving UDP packet: %s", strerror(errno));
			continue;
		}

		if (process_ipfix_packet(buffer, rrc) == false) {
			dolog(ll_error, "main: problem processing ipfix packet");
			break;
		}
	}

	close(fd);

	return 0;
}
