//
// Created by ritn on 10/13/23.
//
#include "IUtility.h"

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

int32_t getch() {
    struct termios oldattr, newattr;
    int32_t ch;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return ch;
}

void main_menu() {
    std::cout     << "ТЕХНОЛОГИЧЕСКИЙ РЕЖИМ"                << std::endl
                  << "1.  RS232 RS485"                      << std::endl
                  << "2.  БИП"                              << std::endl
	    	      << "3.  USB"                              << std::endl
                  << "4.  SIM"                              << std::endl
	    	      << "5.  ETHERNET AUTOMOTIVE"              << std::endl
                  << "6.  КОНТРОЛЛЕР АКБ"                   << std::endl
                  << "7.  ВЫХОДНЫЕ СИГНАЛЫ"                 << std::endl
                  << "8.  ВХОДНЫЕ СИГНАЛЫ"                  << std::endl
                  << "9.  ГИРОСКОП"                         << std::endl
                  << "10. МАГНИТОМЕТР"                      << std::endl
                  << "11. K-Line"                           << std::endl
                  << "12. OneWire"                          << std::endl
                  << "13. Bluetooth & WIFI"                 << std::endl
                  << "14. LoRa WAN"                         << std::endl
                  << "15. IRIDIUM"                          << std::endl
                  << "16. CAN"                              << std::endl
                  << "17. ETHERNET MULTIMEDIA"              << std::endl
                  << "18. RTC"                              << std::endl
                  << "19. GNSS"                             << std::endl
                  << "command> ";
}

void signal_handler(int signum) {
    switch (signum) {
	case SIGINT:
	    if (system("clear"))
		std::cout << "Command: clear error" << std::endl;

	    break;

	default:
	    break;
    }

    main_menu();
}

test::IUtility::IUtility() {
    path_platform        = "";
    path_sim_init_script = "";
    path_sos_led_script  = "";

    json config = {
            {"sos_path", "/usr/local/tools/led_sos.sh"},
            {"sim_path", "/usr/local/tools/init_sim7600.sh"},
            {"plm_path", "~/main -d"}
    };

    for (auto& cfg : config.items()) {
        if (cfg.key() == "sos_path")
            path_sos_led_script = cfg.value();

        if (cfg.key() == "sim_path")
            path_sim_init_script = cfg.value();

        if (cfg.key() == "plm_path")
            path_platform = cfg.value();
    }
}

test::IUtility::IUtility(const std::string &name) {
    path_platform        = "";
    path_sos_led_script  = "";
    path_sim_init_script = "";

    file_read file(name);

    auto config = json::parse(file);

    for (auto& cfg : config.items()) {
        if (cfg.key() == "sos_path")
            path_sos_led_script = cfg.value();

        if (cfg.key() == "sim_path")
            path_sim_init_script = cfg.value();

        if (cfg.key() == "plm_path")
            path_platform = cfg.value();
    }
}

#include <array>
#include <cstdio>

void test::IUtility::BIP() {
    if (system("clear"))
        std::cout << "[FAILED]Clear screen error" << std::endl;

    std::cout << "Проверка БИП" << std::endl;

    auto pipe_exec = [&](const char *cmd) -> std::string {
        if (cmd == nullptr)
            return "";

        std::array<char, 128> buff;
        std::string result;

        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);

        if (!pipe) return "";

        while (fgets(buff.data(), buff.size(), pipe.get()) != nullptr)
            result += buff.data();

        return result;
    };

    {
        // execute led down

        if (system("gpioset 3 17=0;gpioset 3 18=0;gpioset 3 19=0"))
            std::cout << "[FAILED]Leds error" << std::endl;
    }

    bool srv_pressed = false;
    int  sos_pressed = 0;

    bool exit_pressed = false;

    std::thread([&]() -> void {
	char ch;

	std::cin >> ch;

	if (ch == 'x')
	    exit_pressed = true;
    }).detach();    

    while (true) {
        if (pipe_exec("gpioget 3 8") == "0\n") {
            std::cout << "Нажата кнопка SOS" << std::endl;

            ++sos_pressed;

            if (sos_pressed == 1) {
                if (system("gpioset 3 17=1"))
                    std::cout << "[FAILED]Set gpio error" << std::endl;

                if (system("gpioset 3 18=0"))
                    std::cout << "[FAILED]Set gpio error" << std::endl;
            }

            if (sos_pressed == 2) {
                if (system("gpioset 3 18=1"))
                    std::cout << "[FAILED]Set gpio error" << std::endl;

                if (system("gpioset 3 17=0"))
                    std::cout << "[FAILED]Set gpio error" << std::endl;
            }

            if (sos_pressed > 2)
                sos_pressed = 0;
        }

        if (pipe_exec("gpioget 3 9") == "0\n") {
            std::cout << "Нажата кнопка SRV" << std::endl;

            srv_pressed = true;

            if (srv_pressed) {
                if (system("gpioset 3 19=1"))
                    std::cout << "[FAILED]Set gpio error" << std::endl;
            }
        }

	if (exit_pressed)
	    return;
    }
}

