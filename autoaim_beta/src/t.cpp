#include <iostream>

int main()
{

    std::string a = "/home/liyuxuan/test.onnx";
    std::cout << a.c_str() << std::endl;
    std::cout << a.replace("onnx","cache")<< std::endl;
    
    return 0;
}
