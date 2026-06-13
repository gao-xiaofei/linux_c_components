#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <pthread.h>
#include "doso_udp_user.h"

struct DosoUdpUser udp_user = {
	.enable_ip_filter = 0,
	.receive_timeout_ms = 1500
};

void *Thread1_Loop()
{
	int udp_state = 0;
    udp_state = doso_udp_user_init(&udp_user, "127.0.0.1", 10001);
    for (;;) {
		udp_state = doso_udp_user_send(&udp_user, "127.0.0.1", 10000, (uint8_t*)"hello", 5);
		if(udp_state != 0){
			doso_udp_user_init(&udp_user, "127.0.0.1", 10001);
		}
        usleep(2*1000*1000);
    }
}


void *Thread2_Loop()
{
	uint8_t rx_buffer[1024] = {0};
	uint16_t rx_size = 0;
	int udp_state = 0;
	usleep(2*1000*1000);
	for(;;)
	{	
		if(udp_user.fd < 0){
			printf("udp fd is not valid\n");
			usleep(1*1000*1000);
			continue;
		}

		udp_state = doso_udp_user_receive(&udp_user, rx_buffer, 1024, &rx_size);
		if(udp_state != 0){
			memset(rx_buffer, 0, sizeof(rx_buffer));
		}
		printf("udp state %d Q- receive size %d - %s\n", udp_state, rx_size, rx_buffer);

	}
}


int main()
{

	printf("test start ...\n");

	struct sched_param param;
	pthread_attr_t tattr;
	/* initialized with default attributes */
  	pthread_attr_init(&tattr);
  	/* safe to get existing scheduling param */
  	pthread_attr_getschedparam(&tattr, &param);
  	/* set the priority; others are unchanged */
  	param.sched_priority = sched_get_priority_max(SCHED_FIFO);
  	/* setting the new scheduling param */
  	pthread_attr_setschedparam(&tattr, &param);
	
	pthread_t Thread1_ID;
	pthread_t Thread2_ID;
  	
	pthread_create(&Thread1_ID, &tattr, &Thread1_Loop, NULL);
 	pthread_create(&Thread2_ID, &tattr, &Thread2_Loop, NULL);

	pthread_join(Thread1_ID, NULL);
	pthread_join(Thread2_ID, NULL);
	return 0;
}

