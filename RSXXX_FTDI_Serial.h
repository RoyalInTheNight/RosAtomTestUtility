#ifndef RSXXX
#define RSXXX

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#include <string>
#include <vector>

#include <cstdlib>
#include <cstdint>

class RS232_1 {
public:
    typedef int     FTDI_Bus;
    typedef termios FTDI_TTY;

    enum class FTDI_Errno
         : uint8_t {
        FTDI_OpenError              = 0xa1,
        FTDI_TTY_GetAttributeError  = 0xa2,
        FTDI_ANOW_GetAttributeError = 0xa3,
        FTDI_WriteMessageError      = 0xa4,
        FTDI_ReadMessageError       = 0xa5,
        FTDI_SetDeviceError         = 0xa6,
        FTDI_SetTTY_Error           = 0xa7,
        FTDI_SetBusError            = 0xa8,
        FTDI_OK                     = 0xff
    };

private:
    FTDI_Bus ftdi_socket;
    FTDI_TTY ftdi_interface;

    std::string ftdi_device;

    std::vector<char> read_buffer;

public:
    RS232_1();
    RS232_1(const std::string&);
    RS232_1(const FTDI_Bus&);
    RS232_1(const FTDI_TTY&);
    RS232_1(const FTDI_Bus&, const FTDI_TTY&);
    RS232_1(const RS232_1&);

    FTDI_Errno FTDI_SetDevice(const std::string&);
    FTDI_Errno FTDI_SetTTY(const FTDI_TTY&);
    FTDI_Errno FTDI_SetBus(const FTDI_Bus&);
    FTDI_Errno FTDI_Open();

    void FTDI_SetTTY_C_CFLAG(int32_t, int32_t, int32_t,
                             int32_t, int32_t, int32_t);

    void FTDI_SetTTY_C_LFLAG(int32_t, int32_t, int32_t,
                             int32_t, int32_t);

    void FTDI_SetTTY_C_IFLAG(int32_t, int32_t);
    void FTDI_SetTTY_C_OFLAG(int32_t, int32_t);

    void FTDI_SetTTY_C_CC(const int32_t, const int32_t,
                               uint32_t,      uint32_t);

    void FTDI_CF_SetInput_SPEED (const int32_t);
    void FTDI_CF_SetOutput_SPEED(const int32_t);

    FTDI_Errno  FTDI_TC_GetAttributeTTY();
    FTDI_Errno  FTDI_TC_GetAttribute_TC_ANOW();
    FTDI_Errno  FTDI_WriteBuffer(const std::string&);
    std::string FTDI_ReadBuffer();

    ~RS232_1();
};

#endif // RSXXX