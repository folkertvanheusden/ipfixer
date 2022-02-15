#include <atomic>
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "db.h"
#include "db-mongodb.h"
#include "ipfix.h"
#include "logging.h"
#include "net.h"


std::atomic_bool stop_flag { false };

void sigh(int sig)
{
	stop_flag = true;

	dolog(ll_info, "Terminating...");
}

int main(int argc, char *argv[])
{
	setlog("test.log", ll_debug, ll_debug);

	signal(SIGINT, sigh);

	ipfix i;

	db *db = new db_mongodb("mongodb://localhost:27017", "testdb", "testcol");

	int fd = create_udp_listen_socket(9995);

	struct pollfd fds[] { { fd, POLLIN, 0 } };

	while(!stop_flag) {
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

		try {
			if (i.process_packet(buffer, rrc, db) == false)
				dolog(ll_error, "main: problem processing ipfix packet");
		}
		catch(const std::out_of_range & e) {
			dolog(ll_error, "main: 'out of range' exception (%s); internal error", e.what());
			break;
		}
	}

	close(fd);

	delete db;

	return 0;
}