bool test::IUtility::SIM() {
    if (system(path_sim_init_script.c_str())) {
        std::cout << "Command: " << path_sim_init_script << " error" << std::endl;

	if (system("ifconfig eth0 up")) {
	     std::cout << "eth0 error" << std::endl;

	     return false;
    	}

        return false;
    }

    std::cout << L"Инициализация модема...OK" << std::endl;

    if (system("qmicli -p -d /dev/cdc-wdm0 --uim-read-transparent=0x3F00,0x2FE2")) {
        std::cout << "Modem iccid get error" << std::endl;

        return false;
    }

    if (system("ifconfig eth0 up")) {
	std::cout << "eth0 error" << std::endl;

	return false;
    }

    std::ofstream resolv("/etc/resolv.conf");

    if (resolv.is_open()) {
	resolv << "nameserver 8.8.8.8";

	resolv.close();
    }

    if (system("speedtest")) {
        std::cout << "Modem speedtest error" << std::endl;

        return false;
    }

    return true;
}

void test::IUtility::RTC() {
    system("clear");

    std::cout << "RTC:" << std::endl;

    int pick = 0;

    tp::u32 count       = 0;
    tp::u32 offset_size = 0;
    tp::u32 offset_RTC  = sizeof(IOSignal::__RTC);
    tp::bit offset_eof  = false;
    tp::bit init        = true;
    tp::s32 spi_socket  = 0;

    std::vector<uint32_t> ISignalOffsetList;
    std::vector<IOSignal::__RTC>        RTC;

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
                                               init, 
                                               false, 
                                               SPI_MODE_0, tx, rx, 
                                               8, 1000000);

        while (!offset_eof) {
            if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_INPUTS) {
                //ISignalOffsetList.push_back(offset_size);
                offset_size += 19;
                //count++;
            }

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_BATTERY)
                offset_size += 21;

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_OUTPUTS)
                offset_size += 4;

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_DCDC)
                offset_size += 5;

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_CAN1 ||
                     rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_CAN2 ||
                     rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_CAN3)
                offset_size += 19;

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_GNSS)
                offset_size += (2 + (int)rx[offset_size + 1]);

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_ETH_AUTOMOTIVE)
                offset_size += (2 + (int)rx[offset_size + 1]);

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_LIN)
                offset_size += (2 + (int)rx[offset_size + 1]);

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_1WIRE)
                offset_size += (2 + (int)rx[offset_size + 1]);

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_RTC) {
                ISignalOffsetList.push_back(offset_size);
                offset_size += 7;

                count++;
            }

            else
                offset_eof =  true;
        }       offset_eof = false;

        for (tp::u32 t = 0; t < ISignalOffsetList.size(); t++) {
            IOSignal::__RTC _RTC(rx, ISignalOffsetList.at(t), 6);

            RTC.push_back(_RTC);
        }

        for (tp::u32 j = 0; j < RTC.size(); j++) {
            std::cout << RTC.at(j).DD << ":"
                      << RTC.at(j).MM << ":"
                      << RTC.at(j).YY << ": "
                      << RTC.at(j).hh << "."
                      << RTC.at(j).mm << "."
                      << RTC.at(j).ss << std::endl;
        }
    }
}

