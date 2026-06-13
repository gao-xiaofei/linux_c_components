#include "doso_udp_user.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h> 




int doso_udp_user_init(struct DosoUdpUser *udp_user_, const char *_listen_ip,const uint16_t _listen_port)
{
    int ret = DOSO_UDP_USER_OK;
    udp_user_->fd = -1;
    udp_user_->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (udp_user_->fd < 0) {
        ret = DOSO_UDP_CREATE_ERR;
        printf("Unable to create socket: errno %d\n", errno);
    }
    // 设置超时
    struct timeval tv;
    tv.tv_sec = udp_user_->receive_timeout_ms / 1000;  // 秒
    tv.tv_usec = (udp_user_->receive_timeout_ms % 1000) * 1000; // 微秒
    if (setsockopt(udp_user_->fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        printf("Failed to set socket timeout\n");
    }
    if(_listen_port != 0){
        memset(&udp_user_->listen_addr, 0, sizeof(udp_user_->listen_addr));
        udp_user_->listen_addr.sin_family = AF_INET;
        udp_user_->listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        udp_user_->listen_addr.sin_port = htons(_listen_port);

        if(udp_user_->enable_ip_filter){
            udp_user_->listen_addr.sin_addr.s_addr = inet_addr(_listen_ip);
        }

        if (bind(udp_user_->fd, (struct sockaddr *)&udp_user_->listen_addr, sizeof(udp_user_->listen_addr)) < 0) {
            printf("Socket bind failed: errno %d\n", errno);
            close(udp_user_->fd);
            udp_user_->fd = -1;
            ret = DOSO_UDP_BIND_ERR;
        }

        printf("Socket created successfully, listening on port %d\n", _listen_port);
    }
    return ret;
}

int doso_udp_user_send(struct DosoUdpUser *udp_user_, const char *_send_ip, const uint16_t _send_port, uint8_t *output_buf, uint16_t output_len)
{
    int ret = DOSO_UDP_USER_OK;
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(_send_ip);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(_send_port);

    if(udp_user_->fd >= 0)
    {
        int err = sendto(udp_user_->fd, output_buf, output_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err < 0) {
            ret = DOSO_UDP_SEND_ERR;
            printf( "Error occurred during sending: errno %d\n", errno);
        }
    }else{
        ret = DOSO_UDP_CREATE_ERR;
    }
    return ret;
}


int doso_udp_user_receive(struct DosoUdpUser *udp_user_, uint8_t *rx_buffer, uint16_t rx_size_max, uint16_t *rx_buffer_len)
{
    int ret = 0;
    socklen_t socklen = sizeof(udp_user_->source_addr);
    ssize_t len = recvfrom(udp_user_->fd, rx_buffer, rx_size_max, 0, (struct sockaddr *)&udp_user_->source_addr, &socklen);
    if (len < 0) {
        *rx_buffer_len = 0;
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            ret = DOSO_UDP_RECV_TIMEOUT;
        }else if(errno == EINTR){
            // TODO: 信号中断
            printf("recvfrom EINTR failed: errno %d\n", errno);
        }else{
            printf("recvfrom failed: errno %d, rebuilding socket\n", errno);
            close(udp_user_->fd);
            udp_user_->fd = -1;
            ret = DOSO_UDP_RECV_ERR;
        }
    }else{
        *rx_buffer_len = len;
    }
    return ret;
}
