#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <unordered_map>
#include <memory>
#include <thread>
#include "libipc/ipc.h"
#include "es_lrpc/message.h"

namespace es_lrpc
{

    template <typename Request, typename Response>
    class server
    {
    public:
        using RequestMessage = message<Request>;
        using ResponseMessage = message<Response>;

    public:
        server(void)
        {
            std::string req_name = typeid(Request).name();
            std::string res_name = typeid(Response).name();

            client_name_ = "es_lrpc::client<" + req_name + "," + res_name + ">";
            server_name_ = "es_lrpc::server<" + req_name + "," + res_name + ">";
			server_id_ = hasher_(server_name_);
			server_topic_ = std::to_string(server_id_);
            listes_channel_ = std::make_shared<ipc::channel>(server_topic_.c_str(), ipc::receiver);
            std::cout << server_name_ << "@" << server_id_ << std::endl;
        }

        bool recv_request(std::function<void(const RequestMessage &)> handler = nullptr, uint64_t timeout = ipc::invalid_value)
        {
            auto buf = listes_channel_->recv(timeout);
            if (buf.size() < RequestMessage::header_size) {
                std::cout << server_name_ << "invalid message length: " << buf.size() << "/ " << RequestMessage::header_size << std::endl;
                return false;
            }

            const RequestMessage msg(buf.data(), buf.size());
            if (msg.size() == 0) {
                std::cout << server_name_ << " msg is null" << std::endl;
                return false;
            }

            std::cout 
                << server_name_ <<  " recv: " << std::endl
                << msg.to_string() << std::endl
                ;

            if (handler) {
                handler(msg);
                return true;
            }

            if (request_handler_) {
                request_handler_(msg);
                return true;
            }

            std::cout << server_name_ << " no request handler!" << std::endl;
            return false;
        }

        bool send_response(const RequestMessage &request, const Response &response)
        {
            ResponseMessage msg(request.get_client_id(), request.get_message_id(), response);

            std::string response_topic = std::to_string(request.get_client_id());
            ipc::channel responser_sender(response_topic.c_str());

            std::cout 
                << server_name_ << " send response:" << std::endl
                << "to client: " << response_topic << std::endl
                << msg.to_string() << std::endl
                ;

            bool send_result = responser_sender.send(msg.data(), msg.size());
            if (!send_result)
            {
                std::cout << "send response failed!" << std::endl;
                return false;
            }

            std::cout << "send response success!" << std::endl;
            return true;
        }

        void set_request_handler(std::function<void(const RequestMessage &)> handler)
        {
            request_handler_ = handler;
        }

        const std::string& get_server_name(void) const 
        {
            return server_name_;
        }

        const std::string& get_client_name(void) const 
        {
            return client_name_;
        }

    private:
		uint64_t server_id_ = 0;
        std::string client_name_;
        std::string server_name_;
        std::string server_topic_;

		std::hash<std::string> hasher_;
        std::shared_ptr<ipc::channel> listes_channel_;;
        std::function<void(const RequestMessage &)> request_handler_;
    };

} // namespace es_lrpc
