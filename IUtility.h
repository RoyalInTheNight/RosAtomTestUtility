
//
// Created by ritn on 10/13/23.
//

#ifndef TEST_UTILITY_IUTILITY_H
#define TEST_UTILITY_IUTILITY_H

#include <string>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>
#include <functional>

using json       = nlohmann::json;
using file_read  =  std::ifstream;

namespace test {
    class IUtility {
    private:
	enum class button
		: uint8_t {
	    BUTTON_SOS = 0x38,
	    BUTTON_SRV = 0x39
	};

        std::string 		 config_name;
        std::string      path_sos_led_script;
        std::string     path_sim_init_script;
        std::string            path_platform;

	// static void button_handler(const button&);

    public:
        enum class BIP_mode
                : uint8_t {
            LED_SOS_RED   = 0x1d,
            LED_SOS_GREEN = 0x1e,
            LED_SRV       = 0x1f
        };

        IUtility();
        IUtility(const std::string& name);

        bool BIP();
        bool SIM();
        bool CAN();

        void menu();
    };
}

#endif //TEST_UTILITY_IUTILITY_H
