#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdio>
#include <thread>
#include <queue>
#include <string.h>
#include <iomanip>

int main(int argc, char *argv[])
{
    __uint32_t var1 = 0;
    __uint32_t var2 = 0;
    __uint32_t var3 = 0;
    __uint32_t var4 = 0;
    __uint32_t var5 = 0;

    while (true)
    {
        usleep(1000);
        std::cout << "var1 is " << var1 << std::endl;
        std::cout << "var2 is " << var2 << std::endl;
        std::cout << "var3 is " << var3 << std::endl;
        std::cout << "var4 is " << var4 << std::endl;
        std::cout << "var5 is " << var5 << std::endl;
        var1 += 1;
        var2 += 2;
        var3 += 3;
        var4 += 4;
        var5 += 5;
        std::cout << "\033[5A";
    }
}