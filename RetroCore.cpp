#include "RetroCore.h"
#include <ctime>
#pragma comment(lib, "user32.lib")

namespace RetroUI {
    int CONSOLE_WIDTH = 210;
    int CONSOLE_HEIGHT = 58;
    int SIDEBAR_WIDTH = 60;
    int CONTENT_WIDTH = CONSOLE_WIDTH - SIDEBAR_WIDTH;
    int sidebarPage = 0;

    std::string RESET = "\033[0m";
    std::string ORANGE = "\033[38;5;208m";
    std::string BRIGHT_ORANGE = "\033[38;5;214m";
    std::string YELLOW = "\033[38;5;226m";
    std::string CYAN = "\033[38;5;51m";
    std::string RED = "\033[38;5;196m";
    std::string WHITE = "\033[38;5;255m";
    std::string GREEN = "\033[38;5;46m";
    std::string DARK_GRAY = "\033[38;5;235m";
    std::string BOLD = "\033[1m";
    std::string INVERSE = "\033[7m";
    
    std::string BLOCK = "\xE2\x96\x88";
    std::string BLOCK_TOP = "\xE2\x96\x80";
    std::string BLOCK_BOTTOM = "\xE2\x96\x84";

    void setupConsole() {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwOutMode = 0;
        GetConsoleMode(hOut, &dwOutMode);
        dwOutMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwOutMode);
        SetConsoleOutputCP(65001);

        HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
        DWORD dwInMode = 0;
        GetConsoleMode(hIn, &dwInMode);
        dwInMode |= ENABLE_MOUSE_INPUT;
        dwInMode &= ~ENABLE_QUICK_EDIT_MODE;
        SetConsoleMode(hIn, dwInMode);

        COORD largeBuffer = {400, 150};
        SetConsoleScreenBufferSize(hOut, largeBuffer);

        HWND consoleWindow = GetConsoleWindow();
        ShowWindow(consoleWindow, SW_MAXIMIZE);

        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hOut, &csbi);
        CONSOLE_WIDTH = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        CONSOLE_HEIGHT = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        CONTENT_WIDTH = CONSOLE_WIDTH - SIDEBAR_WIDTH;

        COORD finalBuffer = {static_cast<SHORT>(CONSOLE_WIDTH), static_cast<SHORT>(CONSOLE_HEIGHT)};
        SetConsoleScreenBufferSize(hOut, finalBuffer);

        if (consoleWindow != NULL) {
            LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
            style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
            SetWindowLong(consoleWindow, GWL_STYLE, style);
        }

        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hOut, &cursorInfo);
        cursorInfo.bVisible = false;
        SetConsoleCursorInfo(hOut, &cursorInfo);
    }

    void clear() { std::cout << "\033[2J\033[1;1H"; }
    void gotoxy(int x, int y) { std::cout << "\033[" << y << ";" << x << "H"; }
    void playSound(int freq, int duration) { Beep(freq, duration); }

    std::string center(const std::string& text, int width) {
        int len = (int)text.length();
        if (len >= width) return text;
        int left = (width - len) / 2;
        return std::string(left, ' ') + text + std::string(width - len - left, ' ');
    }

    void typeWrite(const std::string& text, int speed) {
        for (char c : text) { std::cout << c << std::flush; Sleep(speed); }
    }

    void bootSequence() {
        clear();
        gotoxy(2, 2); std::cout << GREEN; typeWrite("BIOS CHECK ................. OK", 10);
        gotoxy(2, 3); typeWrite("LOADING KERNEL ............. OK", 10);
        gotoxy(2, 4); typeWrite("MOUNTING FILESYSTEM ........ OK", 10);
        gotoxy(2, 5); typeWrite("INITIALIZING GRAPHICS ...... OK", 10);
        gotoxy(2, 7); std::cout << "MEMORY: ";
        for(int i=0; i<=4096; i+=256) {
            std::cout << "\rMEMORY: " << i << " MB OK" << std::flush;
            Sleep(2);
        }
        std::cout << RESET << std::endl;
        Sleep(300);
    }

    void updateTheme() {
        std::time_t now = std::time(nullptr);
        std::tm *ltm = std::localtime(&now);
        if (ltm->tm_hour >= 18 || ltm->tm_hour < 6) {
            ORANGE = "\033[38;5;57m"; BRIGHT_ORANGE = "\033[38;5;129m";
            YELLOW = "\033[38;5;51m"; CYAN = "\033[38;5;201m"; RED = "\033[38;5;160m";
        }
    }
}