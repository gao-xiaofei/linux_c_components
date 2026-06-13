#ifndef _DOSO_UDP_USER_H_
#define _DOSO_UDP_USER_H_

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

enum {
    DOSO_UDP_USER_OK = 0,
    DOSO_UDP_CREATE_ERR = 1,
    DOSO_UDP_BIND_ERR = 2,
    DOSO_UDP_RECV_ERR = 3,
    DOSO_UDP_RECV_TIMEOUT = 4,
    DOSO_UDP_SET_TIMEOUT_ERR = 5,
    DOSO_UDP_SEND_ERR = 6
};


struct DosoUdpUser {
    // 
    int fd;
    struct sockaddr_in listen_addr;
    struct sockaddr_in source_addr;
    // config set
    uint8_t enable_ip_filter;
    uint16_t receive_timeout_ms;

};




int doso_udp_user_init(struct DosoUdpUser *udp_user_, const char *_listen_ip,const uint16_t _listen_port);
int doso_udp_user_send(struct DosoUdpUser *udp_user_, const char *_send_ip, const uint16_t _send_port, uint8_t *output_buf, uint16_t output_len);
int doso_udp_user_receive(struct DosoUdpUser *udp_user_, uint8_t *rx_buffer, uint16_t rx_size_max, uint16_t *rx_buffer_len);





#endif
