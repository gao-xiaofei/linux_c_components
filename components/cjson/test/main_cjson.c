#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <pthread.h>
#include "cJSON.h"


char* read_file(const char* filename, size_t* file_size);


void *Thread1_Loop()
{
	const char* config_path = "../config.json";
    size_t file_size = 0;
    char* json_content = NULL;
    cJSON* root = NULL;

    for (;;) {
		file_size = 0;
        json_content = NULL;
        root = NULL;

		json_content = read_file(config_path, &file_size);
		if (json_content == NULL) {
			printf("Failed to read file: %s\n", config_path);
			goto LOOP_END;
		}

		root = cJSON_Parse(json_content);
        if (root == NULL) {
            const char* error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL) {
                fprintf(stderr, "JSON parse error: %s\n", error_ptr);
            } else {
                fprintf(stderr, "JSON parse error: unknown error\n");
            }
            goto LOOP_END;
        }


		cJSON* app_name = cJSON_GetObjectItem(root, "app_name");
		if (cJSON_IsString(app_name) && app_name->valuestring != NULL) {
			printf("App Name: %s\n", app_name->valuestring);
		}

		// 嵌套对象字段
		cJSON* server = cJSON_GetObjectItem(root, "server");
		if (cJSON_IsObject(server)) {
			cJSON* ip = cJSON_GetObjectItem(server, "ip");
			cJSON* port = cJSON_GetObjectItem(server, "port");
			if (cJSON_IsString(ip) && ip->valuestring != NULL) {
				printf("Server IP: %s\n", ip->valuestring);
			}
			if (cJSON_IsNumber(port)) {
				printf("Server Port: %d\n", port->valueint);
			}
		}

		cJSON* max_connections = cJSON_GetObjectItem(root, "max_connections");
		if (cJSON_IsNumber(max_connections)) {
			printf("Max Connections: %d\n", max_connections->valueint);
		}
LOOP_END:
        if (root != NULL) {
            cJSON_Delete(root);
            root = NULL;
        }
        if (json_content != NULL) {
            free(json_content);
            json_content = NULL;
        }
        usleep(2*1000*1000);
    }

    cJSON_Delete(root);
    free(json_content);

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

// 读取文件内容到缓冲区
char* read_file(const char* filename, size_t* file_size) 
{
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("Failed to open file");
        return NULL;
    }

    // 获取文件大小
    fseek(fp, 0, SEEK_END);
    *file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // 分配缓冲区（+1 用于存储字符串结束符）
    char* buffer = (char*)malloc(*file_size + 1);
    if (buffer == NULL) {
        perror("Failed to allocate memory");
        fclose(fp);
        return NULL;
    }

    // 读取文件内容
    size_t read_bytes = fread(buffer, 1, *file_size, fp);
    if (read_bytes != *file_size) {
        perror("Failed to read file");
        free(buffer);
        fclose(fp);
        return NULL;
    }
    buffer[*file_size] = '\0'; // 字符串结束符

    fclose(fp);
    return buffer;
}