#include <iostream>
#include <thread>
#include "es_lrpc/message.h"
#include "es_lrpc/client.h"
#include "message.pb.h"

using namespace es_config;

int test_people(uint64_t interval_us=1000000, uint32_t number = -1) 
{
    using RpcClient_ApConfig = es_lrpc::client<ApConfig, ApConfig>;
    RpcClient_ApConfig rpc_client(__FILE__, __LINE__);


    for (int i = 0; i < number; i++) {
        ApConfig request;
        request.set_ssid("ssid");
        request.set_password("password");
        request.set_encryption(ApConfig::WPA2);
        request.set_protocol(ApConfig::B_G_N);
        request.set_enable(true);

        bool send_result = rpc_client.send_request(request);
        if (!send_result) {
            std::cout << "Failed to send request" << std::endl;
            return -1;
        }

        rpc_client.recv_response([&](const ApConfig& resp) {
            std::cout 
                << resp.ByteSizeLong()
                << rpc_client.get_client_name() << " received response success!" << std::endl
                ;
        });

        std::this_thread::sleep_for(std::chrono::microseconds(interval_us));
    }

    return 0;
}

int main(int argc, char** argv) 
{
    uint64_t interval = argc > 1 ? std::stoi(argv[1]) : 1000;
    uint32_t number = argc > 2 ? std::stoull(argv[2]) : -1;
    test_people(interval, number);
    return 0;
}