void test::IUtility::ISignal() {
    system("clear");

    int pick = 0;

    tp::u32 count           = 0;
    tp::u32 offset_size     = 0;
    tp::u32 offset_IOSignal = sizeof(IOSignal::__ISignal);
    tp::bit offset_eof      = false;
    tp::bit init            = true;
    tp::s32 spi_socket      = 0;

    std::vector<uint32_t>  ISignalOffsetList;
    std::vector<IOSignal::__ISignal>      IS;

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

        // system("clear");

        memset(tx, 0, sizeof(tx));

        KAMAz_spi_rc1::KAMAz_spi::spi_transmit("/dev/spidev1.0", 
                                               &transfer, 
                                               &spi_socket, 
                                               init, 
                                               false, 
                                               SPI_MODE_0, 
                                               tx, rx,
                                               8, 1000000);

        while (!offset_eof) {
            if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_INPUTS) {
                ISignalOffsetList.push_back(offset_size);
                offset_size += 19;
                count++;
            }

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_BATTERY)
                offset_size += 21;

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_OUTPUTS)
                offset_size += 4;

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_DCDC)
                offset_size += 5;

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_CAN1 ||
                     rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_CAN2 ||
                     rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_CAN3)
                offset_size += 19;

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_GNSS)
                offset_size += (2 + (int)rx[offset_size + 1]);

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_ETH_AUTOMOTIVE)
                offset_size += (2 + (int)rx[offset_size + 1]);

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_LIN)
                offset_size += (2 + (int)rx[offset_size + 1]);

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_1WIRE)
                offset_size += (2 + (int)rx[offset_size + 1]);

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_RTC)
                offset_size += 7;

            else
                offset_eof = true;
        }

        offset_eof = false;

        for (tp::u32 t = 0; t < ISignalOffsetList.size(); t++) {
            IOSignal::__ISignal ISignal(rx, ISignalOffsetList.at(t), 18);

            IS.push_back(ISignal);
        }

        for (tp::u32 j = 0; j < IS.size(); j++) {
            std::cout << "ВХОДНЫЕ СИГНАЛЫ:" << std::endl
                      << "AIN1, B - " << (int)IS[j].AIN1 << std::endl
                      << "AIN2, B - " << (int)IS[j].AIN2 << std::endl
                      << "AIN3, B - " << (int)IS[j].AIN3 << std::endl
                      << "AIN4, B - " << (int)IS[j].AIN4 << std::endl
                      << "Клемма 300, B - " << (int)IS[j].AIN5 << std::endl
                      << "Клемма 150, B - " << (int)IS[j].AIN6 << std::endl
                      << "DIN1 - " << (int)IS[j].DIN1 << std::endl
                      << "DIN1 - " << (int)IS[j].DIN2 << std::endl
                      << "DIN1 - " << (int)IS[j].DIN3 << std::endl
                      << "DIN1 - " << (int)IS[j].DIN4 << std::endl
                      << "DIN1 - " << (int)IS[j].DIN5 << std::endl
                      << "GPS.DETECT - " << (int)IS[j].DIN6 << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void test::IUtility::OSignal() {
    int pick = 0;

    // IOSignal::__OSignal OS;
    IOSignal::__OSignal IS;

    tp::u32 count           = 0;
    tp::u32 offset_size     = 0;
    tp::u32 offset_IOSignal = sizeof(IOSignal::__OSignal);
    tp::bit offset_eof      = false;
    tp::bit init            = true;
    tp::s32 spi_socket      = 0;

    std::vector<uint32_t>  ISignalOffsetList;
    std::vector<IOSignal::__OSignal> OS;

    char tx[1250];
    char rx[1250];

    struct spi_ioc_transfer transfer = {
        .len         = 1250,
        .delay_usecs = 0  
    };

    transfer.cs_change = 0;

    std::thread([&]() {
        while (true) {
            std::cout << "ВЫХОДНЫЕ СИГНАЛЫ:\n"
                      << "1 - Включить LS1\n"
                      << "2 - Выключить LS1\n"
                      << "3 - Включить LS2\n"
                      << "4 - Выключить LS2\n"
                      << "5 - Включить MUTE\n"
                      << "6 - Выключить MUTE\n"
                      << "enter-> ";

            std::cin >> pick;

            switch (pick) {
                case 1:IS.switch1_enstate = 1;break;
                case 2:IS.switch1_enstate = 0;break;
                case 3:IS.switch2_enstate = 1;break;
                case 4:IS.switch2_enstate = 0;break;
                case 5:IS.mute_enstate    = 1;break;
                case 6:IS.mute_enstate    = 0;break;
            }
        }
    }).detach();

    for (tp::u32 i = 0; ; i++) {
        if (i > 0)
            init = false;

        memset(tx, 0, sizeof(tx));

        KAMAz_spi_rc1::KAMAz_spi::spi_transmit("/dev/spidev1.0", 
                                               &transfer, 
                                               &spi_socket, 
                                               init,
                                               false, 
                                               SPI_MODE_0, tx, rx,
                                               8, 1000000);

        // memset(tx, 0, 1250);

        while (!offset_eof) {
            if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_INPUTS)
                //ISignalOffsetList.push_back(offset_size);
                offset_size += 19;
                //count++;

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_BATTERY)
                offset_size += 21;

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_OUTPUTS) {
                ISignalOffsetList.push_back(offset_size);
                offset_size += 4;

                count++;
            }

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_DCDC)
                offset_size += 5;

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_CAN1 ||
                     rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_CAN2 ||
                     rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_CAN3)
                offset_size += 19;

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_GNSS)
                offset_size += (2 + (int)rx[offset_size + 1]);

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_ETH_AUTOMOTIVE)
                offset_size += (2 + (int)rx[offset_size + 1]);

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_LIN)
                offset_size += (2 + (int)rx[offset_size + 1]);

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_1WIRE)
                offset_size += (2 + (int)rx[offset_size + 1]);

            else if (rx[offset_size] == (int)EXCHANGE_IDs_t::msgid_RTC)
                offset_size += 7;

            else
                offset_eof = true;
        }

        for (tp::u32 j = 0; j < ISignalOffsetList.size(); j++) {
            IOSignal::__OSignal OSignal(rx, ISignalOffsetList.at(j), 3);

            OS.push_back(OSignal);
        }

        for (tp::u32 k = 0; k < sizeof(OS) / sizeof(IOSignal::__OSignal); k++) {
            if (IS.switch1_enstate != OS[k].switch1_enstate ||
                IS.switch2_enstate != OS[k].switch2_enstate ||
                IS.mute_enstate    != OS[k].mute_enstate)
                memcpy(tx, &IS, sizeof(IS));
        }
    }
}

