#ifndef KAMAz_SPI
#define KAMAz_SPI

#include "include/utils.h"
#include "include/core.h"

#include <asm-generic/int-ll64.h>
#include <cstddef>
#include <ios>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <cstring>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>
#include <iostream>

namespace KAMAz_spi_rc1 {
    enum class spi_status : tp::u8 {
        o_spi_error   = 1,
        ms_spi_error  = 2,
        mg_spi_error  = 3,
        m_spi_warning = 4,
        ss_spi_error  = 5,
        sg_spi_error  = 6,
        bs_spi_error  = 7,
        bg_spi_error  = 8,
        send_spi_error= 9,
        ok_spi        = 0
    };

    typedef struct {
        tp::u8    can_num;
        tp::u8   data_len;
        tp::u32    can_id;
        tp::u32 timestamp;
        tp::u8    data[8];
    } spi_can_msg;

    class KAMAz_spi_utils {
    public:
        spi_can_msg can_separator(tp::s32 can_num, const char *can_message) {
            KAMAz_spi_rc1::spi_can_msg msg;

            size_t count = 0;

            msg.can_num = can_num;

            char *token = strtok((char *)can_message, " ");

            msg.can_id = (tp::u32)strtol(token, NULL, 16);

            count += strlen(token) + 1;
            token  = strtok((char *)can_message + count, " ");
            count += strlen(token) + 1;

            msg.timestamp = (tp::u32)strtol(token, NULL, 16);

            token  = strtok((char *)can_message + count, " ");
            count += strlen(token) + 1;

            msg.data_len = (tp::u8)strtol(token, NULL, 16);

            for (tp::s32 i = 0; i < (tp::s32)msg.data_len; i++) {
                token = strtok((char *)can_message + count, " ");

                count += strlen(token) + 1;

                msg.data[i] = (tp::u8)strtol(token, NULL, 16);
            }

            return msg;
        }

        spi_can_msg go_can_separator(tp::s32 can_num, std::string can_message) {
            KAMAz_spi_rc1::spi_can_msg msg;

            if (can_message.empty())
                return msg;

            std::string   can_id;
            std::string can_data;
            std::vector<std::string> data;

            for (auto& can : can_message) {
                if (can == '#')
                    break;

                else
                    can_id.push_back(can);
            }

            bool separator = false;

            for (auto& can : can_message) {
                if (separator)
                    if (can != '.')
                        can_data.push_back(can);

                if (can == '#')
                    separator = true;
            }

            std::cout << "can_id: " << can_id << std::endl << "can_data: " << can_data << std::endl;

            if (can_data.size() % 2 != 0)
                return msg;

            msg.can_num   = can_num;
            msg.can_id    = (tp::u32)strtol(can_id.c_str(), NULL, 16);
            msg.timestamp = 0x00000000;
            msg.data_len  = (tp::u8)(can_data.size() / 0x00000002);

            data.resize((tp::s32)msg.data_len);

            for (tp::u32 i = 0, t = 0; i < can_data.size(); i++) {
                data.at(t).push_back(can_data.at(i));
                data.at(t).push_back(can_data.at(++i));

                ++t;
            }

            for (tp::u32 i = 0; i < msg.data_len; i++)
                msg.data[i] = (tp::u8)strtol(data.at(i).c_str(), NULL, 16);

            return msg;
        }

        template<class __Tp> inline std::string to_hex(__Tp val,
                                                       size_t width = sizeof(__Tp) * 2) {
            std::stringstream ss;

            ss << std::uppercase 
               << std::setfill('0') 
               << std::setw(width) 
               << std::hex 
               << (val | 0);

            return ss.str();
        }

        template<class __Tp> inline std::string go_to_hex(__Tp val,
                                                     size_t width = sizeof(__Tp) * 2) {
            std::stringstream ss;

            ss << std::hex << (val | 0);

            return ss.str();
        }
    };

