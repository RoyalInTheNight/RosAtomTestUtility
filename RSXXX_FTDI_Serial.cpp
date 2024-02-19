#include "RSXXX_FTDI_Serial.h"
#include <cstring>
#include <fcntl.h>
#include <termios.h>
#include <iostream>

RS232_1::RS232_1() {
    this->ftdi_socket = 0;

    this->ftdi_device.clear();
    this->read_buffer.resize(__INT16_MAX__);

    memset(&ftdi_interface, 0, sizeof(ftdi_interface));
}

RS232_1::RS232_1(const std::string& dev)       { this->ftdi_device    = dev; }
RS232_1::RS232_1(const RS232_1::FTDI_Bus& bus) { this->ftdi_socket    = bus; }
RS232_1::RS232_1(const RS232_1::FTDI_TTY& tty) { this->ftdi_interface = tty; }
RS232_1::RS232_1(const RS232_1::FTDI_Bus& bus, const RS232_1::FTDI_TTY& tty) {
    this->ftdi_interface = tty;
    this->ftdi_socket    = bus;
}

RS232_1::RS232_1(const RS232_1& RS232) {
    this->ftdi_socket    = RS232.ftdi_socket;
    this->ftdi_device    = RS232.ftdi_device;
    this->ftdi_interface = RS232.ftdi_interface;
}

RS232_1::FTDI_Errno RS232_1::FTDI_SetDevice(const std::string& dev) {
    if (!dev.size())
        return FTDI_Errno::FTDI_SetDeviceError;

    this->ftdi_device = dev;

    return FTDI_Errno::FTDI_OK;
}

RS232_1::FTDI_Errno RS232_1::FTDI_SetTTY(const RS232_1::FTDI_TTY& tty) {
    if (tty.c_cflag <= 0 || 
        tty.c_iflag <= 0 ||
        tty.c_lflag <= 0 ||
        tty.c_oflag <= 0)
        return FTDI_Errno::FTDI_SetTTY_Error;

    else
        this->ftdi_interface = tty;

    return FTDI_Errno::FTDI_OK;
}

RS232_1::FTDI_Errno RS232_1::FTDI_SetBus(const FTDI_Bus& bus) {
    if (bus < 0)
        return FTDI_Errno::FTDI_SetBusError;

    else
        this->ftdi_socket = bus;

    return FTDI_Errno::FTDI_OK;
}

RS232_1::FTDI_Errno RS232_1::FTDI_Open() {
    // std::cout << ftdi_device.c_str() << std::endl;

    this->ftdi_socket = open(this->ftdi_device.c_str(), O_RDWR);

    if (ftdi_socket < 0)
        return FTDI_Errno::FTDI_OpenError;

    return FTDI_Errno::FTDI_OK;
}

void RS232_1::FTDI_SetTTY_C_CFLAG(int32_t c1, int32_t c2, 
                                  int32_t c3, int32_t c4, 
                                  int32_t c5, int32_t c6) {
    ftdi_interface.c_cflag &= c1;
    ftdi_interface.c_cflag &= c2;
    ftdi_interface.c_cflag &= c3;
    ftdi_interface.c_cflag |= c4;
    ftdi_interface.c_cflag &= c5;
    ftdi_interface.c_cflag |= c6;
}

void RS232_1::FTDI_SetTTY_C_LFLAG(int32_t l1, int32_t l2, 
                                  int32_t l3, int32_t l4, 
                                  int32_t l5) {
    ftdi_interface.c_lflag &= l1;
    ftdi_interface.c_lflag &= l2;
    ftdi_interface.c_lflag &= l3;
    ftdi_interface.c_lflag &= l4;
    ftdi_interface.c_lflag &= l5;
}

void RS232_1::FTDI_SetTTY_C_IFLAG(int32_t i1, int32_t i2) {
    ftdi_interface.c_iflag &= i1;
    ftdi_interface.c_iflag &= i2;
}

void RS232_1::FTDI_SetTTY_C_OFLAG(int32_t o1, int32_t o2) {
    ftdi_interface.c_oflag &= o1;
    ftdi_interface.c_oflag &= o2;
}

void RS232_1::FTDI_SetTTY_C_CC(const int32_t time1, const int32_t time2,
                                    uint32_t    t1,      uint32_t t2    ) {
    ftdi_interface.c_cc[time1] = t1;
    ftdi_interface.c_cc[time2] = t2;
}

void RS232_1::FTDI_CF_SetInput_SPEED(const int32_t ispeed) {
    cfsetispeed(&ftdi_interface, ispeed);
}

void RS232_1::FTDI_CF_SetOutput_SPEED(const int32_t ospeed) {
    cfsetospeed(&ftdi_interface, ospeed);
}

RS232_1::FTDI_Errno RS232_1::FTDI_TC_GetAttributeTTY() {
    if (tcgetattr(ftdi_socket, &ftdi_interface) != 0)
        return FTDI_Errno::FTDI_TTY_GetAttributeError;

    return FTDI_Errno::FTDI_OK;
}

RS232_1::FTDI_Errno RS232_1::FTDI_TC_GetAttribute_TC_ANOW() {
    if (tcsetattr(ftdi_socket, TCSANOW, &ftdi_interface) != 0)
        return FTDI_Errno::FTDI_ANOW_GetAttributeError;

    return FTDI_Errno::FTDI_OK;
}

RS232_1::FTDI_Errno RS232_1::FTDI_WriteBuffer(const std::string& buffer) {
    unsigned char msg[buffer.size()];

    for (int32_t i = 0; i < buffer.size(); i++)
        msg[i] = buffer.at(i);

    if (write(ftdi_socket, msg, sizeof(msg)) < 0)
        return FTDI_Errno::FTDI_WriteMessageError;

    return FTDI_Errno::FTDI_OK;
}

std::string RS232_1::FTDI_ReadBuffer() {
    int32_t num_bytes = 0;

    char read_buf[__INT16_MAX__];
    std::string rread;

    num_bytes = read(ftdi_socket, &read_buf, sizeof(read_buf));

    rread = read_buf;

    return rread;
}

RS232_1::~RS232_1() { close(ftdi_socket); }