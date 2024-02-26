#ifndef GCI
#define GCI

#include <sys/ioctl.h>
#include <cstdio>

#include <cstdint>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <map>

class ColoredGCIText {
public:
    static std::string red(const std::string&);
    static std::string black(const std::string&);
    static std::string green(const std::string&);
    static std::string yellow(const std::string&);
    static std::string blue(const std::string&);
    static std::string magenta(const std::string&);
    static std::string cyan(const std::string&);
    static std::string white(const std::string&);
    static std::string brightBlack(const std::string&);
    static std::string brightRed(const std::string&);
    static std::string brightYellow(const std::string&);
    static std::string brightBlue(const std::string&);
    static std::string brightMagenta(const std::string&);
    static std::string brightCyan(const std::string&);
    static std::string brightWhite(const std::string&);
};

class GCI_RosAtomFTDI {
private:
    typedef std::vector<uint32_t>                   dArgs;
    typedef winsize                               ttySize;

    typedef struct console {
        uint32_t      type_label;
        uint32_t    type_x_coord;
        uint32_t    type_y_coord;
        uint32_t coord_x_setting;
        uint32_t coord_y_setting;

        std::string   label_data;
    } dSetup;

    typedef std::vector<dSetup>                 dArgSetup;

private:
    ttySize      consoleSize;
    dArgs        consoleArgs;
    dArgSetup   consoleSetup;

    int32_t col;
    int32_t row;

public:
    const uint32_t title          = 0xa1;
    const uint32_t input          = 0xa2;
    const uint32_t string         = 0xa3;
    const uint32_t b_coord_x      = 0xa4;
    const uint32_t b_coord_y      = 0xa5;
    const uint32_t e_coord_x      = 0xa6;
    const uint32_t e_coord_y      = 0xa7;
    const uint32_t coord_x_center = 0xa8;
    const uint32_t coord_y_center = 0xa9;

    const uint32_t coord_x_center_yes = 0xfc;
    const uint32_t coord_x_center_no  = 0xfd;
    const uint32_t coord_y_center_no  = 0xfe;
    const uint32_t coord_y_center_yes = 0xff;

public:
    GCI_RosAtomFTDI();

    void addObject(const uint32_t, 
                   const uint32_t, 
                   const uint32_t, const std::string&, const uint32_t,
                                                       const uint32_t);
    // void addObject(const uint32_t, const std::vector<std::string>&);

    void print(const dArgSetup&);
    void print();

    void getEnvironment();
    void getEnvironment(const dArgSetup&);

    void winTest();
};

#endif // GCI