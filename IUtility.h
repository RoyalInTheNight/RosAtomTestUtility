
//
// Created by ritn on 10/13/23.
//

#ifndef TEST_UTILITY_IUTILITY_H
#define TEST_UTILITY_IUTILITY_H

#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>
#include <functional>

using json       = nlohmann::json;
using file_read  =  std::ifstream;

namespace IOSignal {
    typedef struct {
        uint8_t          msg_id;
        uint8_t switch1_enstate;
        uint8_t switch2_enstate;
        uint8_t    mute_enstate;
    } __OSignal;

    typedef struct {
        uint8_t msg_id;
        int16_t temperature_batt1;
        uint16_t voltage_batt1;
        uint8_t state_batt1;
        int16_t temperature_batt2;
        uint16_t voltage_batt2;
        uint8_t state_batt2;
        int16_t temperature_batt3;
        uint16_t voltage_batt3;
        uint8_t state_batt3;
        int16_t temperature_batt4;
        uint16_t voltage_batt4;
        uint8_t state_batt4;
    } __Battery;

    typedef struct {
        uint8_t  msg_id;
        uint16_t AIN1;
        uint16_t AIN2;
        uint16_t AIN3;
        uint16_t AIN4;
        uint16_t AIN5;
        uint16_t AIN6;
        uint8_t  DIN1;
        uint8_t  DIN2;
        uint8_t  DIN3;
        uint8_t  DIN4;
        uint8_t  DIN5;
        uint8_t  DIN6;
    } __ISignal;
}

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

        void RS232_FTDI();
        void BIP();
        void USB();
        bool SIM();
        bool CAN();
        void ETHERNET();
        void OSignal();
        void ISignal();
        void HScope();
        void Magnitometr();
        void KLine();
        void OneWire();
        void BT_WIFI();
        void LoRa_WAN();
        void Iridium();
        void LowPower_RealTime();
        void GNSS();

        void menu();
    };
}

#endif //TEST_UTILITY_IUTILITY_H