bool test::IUtility::CAN() {
    tp::bit init       = true;
    tp::s32 spi_socket = 0;

    char tx[100];
    char rx[100];

    int p             = 0;
    int n_transaction = 0;

    std::string can_type_data;

    KAMAz_spi_rc1::spi_can_msg can[5];
    struct spi_ioc_transfer transfer = {
        .len         = 100,
        .delay_usecs = 0  
    };

    transfer.cs_change = 0;

    auto check_size = [&]() {
        tp::u32 count = 0;

        for (tp::u32 i = 0; i < 100; i += 20)
            if (tx[i] != 0) ++count;

        return count;
    };

    for (tp::u32 i = 0; ; i++) {
        if (i > 0)
            init = false;

        KAMAz_spi_rc1::KAMAz_spi::spi_transmit("/dev/spidev1.0", 
                                               &transfer, 
                                               &spi_socket, 
                                               init, 
                                               false,
                                               SPI_MODE_0, 
                                               tx, rx, 
                                               8, 
                                               10000000);

        memset(tx, 0, 100);

        std::cout << "CAN\n"
                  << "Введите номер тестируемого интерфейса и нажмите Enter:\n"
                  << "1 - ПРИЕМ CAN1\n"
                  << "2 - ОТПРАВКА CAN1\n"
                  << "3 - ПРИЕМ CAN2\n"
                  << "4 - ОТПРАВКА CAN2\n"
                  << "5 - ПРИЕМ CAN3\n"
                  << "6 - ОТПРАВКА CAN3\n"
                  << "can> ";

        std::cin >> p;

        switch (p) {
            case 1:
                std::cout << "ПРИЕМ CAN1\n"
                          << "№ ТРАНЗАКЦИИ\tID\tTYPE\tDATA\n";

                for (tp::u32 t = 0; t < check_size(); t++) {
                    memcpy(&can[t], &tx[t * 20], 20);

                    if (can[t].can_num == 1) {
                        n_transaction++;

                        std::cout << n_transaction << "\t" << can[t].can_id
                                                   << "\t" << "no\t";


                        for (int j = 0; j < can[t].data_len; j++)
                            std::cout << can[t].data[j] << " ";

                        std::cout << std::endl; 
                    }
                }

            break;

            case 2:
                std::cout << "ОТПРАВКА CAN\n"
                          << "1 - ID -> 0x1FFFFFFF\n"
                          << "2 - TYPE EXT/STD\n"
                          << "3 - DATA -> 00 11 22 33 44 55 66 77\n"
                          << "ENTER -> ";

                std::cin >> can_type_data;

            break;

            case 3:
                std::cout << "ПРИЕМ CAN1\n"
                          << "№ ТРАНЗАКЦИИ\tID\tTYPE\tDATA\n";

                for (tp::u32 t = 0; t < check_size(); t++) {
                    memcpy(&can[t], &tx[t * 20], 20);

                    if (can[t].can_num == 2) {
                        n_transaction++;

                        std::cout << n_transaction << "\t" << can[t].can_id
                                                   << "\t" << "no\t";


                        for (int j = 0; j < can[t].data_len; j++)
                            std::cout << can[t].data[j] << " ";

                        std::cout << std::endl; 
                    }
                }

            break;
            
            case 4:
            break;

            case 5:
                std::cout << "ПРИЕМ CAN1\n"
                          << "№ ТРАНЗАКЦИИ\tID\tTYPE\tDATA\n";

                for (tp::u32 t = 0; t < check_size(); t++) {
                    memcpy(&can[t], &tx[t * 20], 20);

                    if (can[t].can_num == 3) {
                        n_transaction++;

                        std::cout << n_transaction << "\t" << can[t].can_id
                                                   << "\t" << "no\t";


                        for (int j = 0; j < can[t].data_len; j++)
                            std::cout << can[t].data[j] << " ";

                        std::cout << std::endl; 
                    }
                }

            break;

            case 6:
            break;
        } 
    }
}

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>

