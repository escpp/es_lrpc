#pragma once

#include <string>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <google/protobuf/util/json_util.h>
#include "timestamp.h"

namespace es_lrpc
{
    class message_header
    { 
    public:
        uint64_t m_client_id = 0;
        uint64_t m_message_id = 0;
    };

    template <typename T>
    std::string msg2json(const T& obj)
    {
        std::string json_str;
        google::protobuf::util::MessageToJsonString(obj, &json_str);
        return json_str;
    }

    template<typename T>
    class message
    {
    public:
        inline static const size_t header_size = sizeof(message_header);
    public:
        message(const void* data, size_t length)
        {
            unserialize(data, length);
        }

        message(const T &payload)
        {
            set_message(payload);
        }

        message(uint64_t client_id, uint64_t message_id, const T& message)
        {
            m_client_id = client_id;
            m_message_id = message_id;
            set_message(message);
        }
        
        size_t size(void) const
        {
            if (m_buffer.size() <= header_size) {
                return 0;
            }
            return m_buffer.size();
        }

        const void* data(void) const
        {
            if (m_buffer.size() <= header_size) {
                return nullptr;
            }
            return m_buffer.data();
        }

        bool unserialize(const void* data, size_t length)
        {
            m_buffer.clear();
            if (nullptr == data || length <= 0) {
                return false;
            }


            m_buffer.append((char*)data, length);
            message_header* ptr = (message_header*)m_buffer.data();
            m_client_id = ptr->m_client_id;
            m_message_id = ptr->m_message_id;
            return true;
        }

        void set_client_id(uint64_t client_id)
        {
            m_client_id = client_id;
        }

        void set_message_id(uint64_t message_id)
        {
            m_message_id = message_id;
        }

        void set_message(const T& payload)
        {
            size_t payload_len = payload.ByteSizeLong();
            m_buffer.resize(header_size + payload_len);
            message_header* ptr = (message_header*)m_buffer.data();
            ptr->m_client_id = m_client_id;
            ptr->m_message_id = m_message_id;
            payload.SerializeToArray((char*)m_buffer.data() + header_size, payload_len);
        }

        bool set_message(const void* payload, size_t length)
        {
            if (nullptr == payload || length <= 0) {
                return false;
            }

            m_buffer.resize(header_size + length);
            message_header* ptr = (message_header*)m_buffer.data();
            ptr->m_client_id = m_client_id;
            ptr->m_message_id = m_message_id;
            memcpy(m_buffer.data() + header_size, payload, length);
            return true;
        }

        uint64_t get_client_id(void) const
        {
            return m_client_id;
        }

        uint64_t get_message_id(void) const
        {
            return m_message_id;
        }
        
        std::shared_ptr<T> get_message(void) const
        {
            if (m_buffer.size() <= header_size) {
                return nullptr;
            }

            std::shared_ptr<T> ptr = std::make_shared<T>();
            bool ret = ptr->ParseFromArray(m_buffer.data() + header_size, m_buffer.size() - header_size);
            if (false == ret) {
                return nullptr;
            }

            return ptr;
        }

        std::string get_message_jstring(void) const
        {
            auto msg_ptr = get_message();
            if (nullptr == msg_ptr) {
                return "";
            }

            return msg2json(*msg_ptr);
        }

        std::string to_string() const
        {
            std::stringstream ss;
            ss 
                << "message:" << std::endl
                << "    client_id: " << m_client_id << std::endl
                << "    message_id: " << m_message_id << std::endl
                << "    timestamp: " << timestamp_ns::to_string(m_message_id) << std::endl
                << "    message: " << get_message_jstring() << std::endl
                ;

            return ss.str();
        }

    private:
        uint64_t m_client_id;
        uint64_t m_message_id;
        std::string m_buffer;
    };
} // namespace es_lrpc
