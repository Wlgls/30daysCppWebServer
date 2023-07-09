#include "LogStream.h"
#include <iostream>
int main(){

    LogStream os;
    os << "hello world";
    std::cout << os.buffer().data() << std::endl;
    os.resetBuffer();

    os << 11;
    std::cout << os.buffer().data() << std::endl;
    os.resetBuffer();

    os << 0.1;
    std::cout << os.buffer().data() << std::endl;
    os.resetBuffer();

    os << Fmt("%0.5f", 0.1);
    std::cout << os.buffer().data() << std::endl;
    os.resetBuffer();

    return 0;
}