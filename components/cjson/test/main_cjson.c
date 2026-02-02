#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <pthread.h>
#include "doso_cjson.h"


struct doso_cjson config_data;


void *Thread1_Loop()
{
	const char* config_path = "../test_config.json";
	int ret = doso_cjson_init(&config_data);
    if (ret != 0)
    {
        printf("doso_cjson_init failed!\n");
    }
    
    for (;;) {
		doso_config_parse(config_path, &config_data);
		printf("App Name: %s\n", config_data.app_name);
		printf("gateway_ip: %s\n", config_data.gateway.ip);
		printf("gins_port: %d\n", config_data.gateway.gins_port);
		printf("gins_data_hz: %f\n", config_data.gateway.gins_data_hz);
		printf("thruster_port: %d\n", config_data.gateway.thruster_port);
		printf("thruster_data_hz: %f\n", config_data.gateway.thruster_data_hz);
		printf("battery_port: %d\n", config_data.gateway.battery_port);
		printf("battery_data_hz: %f\n", config_data.gateway.battery_data_hz);
		printf("plc_ip: %s\n", config_data.plc.ip);
		printf("plc_port: %d\n", config_data.plc.port);
		printf("log_level: %d\n", config_data.log_level);
		printf("Max Connections: %d\n", config_data.max_connections);
        usleep(2*1000*1000);
    }
}


void *Thread2_Loop()
{
	for(;;)
	{
		printf("Thread2 running! \n");
		usleep(1*5000*1000);
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

