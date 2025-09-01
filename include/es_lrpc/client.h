#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <chrono>
#include <memory>
#include <thread>
#include "libipc/ipc.h"
#include "es_lrpc/message.h"
#include "es_lrpc/timestamp.h"

namespace es_lrpc
{
template <typename Request, typename Response>
class client
{
public:
    using RequestMessage = message<Request>;
    using ResponseMessage = message<Response>;

public:
    /**
     * @brief 默认构造函数，生成随机的客户端ID，会自动创建一个响应通道，占用id名空间，随着运行会增加
     */
    client(void)
    {
        /**
         * @note 构造函数会自动生成一个唯一的客户端ID，并创建一个响应通道
         * @param timestamp_ns 当前时间戳,增加client_id唯一性
         * @param rand() 随机数,增加client_id唯一性
         * @param server_name_ 请求主题，libipc通过此topic自动路由到对应的es_rcp::server<Request, Response>服务端对象
         * @param client_name_ 响应主题，libipc通过此topic自动路由到当前es_rcp::client<Request, Response>@client_id客户端对象
         */
        uint64_t timestamp_ns = timestamp_ns::now();
        std::string req_name = typeid(Request).name();
        std::string res_name = typeid(Response).name();

        client_name_ = "es_lrpc::client<" + req_name + "," + res_name + ">";
        server_name_ = "es_lrpc::server<" + req_name + "," + res_name + ">";

        client_id_ = hasher_(
            client_name_ +
            std::to_string(reinterpret_cast<uintptr_t>(this)) +
            std::to_string(timestamp_ns) +
            std::to_string(rand())
        );
		client_topic_ = std::to_string(client_id_);

		server_id_ = hasher_(server_name_);
		server_topic_ = std::to_string(server_id_);

        client_name_ += "@" + std::to_string(client_id_);
        response_channel_ = std::make_unique<ipc::channel>(client_topic_.c_str(), ipc::receiver);

        std::cout << "es_lrpc::client listent at " << client_name_ << std::endl;
        std::cout << "es_lrpc::client id " << client_id_ << std::endl;
    }

    /**
     * @brief 带文件名和行号的构造函数，生成固定的客户端ID，对象实例化后，id不会改变，资源占用确定
     */
    client(const std::string &file, int line)
    {
        /**
         * @note 构造函数会自动生成一个唯一的客户端ID，并创建一个响应通道
         * @param timestamp_ns 当前时间戳,增加client_id唯一性
         * @param rand() 随机数,增加client_id唯一性
         * @param server_name_ 请求主题，libipc通过此topic自动路由到对应的es_rcp::server<Request, Response>服务端对象
         * @param client_name_ 响应主题，libipc通过此topic自动路由到当前es_rcp::client<Request, Response>@client_id客户端对象
         */
        uint64_t timestamp_ns = timestamp_ns::now();
        std::string req_name = typeid(Request).name();
        std::string res_name = typeid(Response).name();

        client_name_ = "es_lrpc::client<" + req_name + "," + res_name + ">";
        server_name_ = "es_lrpc::server<" + req_name + "," + res_name + ">";

        client_id_ = hasher_(client_name_ + file + std::to_string(line));
		client_topic_ = std::to_string(client_id_);

		server_id_ = hasher_(server_name_);
		server_topic_ = std::to_string(server_id_);

        client_name_ += "@" + std::to_string(client_id_);
        response_channel_ = std::make_unique<ipc::channel>(client_topic_.c_str(), ipc::receiver);

        std::cout << "es_lrpc::client listent at " << client_name_ << std::endl;
        std::cout << "es_lrpc::client id " << client_id_ << std::endl;
    }

    /**
     * @brief 设置默认响应处理函数
     * @param handler 响应处理函数
     */
    void set_default_response_handler(std::function<void(const Response &)> handler)
    {
        default_response_handler_ = handler;
    }

    /**
     * @brief 发送请求
     * @param request 请求参数
     * @return 发送成功返回true，否则返回false
     */
    bool send_request(const Request &request)
    {
        ipc::channel reqeust_channel(server_topic_.c_str(), ipc::sender);
        RequestMessage msg(client_id_, timestamp_ns::now(),  request);

        std::cout 
            << client_name_ << " send request: " << std::endl
            << msg.to_string() << std::endl
            << " message size: " << msg.size() << std::endl
            ;

        bool send_result = reqeust_channel.send(msg.data(), msg.size());
        if (!send_result)
        {
            std::cerr  << " send request failed!" << std::endl;
            return false;
        }

        std::cout << " send request success!" << std::endl;
        return true;
    }

    /**
     * @brief 接收响应
     * @param handler 响应处理函数
     * @param timeout 接收超时时间单位ms
     * @return 接收结果, true表示成功, false表示失败
     */
    bool recv_response(std::function<void(const Response &)> handler = nullptr, uint64_t timeout = ipc::default_timeout)
    {
        auto buf = response_channel_->recv(timeout);
        if (buf.empty()) {
            std::cerr << client_name_ << " recv response timeout!" << std::endl;
            return false;
        }
        
        const ResponseMessage msg(buf.data(), buf.size());
        auto response_ptr = msg.get_message();
        if (nullptr == response_ptr) {
            std::cerr << client_name_ << "recv response failed!"  << std::endl;
            return false;
        }

        std::cout 
            << client_name_ << " recv response success! " << std::endl
            << msg.to_string() << std::endl
            ;

        if (handler) {
            handler(*response_ptr);
            return true;
        }
        else if (default_response_handler_) {
            default_response_handler_(*response_ptr);
            return true;
        }

        return false;
    }

    const std::string& get_client_name(void) const 
    {
        return client_name_;
    }

    const std::string& get_server_name(void) const
    {
        return server_name_;
    }

    uint64_t get_id(void) const 
    {
        return client_id_;
    }

private:
    uint64_t client_id_;
    std::string client_name_;
    std::string client_topic_;

	uint64_t server_id_;
    std::string server_name_;
    std::string server_topic_;

	std::hash<std::string> hasher_;
    std::unique_ptr<ipc::channel> response_channel_;
    std::function<void(const Response &)> default_response_handler_;
};
} // namespace es_lrpc
