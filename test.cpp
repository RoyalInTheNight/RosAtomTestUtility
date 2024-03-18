#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <cstdio>
#include <thread>
#include <queue>
#include <string.h>
#include <iomanip>

/*int main(int argc, char *argv[])
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
}*/

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "[ERROR]Array don't found" << std::endl;

        return -1;
    }

    std::string _ch;

    uint8_t __ch = 0;

    std::vector<uint8_t> n_queue;

    for (uint32_t i = 1; i < strlen(argv[1]); i += 2) {
        _ch.push_back(argv[1][i - 1]);
        _ch.push_back(argv[1][i]);

        __ch = std::stoi(_ch);
        n_queue.push_back(__ch);

        __ch = 0;
        
        _ch.clear();
    }

    for (const auto& __: n_queue)
        std::cout << (int32_t)__ << std::endl;

    return 0;
}