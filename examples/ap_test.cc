#include <iostream>
#include <fstream>
#include "es_config.pb.h"

using namespace es_config;

int main()
{
    // 创建ApConfig对象并设置值
    Configuration config;
    ApConfig *ap = config.mutable_ap();
    ap->set_ssid("EN-Test-AP");
    ap->set_password("password123");
    ap->set_encryption(ApConfig::WPA2);
    ap->set_protocol(ApConfig::B_G_N);
    ap->set_enable(true);

    std::cerr << "ap->ByteSize() = " << ap->ByteSizeLong() << std::endl;
    std::string ap_buf = ap->SerializeAsString();
    std::string ap_arr(ap->ByteSizeLong(), 0);
    ap->SerializeToArray(&ap_arr[0], ap->ByteSizeLong());
    if (ap_buf.size() > 0)
    {
        std::cerr << "ap_buf: " << ap_buf.size() << " bytes" << std::endl;
    }

    // 序列化到文件
    std::ofstream out("ap_config.bin", std::ios::binary);
    if (!config.SerializeToOstream(&out))
    {
        std::cerr << "Failed to write config to file." << std::endl;
        return -1;
    }
    out.close();

    // 从文件反序列化
    Configuration new_config;
    std::ifstream in("ap_config.bin", std::ios::binary);
    if (!new_config.ParseFromIstream(&in))
    {
        std::cerr << "Failed to parse config from file." << std::endl;
        return -1;
    }
    in.close();
    new_config.ParseFromArray(config.SerializeAsString().c_str(), config.ByteSize());

    // 验证数据
    const ApConfig &new_ap = new_config.ap();
    std::cout << "Deserialized AP Config:" << std::endl;
    std::cout << "SSID: " << new_ap.ssid() << std::endl;
    std::cout << "Password: " << new_ap.password() << std::endl;
    std::cout << "Encryption: " << new_ap.encryption() << std::endl;
    std::cout << "Protocol: " << new_ap.protocol() << std::endl;
    std::cout << "Enabled: " << (new_ap.enable() ? "true" : "false") << std::endl;

    return 0;
}
