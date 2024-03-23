#include <iostream>
#include <spdlog/spdlog.h>
int main()
{

    std::string a = "/home/liyuxuan/test.onnx";
    std::cout << a.c_str() << std::endl;
    spdlog::info("test {}", a.c_str()) ;   
    return 0;
}
