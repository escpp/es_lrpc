cmake_minimum_required(VERSION 3.10)
project(es_lrpc VERSION 1.0.0 LANGUAGES CXX)

# 通过tools/xxx/xxx.cmake 设置头文件和库的路径，相当于-Dxxx_INCLUDE_DIR=xxx -Dxxx_LIBRARY_DIR=xxx
include_directories("${CMAKE_INCLUDE_PATH}")
link_directories("${CMAKE_LIBRARY_PATH}")

# 定义es_lrpc静态库
add_library(es_lrpc
    src/es_lrpc_client.cpp
    src/es_lrpc_server.cpp
)

target_include_directories(es_lrpc
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 定义es_lrpc_shared共享库
add_library(es_lrpc_shared SHARED
    src/es_lrpc_client.cpp
    src/es_lrpc_server.cpp
)

target_include_directories(es_lrpc_shared
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(es_lrpc_shared PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    POSITION_INDEPENDENT_CODE ON
    OUTPUT_NAME "es_lrpc"
)

# 安装头文件
install(
    DIRECTORY "include/"
    DESTINATION "include/es_lrpc"
)

#   # 安装共享库
#   install(TARGETS es_lrpc_shared
#       LIBRARY DESTINATION lib
#   )

#   # 安装静态库
#   install(TARGETS es_lrpc
#       ARCHIVE DESTINATION lib
#   )

# 设置包含路径变量
set(EN_RPC_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 客户端示例
add_executable(es_lrpc_client_example  examples/message.pb.cc examples/client.cpp)
target_include_directories(es_lrpc_client_example PRIVATE ${EN_RPC_INCLUDE_DIR})
target_link_libraries(es_lrpc_client_example PRIVATE ipc es_lrpc pthread protobuf)

# 服务端示例
add_executable(es_lrpc_server_example  examples/message.pb.cc examples/server.cpp)
target_include_directories(es_lrpc_server_example PRIVATE ${EN_RPC_INCLUDE_DIR})
target_link_libraries(es_lrpc_server_example PRIVATE ipc es_lrpc pthread protobuf)
