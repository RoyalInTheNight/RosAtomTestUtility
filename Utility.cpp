//
// Created by ritn on 10/13/23.
//
#include "IUtility.h"

#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <csignal>
#include <thread>
#include <memory>

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

void signal_handler(int signum) {
    switch (signum) {
	case SIGINT:
	    if (system("clear"))
		std::cout << "Command: clear error" << std::endl;

	    break;

	default:
	    break;
    }

    std::cout << "ТЕХНОЛОГИЧЕСКИЙ РЕЖИМ" << std::endl
                  << "1. БИП"                << std::endl
                  << "2. SIM"                << std::endl
	    	  << "3. ГИРОСКОП"           << std::endl
                  << "4. CAN"                << std::endl
	    	  << "5. ETHERNET MULTIMEDIA"<< std::endl
                  << "command> ";
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

/*void test::IUtility::button_handler(const button& btn) {
    auto pipe_exec = [&](const char *cmd) -> std::string {
	    if (cmd == nullptr)
	        return "";

    	std::array<char, 128> buff;
	    std::string result;

    	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);

	    if (!pipe) return "";

	    while (fgets(buff.data(), buff.size(), pipe.get()) != nullptr)
	        result += buff.data();;

	    return result;
    };

    bool srv_pressed = false;
    bool sos_pressed = false;

    while (true) {
    	if (btn == button::BUTTON_SOS) {
            if (std::stoi(pipe_exec("gpioget 3 8")) == 1) {
                if (srv_pressed)
                    std::cout << "[ INFO ]Button sos unpressed" << std::endl;

                srv_pressed = false;
            }

            if (std::stoi(pipe_exec("gpioget 3 8")) == 0) {
                if (!srv_pressed)
                    std::cout << "[ INFO ]Button sos pressed" << std::endl;

                srv_pressed = true;
            }
        }

    	if (btn == button::BUTTON_SRV) {
            if (std::stoi(pipe_exec("gpioget 3 9")) == 1) {
                if (sos_pressed)
                    std::cout << "[ INFO ]Button srv unpressed" << std::endl;

                sos_pressed = false;
            }

            if (std::stoi(pipe_exec("gpioget 3 9")) == 0) {
                if (!sos_pressed)
                    std::cout << "[ INFO ]Button srv pressed" << std::endl;

                sos_pressed = true;
            }
        }
    }
}*/

/*bool test::IUtility::BIP(const test::IUtility::BIP_mode& mode, bool led) {
    std::string path = path_sos_led_script;

    if (mode == BIP_mode::LED_SOS_RED) {
        path = path_sos_led_script + " red";
	
	if (led)
	    path += " 1";
    }

    if (mode == BIP_mode::LED_SOS_GREEN) {
        path = path_sos_led_script + " green";

        if (led)
            path += " 1";
    }

    if (mode == BIP_mode::LED_SRV) {
        path = path_sos_led_script + " srv";

        if (led)
            path += " 1";
    }

    if (system(path.c_str())) {
        std::cout << "Command: " << path << " error" << std::endl;

        return false;
    }

    std::cout << "Led ok..." << std::endl;

    return true;
}*/

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
    if (system(path_platform.c_str())) {
        std::cout << "Command: " << path_platform << " error" << std::endl;

        return false;
    }

    return true;
}

void test::IUtility::menu() {
    signal(SIGINT, signal_handler);

    // std::thread(button_handler, button::BUTTON_SOS).detach();
    // std::thread(button_handler, button::BUTTON_SRV).detach();

    int pick_value     = 0;
    int pick_led_value = 0;

    bool led_red   = false;
    bool led_green = false;
    bool led_srv   = false;

    while (true) {
        std::cout << "ТЕХНОЛОГИЧЕСКИЙ РЕЖИМ" << std::endl
                  << "1. БИП"                << std::endl
                  << "2. SIM"                << std::endl
		  << "3. ГИРОСКОП"	     << std::endl
                  << "4. CAN"                << std::endl
		  << "5. ETHERNET MULTIMEDIA"<< std::endl
                  << "command> ";

        std::cin >> pick_value;

        switch (pick_value) {
            case 1:
                /*while (true) {
                    std::cout << "1. LED_SOS_RED" << std::endl
                              << "2. LED_SOS_GREEN" << std::endl
                              << "3. LED_SRV" << std::endl
                              << "4. EXIT" << std::endl
                              << "led_commmand> ";

                    std::cin >> pick_led_value;

                    if (pick_led_value == 1) {
                        if (led_red == false)
                            led_red = true;

                        else
                            led_red = false;

                        if (BIP(BIP_mode::LED_SOS_RED, led_red))
                            std::cout << "Led red pick success" << std::endl;

                        else
                            std::cout << "Led red pick failed" << std::endl;
                    } else if (pick_led_value == 2) {
                        if (led_green == false)
                            led_green = true;

                        else
                            led_green = false;

                        if (BIP(BIP_mode::LED_SOS_GREEN, led_green))
                            std::cout << "Led green pick failed" << std::endl;

                        else
                            std::cout << "Led green pick failed" << std::endl;
                    } else if (pick_led_value == 3) {
                        if (led_srv == false)
                            led_srv = true;

                        else
                            led_srv = false;

                        if (BIP(BIP_mode::LED_SRV, led_srv))
                            std::cout << "Led srv pick failed" << std::endl;

                        else
                            std::cout << "Led srv pick failed" << std::endl;
                    } else if (pick_led_value == 4) {
                        break;
                    } else
                        std::cout << "Unknown led" << std::endl;

                    // break;

                    std::cout << "_____________________________________" << std::endl;
                    std::cout << "|led_red|led_green|led_srv|   pick  |" << std::endl;
                    std::cout << "|_______|_________|_______|_________|" << std::endl
                              << "|   " << led_red
                              << "   |    " << led_green
                              << "    |   " << led_srv
                              << "   |    " << pick_led_value << "    |" << std::endl
                              << "-------------------------------------" << std::endl;

                }*/

                BIP();

                break;

            case 2:
                if (SIM())
                    std::cout << "SIM init success" << std::endl;

                else
                    std::cout << "SIM init error..." << std::endl;

                break;

	    case 3:break;
            case 4:
                std::thread([&]() -> void {
                    if (CAN())
                        std::cout << "CAN init success" << std::endl;

                    else
                        std::cout << "CAN init error..." << std::endl;
                }).detach();

                break;

	    case 5:
		std::cout << "ТЕСТ ETHERNET MULTIMEDIA" << std::endl;
		break;

            default:
                std::cout << "Unknown option" << std::endl;

                break;

        }
    }
}
