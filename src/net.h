#include <stdint.h>


int create_udp_listen_socket(const int port);

uint16_t get_net_short(const uint8_t *const p);
uint32_t get_net_long(const uint8_t *const p);
uint64_t get_net_long_long(const uint8_t *const p);

void put_net_long(uint8_t *const p, const uint32_t v);
void put_net_long_long(uint8_t *const p, const uint64_t v);

int connect_to(const std::string & host, const int portnr);

ssize_t WRITE(int fd, const uint8_t *whereto, size_t len);
