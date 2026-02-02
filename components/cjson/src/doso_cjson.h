#ifndef _DOSO_CJSON_H_
#define _DOSO_CJSON_H_

#include "cJSON.h"

#define DEFAULT_APP_NAME        "doso_cjson_app"
#define DEFAULT_VERSION_MAJOR   1       // 主版本号
#define DEFAULT_VERSION_MINOR   0       // 次版本号
#define DEFAULT_VERSION_PATCH   0       // 修订号
#define DEFAULT_GATEWAY_IP      "192.168.1.1"
#define DEFAULT_GINS_PORT       8001
#define DEFAULT_GINS_HZ         100.0f 
#define DEFAULT_THRUSTER_PORT   8002
#define DEFAULT_THRUSTER_HZ     50.0f
#define DEFAULT_BATTERY_PORT    8003
#define DEFAULT_BATTERY_HZ      10.0f    
#define DEFAULT_PLC_IP          "192.168.1.2"
#define DEFAULT_PLC_PORT        500
#define DEFAULT_LOG_LEVEL       1
#define DEFAULT_MAX_CONN        10       

struct gateway{
    char ip[16];        // 网关IP，IPv4最多15个字符+1个结束符
    int gins_port;              // gins端口
    float gins_data_hz;         // gins数据频率
    int thruster_port;          // 推进器端口
    float thruster_data_hz;     // 推进器数据频率
    int battery_port;           // 电池端口
    float battery_data_hz;      // 电池数据频率
};
struct plc{
    char ip[16]; 
    int port;              
};

struct doso_cjson{
    char app_name[32];          // 应用名称，预留足够长度
    int version[3];             // 版本号，如"1.0.0"格式
    struct gateway gateway;        // 嵌套服务器子配置
    struct plc plc;
    int log_level;              // 日志级别，对应宏定义
    int max_connections;        // 最大连接数
};

int doso_cjson_init(struct doso_cjson *cjson);
void doso_config_parse(const char* config_path, struct doso_cjson * config);

#endif // _DOSO_CJSON_H_