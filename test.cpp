#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <csignal>
#include <thread>
#include <memory>
#include <vector>

#include "RSXXX_FTDI_Serial.h"
#include "GCI.h"
#include "include/core.h"
#include "include/utils.h"
#include "include/socket.h"
#include "spi.h"

int main() {
    system("clear");

    std::cout << "Тестовый обмен" << std::endl;

    tp::u32 count      = 0;
    tp::bit init       = true;
    tp::s32 spi_socket = 0;

    char tx[1250];
    char rx[1250];

    struct spi_ioc_transfer transfer = {
        .len         = 1250,
        .delay_usecs = 0
    };

    transfer.cs_change = 0;

    for (tp::u32 i = 0; ; i++) {
        if (i > 0)
            init = false;

        memset(tx, 0, sizeof(tx));

        KAMAz_spi_rc1::KAMAz_spi::spi_transmit("/dev/spidev1.0", 
                                               &transfer, 
                                               &spi_socket,
                                               init, false, 
                                               SPI_MODE_0, 
                                               tx, rx, 8,
                                               1000000);
    }
}