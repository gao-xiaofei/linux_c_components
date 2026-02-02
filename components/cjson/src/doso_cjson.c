
#include "doso_cjson.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>



static char* read_file(const char* filename, size_t* file_size) 
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

int doso_cjson_init(struct doso_cjson *cjson)
{
    if (cjson == NULL)
    {
        printf("Error: doso_cjson_init input pointer is NULL!\n");
        return -1;
    }

    memset(cjson->app_name, 0, sizeof(cjson->app_name));  
    strncpy(cjson->app_name, DEFAULT_APP_NAME, sizeof(cjson->app_name) - 1); // 留结束符
    cjson->version[0] = DEFAULT_VERSION_MAJOR;
    cjson->version[1] = DEFAULT_VERSION_MINOR;
    cjson->version[2] = DEFAULT_VERSION_PATCH;

    struct gateway *srv = &cjson->gateway; 
    memset(srv->ip, 0, sizeof(srv->ip));
    strncpy(srv->ip, DEFAULT_GATEWAY_IP, sizeof(srv->ip) - 1);
    srv->gins_port = DEFAULT_GINS_PORT;
    srv->gins_data_hz = DEFAULT_GINS_HZ;
    srv->thruster_port = DEFAULT_THRUSTER_PORT;
    srv->thruster_data_hz = DEFAULT_THRUSTER_HZ;
    srv->battery_port = DEFAULT_BATTERY_PORT;
    srv->battery_data_hz = DEFAULT_BATTERY_HZ;

    struct plc *plc_srv = &cjson->plc; 
    memset(plc_srv->ip, 0, sizeof(plc_srv->ip));
    strncpy(plc_srv->ip, DEFAULT_PLC_IP, sizeof(plc_srv->ip) - 1);
    plc_srv->port = DEFAULT_PLC_PORT;

    cjson->log_level = DEFAULT_LOG_LEVEL;
    cjson->max_connections = DEFAULT_MAX_CONN;

    return 0;
}

void doso_config_parse(const char* config_path, struct doso_cjson * config)
{
    size_t file_size = 0;
    char* json_content = NULL;
    cJSON* root = NULL;

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
        strcpy(config->app_name, app_name->valuestring);
    }

    // 嵌套对象字段
    cJSON* gateway = cJSON_GetObjectItem(root, "gateway");
    if (cJSON_IsObject(gateway)) {
        cJSON* gateway_ip = cJSON_GetObjectItem(gateway, "ip");
        cJSON* gins_port = cJSON_GetObjectItem(gateway, "gins_port");
        cJSON* gins_data_hz = cJSON_GetObjectItem(gateway, "gins_data_hz");
        cJSON* thruster_port = cJSON_GetObjectItem(gateway, "thruster_port");
        cJSON* thruster_data_hz = cJSON_GetObjectItem(gateway, "thruster_data_hz");
        cJSON* battery_port = cJSON_GetObjectItem(gateway, "battery_port");
        cJSON* battery_data_hz = cJSON_GetObjectItem(gateway, "battery_data_hz");

        if (cJSON_IsString(gateway_ip) && gateway_ip->valuestring != NULL) {
            strcpy(config->gateway.ip, gateway_ip->valuestring);
        }
        if (cJSON_IsNumber(gins_port)) {
            config->gateway.gins_port = gins_port->valueint;
        }
        if (cJSON_IsNumber(gins_data_hz)) {
            config->gateway.gins_data_hz = gins_data_hz->valuedouble;
        }
        if (cJSON_IsNumber(thruster_port)) {
            config->gateway.thruster_port = thruster_port->valueint;
        }
        if (cJSON_IsNumber(thruster_data_hz)) {
            config->gateway.thruster_data_hz = thruster_data_hz->valuedouble;
        }
        if (cJSON_IsNumber(battery_port)) {
            config->gateway.battery_port = battery_port->valueint;
        }
        if (cJSON_IsNumber(battery_data_hz)) {
            config->gateway.battery_data_hz = battery_data_hz->valuedouble;
        }
    }
    cJSON* plc = cJSON_GetObjectItem(root, "plc");
    if (cJSON_IsObject(gateway)) {
        cJSON* _ip = cJSON_GetObjectItem(plc, "ip");
        cJSON* _port = cJSON_GetObjectItem(plc, "port");
        if (cJSON_IsString(_ip) && _ip->valuestring != NULL) {
            strcpy(config->plc.ip, _ip->valuestring);
        }
        if (cJSON_IsNumber(_port)) {
            config->plc.port = _port->valueint;
        }
    }
    cJSON* log_level = cJSON_GetObjectItem(root, "log_level");
    if (cJSON_IsNumber(log_level)) {
        config->log_level = log_level->valueint;
    }
    cJSON* max_connections = cJSON_GetObjectItem(root, "max_connections");
    if (cJSON_IsNumber(max_connections)) {
        config->max_connections = max_connections->valueint;
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


}
