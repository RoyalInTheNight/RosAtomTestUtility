//
// Created by ritn on 10/13/23.
//
#include "IUtility.h"

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <csignal>
#include <thread>
#include <memory>

#include "RSXXX_FTDI_Serial.h"

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

bool test::IUtility::CAN() {
    if (system("/tmp/fota.sock")) {
	std::cout << "file deleted" << std::endl;
    }

    else {
	std::cout << "file delete success" << std::endl;
    }
	
    if (system(path_platform.c_str())) {
        std::cout << "Command: " << path_platform << " error" << std::endl;

        return false;
    }

    return true;
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

	        case 3:break;
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

            case 1:
                system("clear");

                std::cout << "Введите номер тестируемого интерфейса и нажмите Enter:" << std::endl
                          << "1 - RS232.1" << std::endl
                          << "2 - RS232.2" << std::endl
                          << "3 - RS485"   << std::endl
                          << "rs-peak>";

                rs_peak = 0;

                std::cin >> rs_peak;

                switch (rs_peak) {
                    case 1:
                        if (ftdi.FTDI_SetDevice("/dev/ttyUSB3") == RS232_1::FTDI_Errno::FTDI_SetDeviceError)
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
                                             << "принято:   "  << message_read << std::endl;
                                }

                                else {
                                    std::cout << "<-=====-RS232.1-=====->" << std::endl << std::endl
                                                 << "ввод:  "      << std::endl
                                                 << "отправлено: " << std::endl
                                                 << "принято:   "  << message_read << std::endl;
                                }
                            }
                        }).detach();

                        while (true) {
                            system("clear");

                            std::cout << "<-=====-RS232.1-=====->" << std::endl << std::endl
                                      << "ввод:  "      << std::endl
                                      << "отправлено: " << std::endl
                                      << "принято:   "  << std::endl << "      \033[3A";

                            std::cin >> rsXXX;

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

            default:
                std::cout << "Unknown option" << std::endl;

                break;

        }
    }
}
