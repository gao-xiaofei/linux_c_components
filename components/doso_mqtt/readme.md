### 编译 Paho MQTT 库

``` bash
git clone https://github.com/eclipse/paho.mqtt.c.git
cd paho.mqtt.c
# 如果遇到fatal error: openssl/ssl.h: No such file or directory
sudo apt install -y libssl-dev
make
sudo make install
```

### 编译 doso_mqtt测试程序

``` bash
cd components/doso_mqtt/test
mkdir build && cd build
cmake ..
make
```

### 测试程序

``` bash
./doso_mqtt
```