#ifndef RETRO_CORE_H
#define RETRO_CORE_H

#include <string>
#include <windows.h>
#include <iostream>
#include <vector>

namespace RetroUI {
    // Global Dimensions
    extern int CONSOLE_WIDTH;
    extern int CONSOLE_HEIGHT;
    extern int SIDEBAR_WIDTH;
    extern int CONTENT_WIDTH;
    extern int sidebarPage;

    // Colors & Styles
    extern std::string RESET;
    extern std::string ORANGE;
    extern std::string BRIGHT_ORANGE;
    extern std::string YELLOW;
    extern std::string CYAN;
    extern std::string RED;
    extern std::string WHITE;
    extern std::string GREEN;
    extern std::string DARK_GRAY;
    extern std::string BOLD;
    extern std::string INVERSE;
    
    // Symbols
    extern std::string BLOCK;
    extern std::string BLOCK_TOP;
    extern std::string BLOCK_BOTTOM;

    // Core Functions
    void setupConsole();
    void clear();
    void gotoxy(int x, int y);
    std::string center(const std::string& text, int width);
    void playSound(int freq, int duration);
    void typeWrite(const std::string& text, int speed);
    void bootSequence();
    void updateTheme();
}
#endif