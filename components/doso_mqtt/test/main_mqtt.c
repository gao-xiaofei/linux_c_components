#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include "MQTTClient.h"


// gcc doso_mqtt.c -o demo_test  -lpthread -lpaho-mqtt3c


#define ADDRESS             "tcp://broker.emqx.io:1883"
#define USERNAME            "emqx"
#define PASSWORD            "public"
#define CLIENTID            "c-client"
#define QOS                 0
#define TOPIC               "topic/c-test"
#define TIMEOUT             10000L
#define RETRY_DELAY_MS      2000

#define SUB_TOPIC_1 "topic/c-test"
#define SUB_TOPIC_2 "topic/c-test2"

static int8_t is_alive = false;
int retry_count = 0;

int doso_sync_mqtt_subscribe(MQTTClient client)
{
    int rc = MQTTClient_subscribe(client, SUB_TOPIC_1, QOS);
    if(rc != MQTTCLIENT_SUCCESS){
        printf("MQTTClient_subscribe failed: %d\n", rc);
    }
    rc = MQTTClient_subscribe(client, SUB_TOPIC_2, QOS);
    if(rc != MQTTCLIENT_SUCCESS){
        printf("MQTTClient_subscribe failed: %d\n", rc);
    }
    return rc;
}

int doso_sync_mqtt_unsubscribe(MQTTClient client)
{
    int rc = MQTTClient_unsubscribe(client, SUB_TOPIC_1);
    if(rc != MQTTCLIENT_SUCCESS){
        printf("MQTTClient_unsubscribe failed: %d\n", rc);
    }
    rc = MQTTClient_unsubscribe(client, SUB_TOPIC_2);
    if(rc != MQTTCLIENT_SUCCESS){
        printf("MQTTClient_unsubscribe failed: %d\n", rc);
    }
}

int doso_sync_mqtt_publish(MQTTClient client, char *topic, char *payload, uint16_t payload_len) 
{
    int rc = 0;
    MQTTClient_message message = MQTTClient_message_initializer;
    message.payload = payload;
    message.payloadlen = payload_len;
    message.qos = QOS;
    message.retained = 0;
    MQTTClient_deliveryToken token;
    rc = MQTTClient_publishMessage(client, topic, &message, &token);
    if(rc != MQTTCLIENT_SUCCESS){
        printf("MQTTClient_publishMessage failed: %d\n", rc);
        return rc;
    }
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    if(rc != MQTTCLIENT_SUCCESS){
        printf("MQTTClient_waitForClmpletion failed: %d\n", rc);
    }
    return rc;
}

void connect_lost(void* context, char* cause) {

    MQTTClient client = *(MQTTClient*)context;
    
    printf("=== 连接丢失！原因：%s ===\n", (cause != NULL) ? cause : "未指定");
    doso_sync_mqtt_unsubscribe(client);
    MQTTClient_disconnect(client, TIMEOUT);
    MQTTClient_destroy(&client);
    is_alive = false;
    
}

int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) 
{
    (void)topicLen;
    (void)context;
    // MQTTClient client = *(MQTTClient*)context;
    char *payload = message->payload;
    printf("Received `%s` from `%s` topic \n", payload, topicName);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}


void *Thread1_Loop()
{

    int rc;
    char payload[32];
    int i = 0;
    MQTTClient client;

    for (;;) {
     
        if(is_alive){
            // publish message to broker
            snprintf(payload, 32, "message-%d", i++);
            doso_sync_mqtt_publish(client, TOPIC, payload, 32);
        }else{
            printf("尝试连接mqtt服务器（第%d次）...\n", retry_count + 1);

            MQTTClient_create(&client, ADDRESS, CLIENTID, 0, NULL);
            MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
            conn_opts.username = USERNAME;
            conn_opts.password = PASSWORD;
            MQTTClient_setCallbacks(client, &client, connect_lost, on_message, NULL);
            int connect_rc = MQTTClient_connect(client, &conn_opts);
            if (connect_rc == MQTTCLIENT_SUCCESS) {
                printf("连接mqtt服务器成功！\n");
                doso_sync_mqtt_subscribe(client);
                is_alive = true;
                return;  // 重连成功，退出回调函数
            } else {
                printf("连接mqtt服务器失败，错误码：%d\n", connect_rc);
                
                MQTTClient_disconnect(client, TIMEOUT);
                MQTTClient_destroy(&client);
                is_alive = false;
                retry_count++;
                usleep(RETRY_DELAY_MS*1000);  
            }
        }

        usleep(1*100*1000);
    }

    MQTTClient_unsubscribe(client, TOPIC);
    MQTTClient_disconnect(client, TIMEOUT);
    MQTTClient_destroy(&client);

}


void *Thread2_Loop()
{
	for(;;)
	{
		printf("Thread1 running! \n");
		usleep(1000000);
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
