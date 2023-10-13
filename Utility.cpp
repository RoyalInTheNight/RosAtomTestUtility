//
// Created by ritn on 10/13/23.
//
#include "IUtility.h"

#include <iostream>
#include <termios.h>
#include <unistd.h>

#include <thread>

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

test::IUtility::IUtility() {
    path_platform        = "";
    path_sim_init_script = "";
    path_sos_led_script  = "";

    json config = {
            {"sos_path", "/usr/local/tools/led_sos.sh"},
            {"sim_path", "/usr/local/tools/init_sim7600H.sh"},
            {"plm_path", "~/KAMAz_PLATFORM-main/build/main -d"}
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

bool test::IUtility::BIP(const test::IUtility::BIP_mode& mode) {
    if (mode == BIP_mode::LED_SOS_RED)
        path_sos_led_script += path_sos_led_script + " red";

    if (mode == BIP_mode::LED_SOS_GREEN)
        path_sos_led_script += path_sos_led_script + " green";

    if (mode == BIP_mode::LED_SRV)
        path_sos_led_script += path_sos_led_script + " srv";

    if (system(path_sos_led_script.c_str())) {
        std::cout << "Command: " << path_sos_led_script << " error" << std::endl;

        return false;
    }

    std::cout << "Led ok..." << std::endl;

    return true;
}

bool test::IUtility::SIM() {
    if (system(path_sim_init_script.c_str())) {
        std::cout << "Command: " << path_sim_init_script << " error" << std::endl;

        return false;
    }

    std::cout << L"Инициализация модема...OK" << std::endl;

    if (system("qmicli -p -d /dev/cdc-wdm0 --uim-read-transparent=0x3F00,0x2FE2")) {
        std::cout << "Modem iccid get error" << std::endl;

        return false;
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
    int pick_value     = 0;
    int pick_led_value = 0;

    while (true) {
        std::cout << "ТЕХНОЛОГИЧЕСКИЙ РЕЖИМ" << std::endl
                  << "1. БИП"                << std::endl
                  << "2. SIM"                << std::endl
                  << "3. CAN"                << std::endl
                  << "command> ";

        std::cin >> pick_value;

        switch (pick_value) {
            case 1:
                std::cout << "1. LED_SOS_RED"   << std::endl
                          << "2. LED_SOS_GREEN" << std::endl
                          << "3. LED_SRV"       << std::endl
                          << "led_commmand> ";

                std::cin >> pick_led_value;

                if (pick_led_value == 1) {
                    if (BIP(BIP_mode::LED_SOS_RED))
                        std::cout << "Led red pick success" << std::endl;

                    else
                        std::cout << "Led red pick failed"  << std::endl;
                }

                else if (pick_led_value == 2) {
                    if (BIP(BIP_mode::LED_SOS_GREEN))
                        std::cout << "Led green pick failed" << std::endl;

                    else
                        std::cout << "Led green pick failed" << std::endl;
                }

                else if (pick_led_value == 3) {
                    if (BIP(BIP_mode::LED_SRV))
                        std::cout << "Led srv pick failed" << std::endl;

                    else
                        std::cout << "Led srv pick failed" << std::endl;
                }

                else
                    std::cout << "Unknown led" << std::endl;

                break;

            case 2:
                if (SIM())
                    std::cout << "SIM init success" << std::endl;

                else
                    std::cout << "SIM init error..." << std::endl;

                break;

            case 3:
                std::thread([&]() -> void {
                    if (CAN())
                        std::cout << "CAN init success" << std::endl;

                    else
                        std::cout << "CAN init error..." << std::endl;
                }).detach();

                break;

            default:
                std::cout << "Unknown option" << std::endl;

                break;

        }
    }
}