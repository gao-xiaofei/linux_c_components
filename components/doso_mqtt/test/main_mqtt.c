#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "MQTTClient.h"


// gcc doso_mqtt.c -o demo_test  -lpthread -lpaho-mqtt3c


#define ADDRESS     "tcp://broker.emqx.io:1883"
#define USERNAME    "emqx"
#define PASSWORD    "public"
#define CLIENTID    "c-client"
#define QOS         0
#define TOPIC       "emqx/c-test"
#define TIMEOUT     10000L


void publish(MQTTClient client, char *topic, char *payload) {
    MQTTClient_message message = MQTTClient_message_initializer;
    message.payload = payload;
    message.payloadlen = strlen(payload);
    message.qos = QOS;
    message.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &message, &token);
    MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Send `%s` to topic `%s` \n", payload, TOPIC);
}

int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char *payload = message->payload;
    printf("Received `%s` from `%s` topic \n", payload, topicName);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}


void *Thread1_Loop()
{

    int rc;
    MQTTClient client;

    MQTTClient_create(&client, ADDRESS, CLIENTID, 0, NULL);
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.username = USERNAME;
    conn_opts.password = PASSWORD;



    MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    } else {
        printf("Connected to MQTT Broker!\n");
    }
    // subscribe topic
    MQTTClient_subscribe(client, TOPIC, QOS);
    
    char payload[16];
    int i = 0;

    for (;;) {
        // publish message to broker
        snprintf(payload, 16, "message-%d", i++);
        publish(client, TOPIC, payload);
        usleep(1*100*1000);
    }
    MQTTClient_unsubscribe(client, TOPIC);
    MQTTClient_disconnect(client, TIMEOUT);
    MQTTClient_destroy(&client);
    return rc;

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
