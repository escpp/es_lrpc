#include <iostream>
#include <thread>
#include <chrono>
#include "es_lrpc/server.h"
#include "message.pb.h"

using namespace es_config;

int test_people() {
    using ApConfig_RpcServer = es_lrpc::server<ApConfig, ApConfig>;
    ApConfig_RpcServer rpc_server;
    
    while (true) {
        rpc_server.recv_request([&](const ApConfig_RpcServer::RequestMessage& request) {
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


int main() 
{
    test_people();
    return 0;
}