void test::IUtility::menu() {
    signal(SIGINT, signal_handler);

    int pick_value     = 0;
    int pick_led_value = 0;
    int rs_peak        = 0;

    bool led_red   = false;
    bool led_green = false;
    bool led_srv   = false;

    std::ofstream  input_rs;
    std::ifstream output_rs;

    std::string rsXXX;
    std::string message_read;

    RS232_1 ftdi;

    sockaddr_in server_ethernet_multimedia;
    sockaddr_in host_local_ip_address;
    socklen_t   host_local_len = sizeof(host_local_ip_address);
    int         socket_server_dns;
    char        buffer_ip[100];
    char *      point;

    uint16_t    dns_port   = 53;
    std::string dns_server = "8.8.8.8";

    std::string anykey;

    std::string rsOUT;
    std::string rsDNT;

    std::string rsSended;
    std::string pressAnyKey;

    while (true) {
        main_menu();
        std::cin >> pick_value;

        switch (pick_value) {
            case 2:
                BIP();

                break;

            case 4:
                if (SIM())
                    std::cout << "SIM init success" << std::endl;

                else
                    std::cout << "SIM init error..." << std::endl;

                break;

	        case 3:
		        system("rm -rf usb_mount;mkdir usb_mount;umount /dev/sda");

                std::cout << "USB:\nПодключите flash накопитель в порт USB Type-C" << std::endl;

    		    while (true) {
	    	        system("clear");
		
		            std::cout << "USB:\nПодключите flash накопитель в порт USB Type-C" << std::endl;

                    if (system("ls /dev/sda")) {
                        std::cout << ColoredGCIText::red("USB flash накопитель не авториззирован") << std::endl;
			            system("gpioset 0 13=0;gpioset 0 13=1");
    		        }

	    	        else {
                        break;
                    }
                    //    system("mount /dev/sda usb_mount;ls usb_mount");
    		    }

		        system("mount /dev/sda usb_mount;ls usb_mount");

                std::cin >> pressAnyKey;

		    break;

            case 8:
                this->ISignal();
            break;
            case 16:
                std::thread([&]() -> void {
                    if (CAN())
                        std::cout << "CAN init success" << std::endl;

                    else
                        std::cout << "CAN init error..." << std::endl;
                }).detach();

                break;

	        case 17:
                system("clear");

                socket_server_dns = ::socket(AF_INET, SOCK_DGRAM, 0);

                if (socket_server_dns < 0) {
                    std::cout << "AF_INET suport fail" << std::endl;
                    
                    break;
                }

                memset(&server_ethernet_multimedia, 0, sizeof(server_ethernet_multimedia));
                server_ethernet_multimedia.sin_addr.s_addr = inet_addr(dns_server.c_str());
                server_ethernet_multimedia.sin_port        = htons(dns_port);
                server_ethernet_multimedia.sin_family      = AF_INET;

                if (connect(socket_server_dns, (const sockaddr *)&server_ethernet_multimedia, sizeof(server_ethernet_multimedia)) < 0) {
                    std::cout << "Connect to dns failed" << std::endl;

                    break;
                }

                if (getsockname(socket_server_dns, (sockaddr *)&host_local_ip_address, &host_local_len) < 0) {
                    std::cout << "Get local address failed" <<  std::endl;

                    break;
                }

                point = (char *)inet_ntop(AF_INET, &host_local_ip_address.sin_addr, buffer_ip, 100);

                if (point != NULL) {
                    std::cout << "IP-адрес БТП "  << buffer_ip << std::endl;
                    std::cout << "Выполните команду 'ping " << buffer_ip  << "' в командной строке данного ПК." << std::endl;
                }

                else {
                    std::cout << "Operation failed" << std::endl;
                    
                    break;
                }

                close(socket_server_dns);

                std::cout << "socket closed, dns server " << dns_server << ":" << dns_port << " incative" << std::endl;

                std::cin >> anykey;

		    break;

            case 7:
                this->OSignal();

            break;

            case 1:
                system("clear");

                std::cout << "Введите номер тестируемого интерфейса и нажмите Enter:" << std::endl
                          << "1 - RS232.1" << std::endl
                          << "2 - RS232.2" << std::endl
                          << "3 - RS485"   << std::endl
                          << "rs-peak> ";

                rs_peak = 0;

                std::cin >> rs_peak;

                switch (rs_peak) {
                    case 1:
                        if (ftdi.FTDI_SetDevice("/dev/ttyUSB0") == RS232_1::FTDI_Errno::FTDI_SetDeviceError)
                            std::cout << "[ERROR]Device don't set" << std::endl;

                        if (ftdi.FTDI_Open() == RS232_1::FTDI_Errno::FTDI_OpenError)
                            std::cout << "[ERROR]FTDI open error" << std::endl;

                        if (ftdi.FTDI_TC_GetAttributeTTY() == RS232_1::FTDI_Errno::FTDI_TTY_GetAttributeError)
                            std::cout << "[ERROR]FTDI get attriibute TTY error" << std::endl;
                            
                        ftdi.FTDI_SetTTY_C_CFLAG(~PARENB,  ~CSTOPB,         ~CSIZE,
                                                     CS8, ~CRTSCTS, CREAD | CLOCAL);

                        ftdi.FTDI_SetTTY_C_LFLAG(~ICANON, ~ECHO, ~ECHOE, 
                                                 ~ECHONL, ~ISIG);

                        ftdi.FTDI_SetTTY_C_IFLAG(~(IXON   | IXOFF  | IXANY),
                                                 ~(IGNBRK | BRKINT | PARMRK |
                                                   ISTRIP | INLCR  | IGNCR  | ICRNL));

                        ftdi.FTDI_SetTTY_C_OFLAG(~OPOST, ~ONLCR);
                        ftdi.FTDI_SetTTY_C_CC(VTIME, VMIN,
                                              10,    0    );

                        ftdi.FTDI_CF_SetInput_SPEED (B115200);
                        ftdi.FTDI_CF_SetOutput_SPEED(B115200);

                        std::thread([&](){
                            while (true) {
                                message_read = ftdi.FTDI_ReadBuffer();

                                // std::cout << "     \033[1A" << message_read;

                                system("clear");

                                if (!rsXXX.empty()) {
                                    std::cout << "<-=====-RS232.1-=====->" << std::endl << std::endl
                                             << "ввод:  "      << std::endl
                                             << "отправлено: " << rsXXX        << std::endl
                                             << "принято:   "  << message_read << std::endl << "      \033[4A";
                                }

                                else {
                                    std::cout << "<-=====-RS232.1-=====->" << std::endl << std::endl
                                                 << "ввод:  "      << std::endl
                                                 << "отправлено: " << std::endl
                                                 << "принято:   "  << message_read << std::endl << "      \033[4A";
                                }

				message_read = "";
                            }
                        }).detach();

                        while (true) {
                            system("clear");

			    if (rsSended.empty()) {
                                std::cout << "<-=====-RS232.1-=====->" << std::endl << std::endl
                                          << "ввод:  "      << std::endl
                                          << "отправлено: " << std::endl
                                          << "принято:   "  << std::endl << "      \033[3A";
                            }

			    else {
			    	std::cout << "<-=====-RS232.1-=====->" << std::endl << std::endl
					  << "ввод:  "      << std::endl
				      	  << "отправлено: " << rsSended << std::endl
				          << "принято:   "  << std::endl << "      \033[3A";
			    }

                            std::cin >> rsXXX;
			    rsSended  = rsXXX;

			    rsXXX = "";

			    system("clear");

			    // std::cout << "<-=====-RS232.1-=====->" << std::endl << std::endl
			    //	      << "ввод: "       << std::endl
			    //	      << "отправлено: " << rsXXX << std::endl
			    //	      << "принято:   " << std::endl;

                            ftdi.FTDI_WriteBuffer(rsXXX);

                            if (rsXXX == "exit")
                                break;
                        }

                        break;

                        case 2:
                            std::cout << "<-=====-RS232.2-=====->" << std::endl
                                      << "ввод:  "      << std::endl
                                      << "отправлено: " << std::endl
                                      << "принято:   "  << std::endl;
                        break;

                        case 3:
                            std::cout << "<-=====-RS485-=====->" << std::endl
                                      << "ввод:  "      << std::endl
                                      << "отправлено: " << std::endl
                                      << "принято:   "  << std::endl;
                        break;
                }

            break;

            case 19:
                system("clear");
                
                std::cout << "GNSS: " << std::endl;

                this->GNSS();

            break;

            default:
                std::cout << "Unknown option" << std::endl;

                break;

        }
    }
}