    namespace KAMAz_spi {
        tp::addr spi_transmit(const char *     CAN_DEVICE,
                              struct spi_ioc_transfer *tr,
                              int32_t *	   fd,
                              bool 		   init_spi,
                              bool        debug_mod,
                              uint32_t 	   mode,
                              char *	   tx,
                              char *	   rx,
                              uint8_t 	   bits  = 8,
                              uint32_t     speed = 500000) {
            int32_t result;

            if (init_spi) {
                uint32_t request;
                int32_t out_fd;

                if (mode & SPI_TX_OCTAL)
                    tr->tx_nbits = 8;
                else if (mode & SPI_TX_QUAD)
                    tr->tx_nbits = 4;
                else if (mode & SPI_TX_DUAL)
                    tr->tx_nbits = 2;

                else if (mode == 0)
                    tr->tx_nbits = 1;

                if (mode & SPI_RX_OCTAL)
                    tr->tx_nbits = 8;
                else if (mode & SPI_RX_QUAD)
                    tr->tx_nbits = 4;
                else if (mode & SPI_RX_DUAL)
                    tr->rx_nbits = 2;

                else if (mode == 0)
                    tr->tx_nbits = 1;

                tr->speed_hz 	  = speed;
                tr->bits_per_word = bits;
                tr->tx_buf   	  = (__u64)tx;
                tr->rx_buf   	  = (__u64)rx;

                if (!(mode & SPI_LOOP)) {
                    if (mode & (SPI_TX_OCTAL | SPI_TX_QUAD | SPI_TX_DUAL))
                        tr->rx_buf = 0;
                    else if (mode & (SPI_RX_OCTAL | SPI_RX_QUAD | SPI_RX_DUAL))
                        tr->tx_buf = 0;
                }

                *fd = open(CAN_DEVICE, O_RDWR);

                if (*fd < 0) {
                    if (debug_mod)
                        std::cerr << "spi_nor: [FAILED]Error open can" << std::endl;
                    
                    return;
                }

                else
                    if (debug_mod)
                        std::cout << "spi_nor: [  OK  ]Open can device success" << std::endl;

                result  = ioctl(*fd, SPI_IOC_WR_MODE32, &mode);
                request = mode;

                if (result < 0) {
                    if (debug_mod)
                        std::cerr << "spi_nor: [FAILED]Error set spi mode wr" << std::endl;
                    
                    return;
                }

                else
                    if (debug_mod)
                        std::cout << "spi_nor: [  OK  ]Set spi mode success" << std::endl;

                result = ioctl(*fd, SPI_IOC_RD_MODE32, &mode);

                if (result < 0) {
                    if (debug_mod)
                        std::cerr << "spi_nor: [FAILED]Error set spi mode rd" << std::endl;
                    
                    return;
                }

                else
                    if (debug_mod)
                        std::cout << "spi_nor: [  OK  ]Set spi mode success" << std::endl;

                if (request != mode)
                    if (debug_mod)
                        std::cout << "spi_nor: [WARNING]Device does not support mode: " << std::hex << request << std::endl;

                result = ioctl(*fd, SPI_IOC_WR_BITS_PER_WORD, &bits);

                if (result < 0) {
                    if (debug_mod)
                        std::cerr << "spi_nor: [FAILED]Can't set bits" << std::endl;
                    
                    return;
                }

                else
                    if (debug_mod)
                    std::cout << "spi_nor: [  OK  ]Set bits success" << std::endl;

                result = ioctl(*fd, SPI_IOC_RD_BITS_PER_WORD, &bits);

                if (result < 0) {
                    if (debug_mod)
                        std::cerr << "spi_nor: [FAILED]Can't get bits" << std::endl;
                    
                    return;
                }

                else
                    if (debug_mod)
                        std::cout << "spi_nor: [  OK  ]Get bits success" << std::endl;

                result = ioctl(*fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

                if (result < 0) {
                    if (debug_mod)
                        std::cerr << "spi_nor: [FAILED]Set speed error" << std::endl;
                    
                    return;
                }

                else
                    if (debug_mod)
                        std::cout << "spi_nor: [  OK  ]Set speed success" << std::endl;

                result = ioctl(*fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

                if (result < 0) {
                    if (debug_mod)
                        std::cerr << "spi_nor: [FAILED]Get speed error" << std::endl;
                    
                    return;
                }

                else
                    if (debug_mod)
                        std::cout << "spi_nor: [  OK  ]Get speed success" << std::endl;
            }


            result = ioctl(*fd, SPI_IOC_MESSAGE(1), tr);

            if (result < 1) {
                if (debug_mod)
                    std::cerr << "spi_nor: [FAILED]Send message spi error" << std::endl;
                
                return;
            }
        }
    }
}

#endif // KAMAz_SPI