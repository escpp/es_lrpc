# es_lrpc - 基于libipc的RPC通信框架
- 基于libipc实现类似rpc功能的问答模式通信框架；
- protobuf定义message Request{...} 和 message Response{...} 请求回复消息对；
- client.h定义客户端类，es_lrpc::client<Request, Response>, 发送Request，接收Response;
- server.h定义服务端类, es_lrpc::server<Request, Response>, 监听Request通道，处理Request，返回Response;

## 概述
es_lrpc是一个基于libipc实现的问答模式RPC通信框架，提供简单的客户端-服务端通信机制。

## 主要特性
- 基于protobuf的消息序列化
- 基于模板实例化自动消息路由
- 实现同步请求/响应处理
- libipc库处理线程安全问题

## 构建与安装

### 依赖
- libipc库 https://github.com/escpp/libipc
- protobuf序列化库 https://github.com/protocolbuffers/protobuf

### 构建步骤
```bash
# 编译x86平台库
make x86

# 编译mp135平台
make mp135
```

### 安装
```bash
make install
```

## 开发流程
- 定义protobuf消息格式，生成消息头和消息序列化结构体;
- 定义es_lrpc::client<Request, Response>和es_lrpc::server<Request, Response>类，实现消息处理函数;
- 实现消息处理函数，处理Request，返回Response;
- 启动服务端，监听Request通道;
- 启动客户端，发送Request，接收Response;

## 请求端示例
```cpp
#include "message.bp.h"
#include "es_lrpc/client.h"

using namespace es_config;

int main() 
{ 
    enp_rpc::client<ApConfig, ApConfig> client;
    const uint64_t timeout_ms = 5000;
    ApConfig request;

    client.send_request(request);
    client.recv_response([](const ApConfig& response) {
        // 处理response
    }, timeout_ms);

    return 0;
}
```

## 服务端示例
```cpp
#include "message.bp.h"
#include "es_lrpc/server.h"

using namespace es_config;

int main() 
{ 
    es_lrpc::server<ApConfig, ApConfig> rpc_server;
    
    while (true) {
        rpc_server.recv_request([&](const es_lrpc::server<ApConfig, ApConfig>::RequestMessage& request) {
            std::cout 
                << rpc_server.get_server_name() << " received request!" << std::endl
                << "from client: " << rpc_server.get_client_name() << "@" << request.get_client_id() << std::endl
                << request.to_string()
                ;

            ApConfig response;
            response.set_enable(true);
            response.set_ssid("es_lrpc");
            response.set_password("1234567890");
            response.set_encryption(ApConfig::WPA2);
            response.set_protocol(ApConfig::B_G);
            
            bool send_result = rpc_server.send_response(request, response);
            if (!send_result) {
                std::cout << "Failed to send response" << std::endl;
                return;
            }
        });
    }
    
    return 0;
}
```