
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

typedef enum {
    msgid_NONE = 0,
    msgid_CAN1,
    msgid_CAN2,
    msgid_CAN3,
    msgid_GNSS,
    msgid_ETH_AUTOMOTIVE,
    msgid_LIN,
    msgid_1WIRE,
    msgid_DCDC,
    msgid_OUTPUTS,
    msgid_BATTERY,
    msgid_INPUTS,
    msgid_RTC
}EXCHANGE_IDs_t;

namespace IOSignal {
    typedef struct OSig {
        OSig() : msg_id{9} {}
        OSig(char *rx, uint32_t begPoint, uint32_t endPoint) {
            msg_id = (int)rx[begPoint];

            switch1_enstate = rx[begPoint + 1];
            switch2_enstate = rx[begPoint + 2];
            mute_enstate    = rx[endPoint];
        }

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

    typedef struct RTC {
        RTC() : msg_id(12) {}
        RTC(char *rx, uint32_t begPoint, uint32_t endPoint) {
            msg_id = (int)rx[begPoint];

            DD = rx[begPoint + 1];
            MM = rx[begPoint + 2];
            YY = rx[begPoint + 3];
            hh = rx[begPoint + 4];
            mm = rx[begPoint + 5];
            ss = rx[endPoint];
        }

        uint8_t msg_id;
        uint8_t DD;
        uint8_t MM;
        uint8_t YY;
        uint8_t hh;
        uint8_t mm;
        uint8_t ss;
    } __RTC;

    typedef struct ISig {
        ISig() : msg_id{11} {}
        ISig(char *rx, uint32_t begPoint, uint32_t endPoint) {
            msg_id = (int)rx[begPoint];

            AIN1 = ((rx[begPoint + 1] << 1)  + (rx[begPoint + 2]));
            AIN2 = ((rx[begPoint + 3] << 1)  + (rx[begPoint + 4]));
            AIN3 = ((rx[begPoint + 5] << 1)  + (rx[begPoint + 6]));
            AIN4 = ((rx[begPoint + 7] << 1)  + (rx[begPoint + 8]));
            AIN5 = ((rx[begPoint + 9] << 1)  + (rx[begPoint + 10]));
            AIN6 = ((rx[begPoint + 11] << 1) + (rx[begPoint + 12]));

            DIN1 = rx[begPoint + 13];
            DIN2 = rx[begPoint + 14];
            DIN3 = rx[begPoint + 15];
            DIN4 = rx[begPoint + 16];
            DIN5 = rx[begPoint + 17];
            DIN6 = rx[begPoint + endPoint];
        }

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
        void RTC();

        void menu();
    };
}

#endif //TEST_UTILITY_IUTILITY_H
