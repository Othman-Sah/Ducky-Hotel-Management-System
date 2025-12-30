#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

namespace ConsoleUI {

    void drawHeader(const std::string& title) {
        // Simple header with a border
        std::cout << "**************************************************" << std::endl;
        std::cout << "* " << std::setw(46) << std::left << title << " *" << std::endl;
        std::cout << "**************************************************" << std::endl;
    }

    void drawMenu(const std::vector<std::string>& options) {
        std::cout << "+------------------------------------------------+" << std::endl;
        for (size_t i = 0; i < options.size(); ++i) {
            std::cout << "| " << (i + 1) << ". " << std::setw(43) << std::left << options[i] << " |" << std::endl;
        }
        std::cout << "+------------------------------------------------+" << std::endl;
        std::cout << "Entrez votre choix: ";
    }
    
    void drawInfoBox(const std::vector<std::string>& lines) {
        std::cout << "+------------------------------------------------+" << std::endl;
        for(const auto& line : lines) {
            std::cout << "| " << std::setw(46) << std::left << line << " |" << std::endl;
        }
        std::cout << "+------------------------------------------------+" << std::endl;
    }

} // namespace ConsoleUI

#endif // CONSOLE_UI_H
