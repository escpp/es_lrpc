#pragma once
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

namespace es_lrpc
{

    class timestamp_ns
    {
    public:
        static std::string to_string(uint64_t nanoseconds)
        {
            auto tp = std::chrono::system_clock::time_point(
                std::chrono::nanoseconds(nanoseconds));

            // 获取秒部分
            std::time_t t = std::chrono::system_clock::to_time_t(tp);
            std::tm tm = *std::localtime(&t);

            // 获取毫秒部分
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                          tp.time_since_epoch()) %
                      1000;

            std::ostringstream oss;
            oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
                << "." << std::setw(3) << std::setfill('0') << ms.count();

            return oss.str();
        }

        static uint64_t now(void)
        {
            return std::chrono::duration_cast<std::chrono::nanoseconds>(
                       std::chrono::system_clock::now().time_since_epoch())
                .count();
        }
    };
} // namespace es_lrpc