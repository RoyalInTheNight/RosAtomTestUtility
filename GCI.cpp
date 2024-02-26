#include "GCI.h"
#include <asm-generic/ioctls.h>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>

GCI_RosAtomFTDI::GCI_RosAtomFTDI() {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &this->consoleSize);

    col = consoleSize.ws_col;
    row = consoleSize.ws_row;
}

void GCI_RosAtomFTDI::addObject(const uint32_t   type_label,
                                const uint32_t type_x_coord, 
                                const uint32_t type_y_coord, const std::string&     label,
                                                             const uint32_t coord_x_value,
                                                             const uint32_t coord_y_value) {
    dSetup tmpSetup;

    tmpSetup.type_label    = type_label;
    tmpSetup.type_x_coord  = type_x_coord;
    tmpSetup.type_y_coord    = type_y_coord;
    tmpSetup.coord_x_setting = coord_x_value;
    tmpSetup.coord_y_setting = coord_y_value;
    tmpSetup.label_data    = label;

    this->consoleSetup.push_back(tmpSetup);
    this->consoleArgs.push_back(type_label);
}

/*void GCI_RosAtomFTDI::addObject(const uint32_t type_label, const std::vector<std::string>& labels) {
    for (const auto& pLabel : labels) {
        this->consoleSetup.insert({type_label, pLabel});
        this->consoleArgs.push_back(type_label);
    }
}*/

void GCI_RosAtomFTDI::print() {
    if (consoleSetup.empty())
        std::cout << ColoredGCIText::red("[ERROR]Вывод не настроен") << std::endl;

    for (const auto& cArgs: consoleSetup) {
        switch (cArgs.type_label) {
            case 0xa1: // title
                if (cArgs.type_x_coord != coord_x_center &&
                    cArgs.type_y_coord != coord_y_center) {
                    if (cArgs.type_x_coord == b_coord_x) {
                        if (cArgs.type_y_coord == b_coord_y)
                            for (uint32_t i = 0; i < cArgs.coord_y_setting; i++)
                                std::cout << std::endl;

                        for (uint32_t i = 0; i < cArgs.coord_x_setting; i++)
                            std::cout << " ";

                        std::cout << cArgs.label_data << "\t// this is title" << std::endl;
                    }
                }

                else if (cArgs.type_x_coord    == coord_x_center &&
                         cArgs.coord_x_setting == coord_x_center_yes) {
                    
                    int size_label = cArgs.label_data.size();
                    int  col_label = (col - size_label) / 2;

                    if (cArgs.type_y_coord    == coord_y_center &&
                        cArgs.coord_y_setting == coord_y_center_yes) {

                        int row_label = row / 2;

                        for (uint32_t i = 0; i < row_label; i++)
                            std::cout << std::endl;
                    }

                    for (uint32_t i = 0; i < col_label; i++)
                        std::cout << " ";

                    std::cout << cArgs.label_data << "\t// this is title" << std::endl;
                }

            break;

            case 0xa2: // input
                if (cArgs.type_x_coord != coord_x_center &&
                    cArgs.type_y_coord != coord_y_center) {
                    if (cArgs.type_x_coord == b_coord_x) {
                        if (cArgs.type_y_coord == b_coord_y) {
                            for (uint32_t i = 0; i < cArgs.coord_y_setting; i++)
                                std::cout << std::endl;
                        }

                        for (uint32_t i = 0; i < cArgs.coord_x_setting; i++)
                            std::cout << " ";

                        std::cout << cArgs.label_data << ": "  << "\t// this is input" << std::endl;
                    }
                }

                else if (cArgs.type_x_coord    == coord_x_center &&
                         cArgs.coord_x_setting == coord_x_center_yes) {
                    
                    int size_label = cArgs.label_data.size();
                    int  col_label = (col - size_label) / 2;

                    if (cArgs.type_y_coord    == coord_y_center &&
                        cArgs.coord_y_setting == coord_y_center_yes) {

                        int row_label = row / 2;

                        for (uint32_t i = 0; i < row_label; i++)
                            std::cout << std::endl;
                    }

                    for (uint32_t i = 0; i < col_label; i++)
                        std::cout << " ";

                    std::cout << cArgs.label_data << ": " << "\t// this is input" << std::endl;
                }

            break;

            case 0xa3: // string
                if (cArgs.type_x_coord != coord_x_center &&
                    cArgs.type_y_coord != coord_y_center) {
                    if (cArgs.type_x_coord == b_coord_x) {
                        for (uint32_t i = 0; i < cArgs.coord_x_setting; i++)
                            std::cout << " ";

                        if (cArgs.type_y_coord == b_coord_y) {
                            for (uint32_t i = 0; i < cArgs.coord_y_setting; i++)
                                std::cout << std::endl;
                        }

                        std::cout << cArgs.label_data << "\t// this is string" << std::endl;
                    }
                }

                else if (cArgs.type_x_coord    == coord_x_center &&
                         cArgs.coord_x_setting == coord_x_center_yes) {
                    
                    int size_label = cArgs.label_data.size();
                    int  col_label = (col - size_label) / 2;

                    for (uint32_t i = 0; i < col_label; i++)
                        std::cout << " ";

                    if (cArgs.type_y_coord    == coord_y_center &&
                        cArgs.coord_y_setting == coord_y_center_yes) {

                        int row_label = row / 2;

                        for (uint32_t i = 0; i < row_label; i++)
                            std::cout << std::endl;
                    }

                    std::cout << cArgs.label_data << "\t// this is string" << std::endl;
                }

            break;

            case 0xa4:break;
            case 0xa5:break;
            case 0xa6:break;
            case 0xa7:break;
            case 0xa8:break;
            case 0xa9:break;

            default:
                std::cout << ColoredGCIText::red("[ERROR]Неправильная настройка") << std::endl;

                break;
        }
    }
}

void GCI_RosAtomFTDI::winTest() {
    std::cout << "col: " << col << std::endl;
    std::cout << "row: " << row << std::endl;

    while (true) {
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &consoleSize);

        if (col != consoleSize.ws_col || row != consoleSize.ws_row) {
            col = consoleSize.ws_col;
            row = consoleSize.ws_row;

            system("clear");

            std::cout << "col: " << col << std::endl;
            std::cout << "row: " << row << std::endl;
        }
    }
}

void GCI_RosAtomFTDI::getEnvironment() {

}

std::string ColoredGCIText::red(const std::string &str) {
    std::string red_text = "\x1B[31m" + str + "\033[0m";

    return red_text;
}