#include <iostream>
#include "es_lrpc/timestamp.h"

int main() {
    es_lrpc::timestamp_ns timestamp;

    std::cout << "timestamp_ns: " << timestamp.now() << std::endl; // 1678486206967967  

    return 0;
}