void test::IUtility::GNSS() {
    struct spi_ioc_transfer transfer = {
        .len         = 100,
        .delay_usecs = 0
    };

    KAMAz_spi_rc1::spi_can_msg msg[5];
    platform_utils                plm;

    transfer.cs_change = 0;

    char buffer_tx[100];
    char buffer_rx[100];

    tp::bit init       = true;
    tp::s32 spi_socket = 0;

    auto check_size = [&]() -> tp::s32 {
        tp::s32 count = 0;

        for (tp::s32 t = 0; t < 100; t += 20)
            if (buffer_rx[t] != 0)
                ++count;

        return count;
    };

    for (tp::u32 i = 0;; i++) {
        if (i > 0)
            init = false;

        KAMAz_spi_rc1::KAMAz_spi::spi_transmit("/dev/spidev1.0",
                                               &transfer,
                                               &spi_socket, 
                                               init, 
                                               false,
                                               SPI_MODE_0,
                                               buffer_tx,
                                               buffer_rx, 8, 10000000);

        memset(buffer_tx, 0, 100);

        for (tp::u32 j = 0; j < check_size(); j++) {
            plm.memcpy(&msg[j], &buffer_rx[j * 20], 20);

            switch (msg[j].can_num) {
                case 4:
                    std::cout << buffer_rx << std::endl;

                //default:
                //    std::cout << ColoredGCIText::red("[ERROR]Unknown data string") << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}