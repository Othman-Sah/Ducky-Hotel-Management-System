#ifndef RETROUI_H
#define RETROUI_H

#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <sstream>
#include <map>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#pragma comment(lib, "user32.lib")

namespace RetroUI {

    // Layout Constants
    static int CONSOLE_WIDTH = 210;
    static int CONSOLE_HEIGHT = 58;
    static int SIDEBAR_WIDTH = 60;
    static int CONTENT_WIDTH = CONSOLE_WIDTH - SIDEBAR_WIDTH;
    static int sidebarPage = 0;

    static std::string RESET = "\033[0m";
    static std::string ORANGE = "\033[38;5;208m";      // Main Borders
    static std::string BRIGHT_ORANGE = "\033[38;5;214m"; // Highlights
    static std::string YELLOW = "\033[38;5;226m";      // Accents
    static std::string CYAN = "\033[38;5;51m";         // Contrast (Free rooms)
    static std::string RED = "\033[38;5;196m";         // Occupied
    static std::string WHITE = "\033[38;5;255m";       // Text
    static std::string GREEN = "\033[38;5;46m";        // Success/Online
    static std::string DARK_GRAY = "\033[38;5;235m";   // Background details
    static std::string BOLD = "\033[1m";
    static std::string INVERSE = "\033[7m";

    // --- Music Player State & Functions ---
    static std::vector<std::string> playlist;
    static int currentSongIndex = 0;
    static bool isMusicPlaying = false;
    static bool isMusicPaused = false;
    static int mediaButtonFocus = -1; // -1: None, 0: Prev, 1: Play, 2: Next

    void loadMusic() {
        if (!playlist.empty()) return;
        WIN32_FIND_DATAA findFileData;
        std::string folder = "music/";
        
        // Try 'music' folder first
        HANDLE hFind = FindFirstFileA((folder + "*.mp3").c_str(), &findFileData);
        
        // If not found, try 'musics' (common typo/variation)
        if (hFind == INVALID_HANDLE_VALUE) {
            folder = "musics/";
            hFind = FindFirstFileA((folder + "*.mp3").c_str(), &findFileData);
        }

        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                playlist.push_back(folder + findFileData.cFileName);
            } while (FindNextFileA(hFind, &findFileData) != 0);
            FindClose(hFind);
        }
    }

    void playMusic() {
        if (playlist.empty()) return;
        // Close any existing instance to prevent errors
        mciSendStringA("close bgm", NULL, 0, NULL);
        std::string cmd = "open \"" + playlist[currentSongIndex] + "\" type mpegvideo alias bgm";
        mciSendStringA(cmd.c_str(), NULL, 0, NULL);
        mciSendStringA("play bgm", NULL, 0, NULL);
        isMusicPlaying = true;
        isMusicPaused = false;
    }

    void stopMusic() {
        mciSendStringA("close bgm", NULL, 0, NULL);
        isMusicPlaying = false;
        isMusicPaused = false;
    }

    void togglePauseMusic() {
        if (isMusicPlaying) {
            if (isMusicPaused) {
                mciSendStringA("resume bgm", NULL, 0, NULL);
                isMusicPaused = false;
            } else {
                mciSendStringA("pause bgm", NULL, 0, NULL);
                isMusicPaused = true;
            }
        } else {
            playMusic();
        }
    }

    void nextMusic() {
        stopMusic();
        if (playlist.empty()) return;
        currentSongIndex = (currentSongIndex + 1) % playlist.size();
        playMusic();
    }

    void setupConsole() {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwOutMode = 0;
        GetConsoleMode(hOut, &dwOutMode);
        dwOutMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwOutMode);
        
        // Set UTF-8 Output for box drawing characters
        SetConsoleOutputCP(65001);

        HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
        DWORD dwInMode = 0;
        GetConsoleMode(hIn, &dwInMode);
        dwInMode |= ENABLE_MOUSE_INPUT;
        dwInMode &= ~ENABLE_QUICK_EDIT_MODE; // Disable QuickEdit to allow mouse input
        SetConsoleMode(hIn, dwInMode);

        // 1. Set a large buffer to allow maximizing
        COORD largeBuffer = {400, 150};
        SetConsoleScreenBufferSize(hOut, largeBuffer);

        // 2. Maximize Window
        HWND consoleWindow = GetConsoleWindow();
        ShowWindow(consoleWindow, SW_MAXIMIZE);

        // 3. Update dimensions based on actual maximized size
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hOut, &csbi);
        CONSOLE_WIDTH = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        CONSOLE_HEIGHT = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        CONTENT_WIDTH = CONSOLE_WIDTH - SIDEBAR_WIDTH;

        // 4. Sync Buffer Size to remove scrollbars
        COORD finalBuffer = {static_cast<SHORT>(CONSOLE_WIDTH), static_cast<SHORT>(CONSOLE_HEIGHT)};
        SetConsoleScreenBufferSize(hOut, finalBuffer);

        // Disable resizing to prevent UI glitches
        if (consoleWindow != NULL) {
            LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
            style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
            SetWindowLong(consoleWindow, GWL_STYLE, style);
        }

        // Hide Cursor
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hOut, &cursorInfo);
        cursorInfo.bVisible = false;
        SetConsoleCursorInfo(hOut, &cursorInfo);
    }

    void clear() {
        std::cout << "\033[2J\033[1;1H";
    }

    void gotoxy(int x, int y) {
        std::cout << "\033[" << y << ";" << x << "H";
    }

    std::string getCurrentTime() {
        std::time_t t = std::time(nullptr);
        std::tm tm = *std::localtime(&t);
        std::stringstream ss;
        ss << (tm.tm_hour < 10 ? "0" : "") << tm.tm_hour << ":" << (tm.tm_min < 10 ? "0" : "") << tm.tm_min << ":" << (tm.tm_sec < 10 ? "0" : "") << tm.tm_sec;
        return ss.str();
    }

    void playSound(int freq, int duration) {
        Beep(freq, duration);
    }

    std::string center(const std::string& text, int width) {
        int len = (int)text.length();
        if (len >= width) return text;
        int left = (width - len) / 2;
        int right = width - len - left;
        return std::string(left, ' ') + text + std::string(right, ' ');
    }

    void typeWrite(const std::string& text, int speed) {
        for (char c : text) {
            std::cout << c << std::flush;
            Sleep(speed);
        }
    }

    void bootSequence() {
        clear();
        gotoxy(2, 2); std::cout << GREEN; typeWrite("BIOS CHECK ................. OK", 10);
        gotoxy(2, 3); typeWrite("LOADING KERNEL ............. OK", 10);
        gotoxy(2, 4); typeWrite("MOUNTING FILESYSTEM ........ OK", 10);
        gotoxy(2, 5); typeWrite("INITIALIZING GRAPHICS ...... OK", 10);
        
        gotoxy(2, 7);
        std::cout << "MEMORY: ";
        for(int i=0; i<=4096; i+=128) {
            std::cout << "\rMEMORY: " << i << " MB OK" << std::flush;
            Sleep(5);
        }
        std::cout << RESET << std::endl;
        Sleep(500);
    }

    void updateTheme() {
        std::time_t now = std::time(nullptr);
        std::tm *ltm = std::localtime(&now);
        if (ltm->tm_hour >= 18 || ltm->tm_hour < 6) {
            // Night Mode (Neon Purple/Blue)
            ORANGE = "\033[38;5;57m";       // Deep Purple
            BRIGHT_ORANGE = "\033[38;5;129m"; // Bright Purple
            YELLOW = "\033[38;5;51m";       // Cyan
            CYAN = "\033[38;5;201m";        // Magenta
            RED = "\033[38;5;160m";         // Deep Red
        } else {
            // Day Mode (Original)
            ORANGE = "\033[38;5;208m";
            BRIGHT_ORANGE = "\033[38;5;214m";
            YELLOW = "\033[38;5;226m";
            CYAN = "\033[38;5;51m";
            RED = "\033[38;5;196m";
        }
    }

    void drawTicker(int frame) {
        // Dimensions & Layout
        int yTop = CONSOLE_HEIGHT - 2;
        int yMid = CONSOLE_HEIGHT - 1;
        int yBot = CONSOLE_HEIGHT;

        int sep1 = CONTENT_WIDTH - 75; // Meteo starts
        int sep2 = CONTENT_WIDTH - 45; // Media starts

        // Helper to print repeated string (for UTF-8 safety)
        auto rep = [](const std::string& s, int n) { for(int i=0; i<n; i++) std::cout << s; };

        // --- Draw Top Border ---
        gotoxy(1, yTop);
        std::cout << ORANGE << "╔";
        
        // News Section Top
        std::string t1 = "[ LIVE NEWS ]";
        int pad1 = (sep1 - 2 - (int)t1.length()) / 2;
        rep("═", pad1); std::cout << t1; rep("═", sep1 - 2 - pad1 - (int)t1.length());
        
        std::cout << "╦";

        // Meteo Section Top
        std::string t2 = "[ METEO ]";
        int w2 = sep2 - sep1 - 1;
        int pad2 = (w2 - (int)t2.length()) / 2;
        rep("═", pad2); std::cout << t2; rep("═", w2 - pad2 - (int)t2.length());

        std::cout << "╦";

        // Media Section Top
        std::string t3 = "[ MEDIA ]";
        int w3 = CONTENT_WIDTH - sep2 - 1;
        int pad3 = (w3 - (int)t3.length()) / 2;
        rep("═", pad3); std::cout << t3; rep("═", w3 - pad3 - (int)t3.length());

        std::cout << "╗" << RESET;

        // --- Draw Middle Line (Separators) ---
        gotoxy(1, yMid); std::cout << ORANGE << "║" << RESET;
        gotoxy(sep1, yMid); std::cout << ORANGE << "║" << RESET;
        gotoxy(sep2, yMid); std::cout << ORANGE << "║" << RESET;
        gotoxy(CONTENT_WIDTH, yMid); std::cout << ORANGE << "║" << RESET;

        // --- Draw Bottom Border ---
        gotoxy(1, yBot);
        std::cout << ORANGE << "╚";
        rep("═", sep1 - 1); std::cout << "╩";
        rep("═", sep2 - sep1 - 1); std::cout << "╩";
        rep("═", CONTENT_WIDTH - sep2 - 1); std::cout << "╝" << RESET;

        // --- Content: Live News ---
        std::string news = "Welcome to Hotel Deluxe *** Special Offer: 20% off Spa treatments *** Weather: Sunny 25C *** System Status: ONLINE *** Use Mouse or Arrow Keys to Navigate *** ";
        int newsW = sep1 - 2;
        int len = (int)news.length();
        int offset = (frame / 2) % len; 
        
        std::string display = news.substr(offset) + news.substr(0, offset);
        if ((int)display.length() > newsW) display = display.substr(0, newsW);
        gotoxy(2, yMid); std::cout << WHITE << display << RESET;

        // --- Content: Meteo ---
        std::time_t now = std::time(nullptr);
        std::tm *ltm = std::localtime(&now);
        bool isNight = (ltm->tm_hour >= 18 || ltm->tm_hour < 6);
        std::string meteoText = isNight ? "18C CLEAR SKY" : "25C SUNNY";
        gotoxy(sep1 + 1, yMid); std::cout << WHITE << center(meteoText, sep2 - sep1 - 1) << RESET;

        // --- Content: Media Player ---

        static bool musicLoaded = false;
        if (!musicLoaded) { loadMusic(); musicLoaded = true; }

        // Controls
        std::string prevBtn = "[<<]";
        std::string playBtn = (isMusicPlaying && !isMusicPaused) ? "[||]" : "[|>]";
        std::string nextBtn = "[>>]";
        
        // Highlight based on focus
        if (mediaButtonFocus == 0) prevBtn = INVERSE + prevBtn + RESET + YELLOW;
        if (mediaButtonFocus == 1) playBtn = INVERSE + playBtn + RESET + YELLOW;
        if (mediaButtonFocus == 2) nextBtn = INVERSE + nextBtn + RESET + YELLOW;
        
        std::string waves = "";
        const std::string bars[] = {" ", "▂", "▃", "▄", "▅", "▆", "▇", "█"};
        if (isMusicPlaying && !isMusicPaused) {
            for(int i=0; i<5; i++) {
                int h = rand() % 8;
                waves += bars[h];
            }
        } else {
            waves = "_____";
        }

        std::string title = playlist.empty() ? "No MP3s" : playlist[currentSongIndex];
        if (title.length() > 12) title = title.substr(0, 9) + "...";
        
        gotoxy(sep2 + 2, yMid);
        std::cout << WHITE << std::left << std::setw(13) << title << RESET;
        std::cout << CYAN << waves << "  " << RESET;
        std::cout << YELLOW << prevBtn << " " << playBtn << " " << nextBtn << RESET;
    }

    void drawBigClock(int x, int y) {
        static const std::vector<std::string> DIGITS[11] = {
            { "  ___  ", " / _ \\ ", "| | | |", "| |_| |", " \\___/ " },
            { "   _   ", "  / |  ", "  | |  ", "  | |  ", "  |_|  " },
            { "  ___  ", " |_  ) ", "  / /  ", " /___| ", "|_____|" },
            { "  ___  ", " |__ / ", "  |_ \\ ", " |___/ ", "|____/ " },
            { "  _ _  ", " | | | ", " |_  _|", "   | | ", "   |_| " },
            { "  ___  ", " | __| ", " |__ \\ ", " |___/ ", "|____/ " },
            { "  __   ", " / /_  ", "| '_ \\ ", "| (_) |", " \\___/ " },
            { " ____  ", "|__  | ", "  / /  ", " /_/   ", "/_/    " },
            { "  ___  ", " ( _ ) ", " / _ \\ ", "| (_) |", " \\___/ " },
            { "  ___  ", " / _ \\ ", "| (_) |", "  \\__, |", "    /_/ " },
            { "   ", " _ ", "(_)", " _ ", "(_)" }
        };

        std::time_t t = std::time(nullptr);
        std::tm tm = *std::localtime(&t);
        
        int nums[] = {
            tm.tm_hour / 10, tm.tm_hour % 10,
            10,
            tm.tm_min / 10, tm.tm_min % 10,
            10,
            tm.tm_sec / 10, tm.tm_sec % 10
        };

        for (int row = 0; row < 5; ++row) {
            gotoxy(x, y + row);
            std::cout << YELLOW << BOLD;
            for (int i = 0; i < 8; ++i) {
                std::cout << DIGITS[nums[i]][row];
            }
            std::cout << RESET;
        }
    }

    // Draw the main static layout (Borders, Sidebar container)
    void drawLayout(const std::string& title, const std::vector<std::string>& sidebar = {}) {
        clear();
        
        // --- Main Content Border ---
        gotoxy(1, 1);
        std::cout << ORANGE << "╔" << std::string(CONTENT_WIDTH - 2, '═') << "╗";
        for(int y=2; y<CONSOLE_HEIGHT; y++) {
            gotoxy(1, y); std::cout << "║";
            gotoxy(CONTENT_WIDTH, y); std::cout << "║";
        }
        gotoxy(1, CONSOLE_HEIGHT);
        std::cout << "╚" << std::string(CONTENT_WIDTH - 2, '═') << "╝";

        // --- Title ---
        gotoxy(1, 3);
        std::cout << "║" << BRIGHT_ORANGE << BOLD << center(title, CONTENT_WIDTH - 2) << RESET << ORANGE << "║";
        gotoxy(1, 4);
        std::cout << "╠" << std::string(CONTENT_WIDTH - 2, '═') << "╣";

        // --- Sidebar (Right) ---
        int sbX = CONTENT_WIDTH + 1;
        gotoxy(sbX, 1);
        std::cout << ORANGE << "╔" << std::string(SIDEBAR_WIDTH - 2, '═') << "╗";
        for(int y=2; y<CONSOLE_HEIGHT; y++) {
            gotoxy(sbX, y); std::cout << "║";
            gotoxy(CONSOLE_WIDTH, y); std::cout << "║";
        }
        gotoxy(sbX, CONSOLE_HEIGHT);
        std::cout << "╚" << std::string(SIDEBAR_WIDTH - 2, '═') << "╝";

        // --- Clock ---
        drawBigClock(sbX + 6, 2);
        gotoxy(sbX, 8);
        std::cout << ORANGE << "╠" << std::string(SIDEBAR_WIDTH - 2, '═') << "╣";

        // --- Sidebar Items ---
        // Parse Sidebar Data for Grid View
        struct RoomGridItem { std::string id; bool free; std::string type; };
        std::vector<RoomGridItem> rooms;
        
        for(const auto& item : sidebar) {
            if (item.find(":") != std::string::npos) {
                // Format: 101:FREE:Type
                size_t firstColon = item.find(":");
                size_t secondColon = item.find(":", firstColon + 1);
                
                std::string num = item.substr(0, firstColon);
                bool isFree = item.find("FREE") != std::string::npos;
                std::string typeStr = (secondColon != std::string::npos) ? item.substr(secondColon + 1) : "Simple";
                
                std::string typeCode = "Si";
                if (typeStr == "Double") typeCode = "D";
                else if (typeStr == "Suite") typeCode = "S";
                else if (typeStr == "Presid.") typeCode = "P";
                
                rooms.push_back({num, isFree, typeCode});
            }
        }

        // Pagination Logic
        int itemsPerPage = 9;
        int totalPages = (rooms.empty()) ? 1 : (rooms.size() + itemsPerPage - 1) / itemsPerPage;
        if (sidebarPage >= totalPages) sidebarPage = totalPages - 1;
        if (sidebarPage < 0) sidebarPage = 0;

        int startIdx = sidebarPage * itemsPerPage;
        int endIdx = min((int)rooms.size(), startIdx + itemsPerPage);

        // Render Grid (3x3)
        int startY = 10;
        int gridX = sbX + 4;
        int col = 0;
        int row = 0;

        for(int i = startIdx; i < endIdx; ++i) {
            int x = gridX + (col * 18); // Spacing
            int y = startY + (row * 5);
            
            std::string color = rooms[i].free ? CYAN : RED;
            std::string borderColor = rooms[i].free ? CYAN : RED;
            
            gotoxy(x, y);     std::cout << borderColor << "┌──────────┐" << RESET;
            gotoxy(x, y + 1); std::cout << borderColor << "│ " << WHITE << center(rooms[i].id, 8) << borderColor << " │" << RESET;
            gotoxy(x, y + 2); std::cout << borderColor << "│ " << YELLOW << center(rooms[i].type, 8) << borderColor << " │" << RESET;
            gotoxy(x, y + 3); std::cout << borderColor << "└──────────┘" << RESET;

            col++;
            if (col >= 3) {
                col = 0;
                row++;
            }
        }

        // Render Arrows
        int arrowY = startY + (3 * 5) + 1; // Below grid
        gotoxy(sbX + 2, arrowY);
        if (sidebarPage > 0) {
            std::cout << YELLOW << "◄ PREV" << RESET;
        } else {
            std::cout << "      ";
        }

        gotoxy(sbX + SIDEBAR_WIDTH - 10, arrowY);
        if (sidebarPage < totalPages - 1) {
            std::cout << YELLOW << "NEXT ►" << RESET;
        }
    }

    void drawInfoBox(const std::vector<std::string>& lines) {
        int boxWidth = 60;
        int startX = (CONTENT_WIDTH - boxWidth) / 2;
        int startY = 10;

        gotoxy(startX, startY++);
        std::cout << ORANGE << "┌" << std::string(boxWidth - 2, '─') << "┐" << RESET;
        for (const auto& line : lines) {
            gotoxy(startX, startY++);
            std::cout << ORANGE << "│ " << WHITE << center(line, boxWidth - 4) << ORANGE << " │" << RESET;
        }
        gotoxy(startX, startY);
        std::cout << ORANGE << "└" << std::string(boxWidth - 2, '─') << "┘" << RESET;
    }

    // Draw a modal box (clears area first)
    void drawModal(const std::string& title, int height) {
        int width = 60;
        int startX = (CONTENT_WIDTH - width) / 2;
        int startY = (CONSOLE_HEIGHT - height) / 2;

        // Clear area
        for(int i=0; i<height; i++) {
            gotoxy(startX, startY + i);
            std::cout << std::string(width, ' ');
        }

        gotoxy(startX, startY);
        std::cout << ORANGE << "╔" << std::string(width - 2, '═') << "╗" << RESET;
        gotoxy(startX, startY + 1);
        std::cout << ORANGE << "║" << WHITE << BOLD << center(title, width - 2) << RESET << ORANGE << "║" << RESET;
        gotoxy(startX, startY + 2);
        std::cout << ORANGE << "╠" << std::string(width - 2, '═') << "╣" << RESET;

        for(int i=3; i<height-1; i++) {
            gotoxy(startX, startY + i);
            std::cout << ORANGE << "║" << std::string(width - 2, ' ') << "║" << RESET;
        }
        gotoxy(startX, startY + height - 1);
        std::cout << ORANGE << "╚" << std::string(width - 2, '═') << "╝" << RESET;
    }

    void drawTable(const std::vector<std::string>& rows) {
        int y = 6;
        for(const auto& row : rows) {
            if (y > CONSOLE_HEIGHT - 3) break;
            gotoxy(4, y++);
            std::cout << WHITE << row << RESET;
        }
    }

    struct RoomData {
        int id;
        std::string type;
        bool free;
    };

    void drawFloorPlan(const std::vector<RoomData>& rooms) {
        std::string BLOCK = "\xE2\x96\x88";
        std::string BLOCK_TOP = "\xE2\x96\x80";
        auto rep = [](const std::string& s, int n) { for(int i=0; i<n; i++) std::cout << s; };

        // Group by floor (e.g., 101 -> Floor 1)
        std::map<int, std::vector<RoomData>> floors;
        for (const auto& r : rooms) {
            int floor = r.id / 100;
            floors[floor].push_back(r);
        }

        int y = 6; // Start below header
        
        // Iterate floors in descending order (Top floor first)
        for (auto it = floors.rbegin(); it != floors.rend(); ++it) {
            int floorNum = it->first;
            const auto& floorRooms = it->second;

            if (y + 15 > CONSOLE_HEIGHT - 3) break; // Safety check

            int mapX = 4;
            
            // Draw Floor Header
            gotoxy(mapX, y++);
            std::cout << ORANGE << BLOCK << " FLOOR " << floorNum << " ";
            rep(BLOCK_TOP, 80);
            std::cout << RESET;

            // Visual Map Template (Blueprint Style)
            std::vector<std::string> mapBase = {
                "   ╔══════╗    ┌──────────┐  ┌──────────┐  ┌────────────────────┐  ┌──────────┐   ",
                "   ║STAIRS║    │          │  │          │  │                    │  │          │   ",
                "   ║      ║    │          │  │          │  │                    │  │          │   ",
                "   ║      ║    │          │  │          │  │                    │  │          │   ",
                "   ╚══════╝    └────┬┬────┘  └────┬┬────┘  └─────────┬┬─────────┘  └────┬┬────┘   ",
                "                    ││            ││                 ││                 ││        ",
                "   ◄ H A L L ───────┴┴────────────┴┴─────────────────┴┴─────────────────┴┴──────► ",
                "                    ││            ││                 ││                 ││        ",
                "   ┌──────┐    ┌────┴┴────┐  ┌────┴┴────┐  ┌─────────┴┴─────────┐  ┌────┴┴────┐   ",
                "   │  WC  │    │          │  │          │  │                    │  │          │   ",
                "   │      │    │          │  │          │  │                    │  │          │   ",
                "   │      │    │          │  │          │  │                    │  │          │   ",
                "   └──────┘    └──────────┘  └──────────┘  └────────────────────┘  └──────────┘   "
            };

            for(int i=0; i<mapBase.size(); i++) {
                gotoxy(mapX, y + i);
                std::cout << DARK_GRAY << mapBase[i] << RESET;
            }

            // Define Room Slots: {suffix, relX, relY, width, height}
            struct Slot { int suffix; int x; int y; int w; int h; };
            std::vector<Slot> slots = {
                {1, 16, 1, 10, 3}, {2, 30, 1, 10, 3}, {3, 44, 1, 20, 3}, {4, 68, 1, 10, 3}, // Top Row
                {5, 16, 9, 10, 3}, {6, 30, 9, 10, 3}, {7, 44, 9, 20, 3}, {8, 68, 9, 10, 3}  // Bottom Row
            };

            for (const auto& r : floorRooms) {
                int suffix = r.id % 100;
                for (const auto& s : slots) {
                    if (s.suffix == suffix) {
                        std::string color = r.free ? CYAN : RED;
                        std::string borderColor = r.free ? CYAN : RED;
                        
                        // Draw Box Borders (Overwriting map base)
                        gotoxy(mapX + s.x - 1, y + s.y - 1); std::cout << borderColor << "┌" << std::string(s.w, '─') << "┐";
                        for(int k=0; k<s.h; k++) {
                            gotoxy(mapX + s.x - 1, y + s.y + k); std::cout << borderColor << "│";
                            gotoxy(mapX + s.x + s.w, y + s.y + k); std::cout << borderColor << "│";
                        }
                        gotoxy(mapX + s.x - 1, y + s.y + s.h); std::cout << borderColor << "└" << std::string(s.w, '─') << "┘";

                        // Draw Info
                        gotoxy(mapX + s.x, y + s.y);     std::cout << color << BOLD << center(std::to_string(r.id), s.w) << RESET;
                        gotoxy(mapX + s.x, y + s.y + 1); std::cout << WHITE << center(r.type.substr(0, s.w), s.w) << RESET;
                        gotoxy(mapX + s.x, y + s.y + 2); std::cout << (r.free ? GREEN : RED) << center(r.free ? "FREE" : "BUSY", s.w) << RESET;
                    }
                }
            }
            y += 15; // Space between floors
        }
    }

    struct ParkingData {
        int id;
        bool free;
    };

    void drawParkingMap(const std::vector<ParkingData>& spots) {
        std::string BLOCK = "\xE2\x96\x88";
        std::string BLOCK_TOP = "\xE2\x96\x80";
        auto rep = [](const std::string& s, int n) { for(int i=0; i<n; i++) std::cout << s; };

        int y = 6;
        int mapX = 4;

        // Header
        gotoxy(mapX, y++);
        std::cout << ORANGE << BLOCK << " PARKING LOT " << std::string(70, ' ') << RESET;
        gotoxy(mapX + 14, y-1); rep(BLOCK_TOP, 80);

        // Map Base
        std::vector<std::string> mapBase = {
            "   ╔════════════════════════════════════════════════════════════════════════════════════════════╗   ",
            "   ║  ENTRY  ►      ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐        ◄ EXIT   ║   ",
            "   ║                │   │ │   │ │   │ │   │ │   │ │   │ │   │ │   │ │   │ │   │                 ║   ",
            "   ║                └───┘ └───┘ └───┘ └───┘ └───┘ └───┘ └───┘ └───┘ └───┘ └───┘                 ║   ",
            "   ║                                                                                            ║   ",
            "   ║                ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐                 ║   ",
            "   ║                │   │ │   │ │   │ │   │ │   │ │   │ │   │ │   │ │   │ │   │                 ║   ",
            "   ║                └───┘ └───┘ └───┘ └───┘ └───┘ └───┘ └───┘ └───┘ └───┘ └───┘                 ║   ",
            "   ║                                                                                            ║   ",
            "   ║                ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐                 ║   ",
            "   ║                │   │ │   │ │   │ │   │ │   │ │   │ │   │ │   │ │   │ │   │                 ║   ",
            "   ║                └───┘ └───┘ └───┘ └───┘ └───┘ └───┘ └───┘ └───┘ └───┘ └───┘                 ║   ",
            "   ║                                                                                            ║   ",
            "   ║                ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐                 ║   ",
            "   ║                │   │ │   │ │   │ │   │ │   │ │   │ │   │ │   │ │   │ │   │                 ║   ",
            "   ║                └───┘ └───┘ └───┘ └───┘ └───┘ └───┘ └───┘ └───┘ └───┘ └───┘                 ║   ",
            "   ║                                                                                            ║   ",
            "   ║                ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐                 ║   ",
            "   ║                │   │ │   │ │   │ │   │ │   │ │   │ │   │ │   │ │   │ │   │                 ║   ",
            "   ║                └───┘ └───┘ └───┘ └───┘ └───┘ └───┘ └───┘ └───┘ └───┘ └───┘                 ║   ",
            "   ╚════════════════════════════════════════════════════════════════════════════════════════════╝   "
        };

        for(int i=0; i<mapBase.size(); i++) {
            gotoxy(mapX, y + i);
            std::cout << DARK_GRAY << mapBase[i] << RESET;
        }

        // Slots: {id, x, y}
        struct PSlot { int id; int x; int y; };
        std::vector<PSlot> pslots = {
            {1, 20, 2}, {2, 26, 2}, {3, 32, 2}, {4, 38, 2}, {5, 44, 2}, {6, 50, 2}, {7, 56, 2}, {8, 62, 2}, {9, 68, 2}, {10, 74, 2},
            
            {11, 20, 6}, {12, 26, 6}, {13, 32, 6}, {14, 38, 6}, {15, 44, 6}, {16, 50, 6}, {17, 56, 6}, {18, 62, 6}, {19, 68, 6}, {20, 74, 6},
            
            {21, 20, 10}, {22, 26, 10}, {23, 32, 10}, {24, 38, 10}, {25, 44, 10}, {26, 50, 10}, {27, 56, 10}, {28, 62, 10}, {29, 68, 10}, {30, 74, 10},
            
            {31, 20, 14}, {32, 26, 14}, {33, 32, 14}, {34, 38, 14}, {35, 44, 14}, {36, 50, 14}, {37, 56, 14}, {38, 62, 14}, {39, 68, 14}, {40, 74, 14},
            
            {41, 20, 18}, {42, 26, 18}, {43, 32, 18}, {44, 38, 18}, {45, 44, 18}, {46, 50, 18}, {47, 56, 18}, {48, 62, 18}, {49, 68, 18}, {50, 74, 18}
        };

        for (const auto& s : spots) {
            for (const auto& ps : pslots) {
                if (ps.id == s.id) {
                    std::string color = s.free ? GREEN : RED;
                    // Draw Box with Status
                    gotoxy(mapX + ps.x, y + ps.y - 1); std::cout << color << "┌" << (s.id < 10 ? "0" : "") << s.id << "┐" << RESET;
                    gotoxy(mapX + ps.x, y + ps.y);     std::cout << color << "│" << (s.free ? "Fr" : "Bs") << "│" << RESET;
                    gotoxy(mapX + ps.x, y + ps.y + 1); std::cout << color << "└───┘" << RESET;
                }
            }
        }
    }

    struct TableData {
        int id;
        bool free;
    };

    void drawRestaurantMap(const std::vector<TableData>& tables) {
        std::string BLOCK = "\xE2\x96\x88";
        std::string BLOCK_TOP = "\xE2\x96\x80";
        auto rep = [](const std::string& s, int n) { for(int i=0; i<n; i++) std::cout << s; };

        int y = 6;
        int mapX = 4;

        gotoxy(mapX, y++);
        std::cout << ORANGE << BLOCK << " RESTAURANT " << std::string(70, ' ') << RESET;
        gotoxy(mapX + 13, y-1); rep(BLOCK_TOP, 81);

        std::vector<std::string> mapBase = {
            "   ╔════════════════════════════════════════════════════════════════════════════════════════════╗   ",
            "   ║   KITCHEN      ┌──────┐    ┌──────┐    ┌──────┐    ┌──────┐    ┌──────┐       BAR          ║   ",
            "   ║   [======]     │      │    │      │    │      │    │      │    │      │      [====]        ║   ",
            "   ║                └──────┘    └──────┘    └──────┘    └──────┘    └──────┘                    ║   ",
            "   ║                                                                                            ║   ",
            "   ║                ┌──────┐    ┌──────┐    ┌──────┐    ┌──────┐    ┌──────┐      ╔════╗        ║   ",
            "   ║   ENTRANCE     │      │    │      │    │      │    │      │    │      │      ║ DJ ║        ║   ",
            "   ║      ==>       └──────┘    └──────┘    └──────┘    └──────┘    └──────┘      ╚════╝        ║   ",
            "   ║                                                                                            ║   ",
            "   ║                ┌──────┐    ┌──────┐    ┌──────┐    ┌──────┐    ┌──────┐                    ║   ",
            "   ║                │      │    │      │    │      │    │      │    │      │                    ║   ",
            "   ║                └──────┘    └──────┘    └──────┘    └──────┘    └──────┘                    ║   ",
            "   ║                                                                                            ║   ",
            "   ║                ┌──────┐    ┌──────┐    ┌──────┐    ┌──────┐    ┌──────┐                    ║   ",
            "   ║                │      │    │      │    │      │    │      │    │      │                    ║   ",
            "   ║                └──────┘    └──────┘    └──────┘    └──────┘    └──────┘                    ║   ",
            "   ╚════════════════════════════════════════════════════════════════════════════════════════════╝   "
        };

        for(int i=0; i<mapBase.size(); i++) {
            gotoxy(mapX, y + i);
            std::cout << DARK_GRAY << mapBase[i] << RESET;
        }

        struct TSlot { int id; int x; int y; };
        std::vector<TSlot> tslots = {
            {1, 20, 1}, {2, 32, 1}, {3, 44, 1}, {4, 56, 1}, {5, 68, 1},
            {6, 20, 5}, {7, 32, 5}, {8, 44, 5}, {9, 56, 5}, {10, 68, 5},
            {11, 20, 9}, {12, 32, 9}, {13, 44, 9}, {14, 56, 9}, {15, 68, 9},
            {16, 20, 13}, {17, 32, 13}, {18, 44, 13}, {19, 56, 13}, {20, 68, 13},
            {21, 20, 17}, {22, 32, 17}, {23, 44, 17}, {24, 56, 17}
        };

        for (const auto& t : tables) {
            for (const auto& ts : tslots) {
                if (ts.id == t.id) {
                    std::string color = t.free ? CYAN : RED;
                    gotoxy(mapX + ts.x, y + ts.y);     std::cout << color << "┌──" << (t.id < 10 ? "0" : "") << t.id << "──┐" << RESET;
                    gotoxy(mapX + ts.x, y + ts.y + 1); std::cout << color << "│ " << (t.free ? "FREE" : "BUSY") << " │" << RESET;
                    gotoxy(mapX + ts.x, y + ts.y + 2); std::cout << color << "└──────┘" << RESET;
                }
            }
        }
    }

    void drawEntranceMap() {
        std::string BLOCK = "\xE2\x96\x88";
        std::string BLOCK_TOP = "\xE2\x96\x80";
        auto rep = [](const std::string& s, int n) { for(int i=0; i<n; i++) std::cout << s; };

        int y = 6;
        int mapX = 4;

        gotoxy(mapX, y++);
        std::cout << ORANGE << BLOCK << " HOTEL ENTRANCE " << std::string(70, ' ') << RESET;
        gotoxy(mapX + 17, y-1); rep(BLOCK_TOP, 77);

        std::vector<std::string> mapBase = {
            "   ╔════════════════════════════════════════════════════════════════════════════════════════════╗   ",
            "   ║                  HOTEL DELUXE MAIN ENTRANCE                                                ║   ",
            "   ║      ┌──────┐                                                        ┌──────┐              ║   ",
            "   ║      │ VALET│            ================================            │ TAXI │              ║   ",
            "   ║      └──────┘            |        RECEPTION DESK        |            └──────┘              ║   ",
            "   ║                          ================================                                  ║   ",
            "   ║                                                                                            ║   ",
            "   ║       [  ]                    [  ]      [  ]                    [  ]                       ║   ",
            "   ║      PLANT                   LOUNGE    LOUNGE                  PLANT                       ║   ",
            "   ║                                                                                            ║   ",
            "   ║                       /  /  /  /  /  /  /  /  /  /  /  /                                   ║   ",
            "   ║                      /  /  /  /  STAIRS /  /  /  /  /  /                                   ║   ",
            "   ╚════════════════════════════════════════════════════════════════════════════════════════════╝   "
        };

        for(int i=0; i<mapBase.size(); i++) {
            gotoxy(mapX, y + i);
            std::cout << WHITE << mapBase[i] << RESET;
        }
    }

    void drawWorld(int x, int y, int frame) {
        float t = frame * 0.2f; // Time/Speed
        
        // Path: Circle
        float r = 0.6f;
        float dx = r * cos(t);
        float dz = r * sin(t) * 0.5f; // Flattened circle (ellipse) for perspective
        
        // Orientation: Face tangent of movement
        // Tangent angle of (cos t, sin t) is t + pi/2
        float angle = t + 1.57f;

        // Bobbing (Walking steps)
        float bob = 0.05f * fabs(sin(t * 4.0f));
        
        float cosA = cos(angle);
        float sinA = sin(angle);
        
        // Tilt for better view
        float tilt = 0.3f;
        float cosTilt = cos(tilt);
        float sinTilt = sin(tilt);
        
        int width = 56;
        int height = 24;
        
        std::vector<char> buffer(width * height, ' ');
        std::vector<float> zBuffer(width * height, -100.0f);
        
        auto plot = [&](float mx, float my, float mz, char c) {
            // 1. Rotate Model (Local Rotation)
            float rx = mx * cosA - mz * sinA;
            float ry = my;
            float rz = mx * sinA + mz * cosA;
            
            // 2. Translate (World Position) + Bobbing
            float wx = rx + dx;
            float wy = ry + bob;
            float wz = rz + dz;
            
            // 3. Camera Tilt
            float cx = wx;
            float cy = wy * cosTilt - wz * sinTilt;
            float cz = wy * sinTilt + wz * cosTilt;
            
            // 4. Project
            int xp = (width / 2) + (int)(cx * 14.0f);
            int yp = (height / 2) - (int)(cy * 7.0f) + 1;
            
            if (xp >= 0 && xp < width && yp >= 0 && yp < height) {
                int idx = yp * width + xp;
                if (cz > zBuffer[idx]) {
                    zBuffer[idx] = cz;
                    buffer[idx] = c;
                }
            }
        };

        // Body
        for(float i=0; i<6.28; i+=0.15) {
            for(float j=0; j<3.14; j+=0.15) {
                float bx = 1.1f * sin(j) * cos(i);
                float by = 0.6f * sin(j) * sin(i);
                float bz = 0.7f * cos(j);
                plot(bx, by, bz, '#');
            }
        }
        
        // Head
        for(float i=0; i<6.28; i+=0.2) {
            for(float j=0; j<3.14; j+=0.2) {
                float hx = 0.45f * sin(j) * cos(i);
                float hy = 0.45f * sin(j) * sin(i);
                float hz = 0.45f * cos(j);
                plot(hx + 0.8f, hy + 0.7f, hz, '@');
            }
        }
        
        // Beak
        for(float r=0; r<0.3; r+=0.05) {
            plot(1.3f + r, 0.6f, 0.0f, '>');
        }

        // Tail
        for(float i=0; i<6.28; i+=0.3) {
            plot(-1.2f, 0.2f + 0.2f*cos(i), 0.2f*sin(i), '*');
        }

        // Wings
        for(float i=0; i<6.28; i+=0.2) {
            for(float j=0; j<3.14; j+=0.2) {
                float wx = 0.5f * sin(j) * cos(i);
                float wy = 0.2f * sin(j) * sin(i);
                float wz = 0.1f * cos(j);
                plot(wx, wy, wz + 0.75f, '='); // Left Wing
                plot(wx, wy, wz - 0.75f, '='); // Right Wing
            }
        }
        
        // Legs (Simple swinging lines)
        float legSwing = sin(t * 4.0f) * 0.2f;
        // Leg 1
        plot(0.0f + legSwing, -0.5f, 0.2f, '|');
        plot(0.0f + legSwing, -0.6f, 0.2f, '\'');
        // Leg 2
        plot(0.0f - legSwing, -0.5f, -0.2f, '|');
        plot(0.0f - legSwing, -0.6f, -0.2f, '\'');
        
        // Render
        for(int row=0; row<height; row++) {
            gotoxy(x, y + row);
            for(int col=0; col<width; col++) {
                char pixel = buffer[row * width + col];
                std::cout << (pixel == ' ' ? " " : (YELLOW + std::string(1, pixel) + RESET));
            }
        }
    }

    void showMapViewer(const std::vector<RoomData>& rooms, const std::vector<ParkingData>& parking, const std::vector<TableData>& tables) {
        int currentMap = 0; // 0: Floor, 1: Parking, 2: Restaurant, 3: Entrance
        const int TOTAL_MAPS = 4;
        bool inMapMode = true;
        HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
        INPUT_RECORD ir[128];
        DWORD nRead;
        bool needRedraw = true;
        int frameCounter = 0;

        FlushConsoleInputBuffer(hIn);

        while (inMapMode) {
            if (needRedraw) {
                std::string title;
                switch(currentMap) {
                    case 0: title = "HOTEL FLOOR PLAN"; break;
                    case 1: title = "PARKING LOT"; break;
                    case 2: title = "RESTAURANT & BAR"; break;
                    case 3: title = "MAIN ENTRANCE"; break;
                }
                
                drawLayout(title);

                switch(currentMap) {
                    case 0: drawFloorPlan(rooms); break;
                    case 1: drawParkingMap(parking); break;
                    case 2: drawRestaurantMap(tables); break;
                    case 3: drawEntranceMap(); break;
                }
                
                gotoxy(4, CONSOLE_HEIGHT - 4);
                std::cout << YELLOW << "◄ PREV MAP (Left Arrow)   |   NEXT MAP (Right Arrow) ►   |   EXIT (Esc/Enter)" << RESET;

                needRedraw = false;
            }

            // Background animations
            drawBigClock(CONTENT_WIDTH + 1 + 6, 2);
            drawWorld(CONTENT_WIDTH + 1 + (SIDEBAR_WIDTH - 56)/2, CONSOLE_HEIGHT - 20, frameCounter++);
            drawTicker(frameCounter);

            if (WaitForSingleObject(hIn, 50) == WAIT_OBJECT_0) {
                ReadConsoleInput(hIn, ir, 128, &nRead);
                for (DWORD i = 0; i < nRead; i++) {
                    if (ir[i].EventType == KEY_EVENT && ir[i].Event.KeyEvent.bKeyDown) {
                        WORD key = ir[i].Event.KeyEvent.wVirtualKeyCode;
                        if (key == VK_LEFT) {
                            currentMap = (currentMap - 1 + TOTAL_MAPS) % TOTAL_MAPS;
                            playSound(400, 20);
                            needRedraw = true;
                        } else if (key == VK_RIGHT) {
                            currentMap = (currentMap + 1) % TOTAL_MAPS;
                            playSound(400, 20);
                            needRedraw = true;
                        } else if (key == VK_ESCAPE || key == VK_RETURN) {
                            inMapMode = false;
                            playSound(600, 50);
                        }
                    }
                }
            }
        }
    }

    // Helper for inputs
    std::string textInput(const std::string& label, int y) {
        int startX = (CONTENT_WIDTH - 40) / 2;
        gotoxy(startX, y);
        std::cout << ORANGE << label << ": " << YELLOW;
        
        // Show cursor
        std::cout << "\033[?25h"; 
        std::string input;
        std::cin >> input;
        std::cout << "\033[?25l"; // Hide cursor
        
        return input;
    }

    int showMenu(const std::string& title, const std::vector<std::string>& options, const std::vector<std::string>& sidebar = {}) {
        int selected = 0;
        HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
        INPUT_RECORD ir[128];
        DWORD nRead;
        bool needRedraw = true;
        int frameCounter = 0;
        mediaButtonFocus = -1; // Reset focus when entering menu

        FlushConsoleInputBuffer(hIn);
        
        while(true) {
            if (needRedraw) {
                drawLayout(title, sidebar);
                
                int menuStartY = (CONSOLE_HEIGHT - (options.size() * 2)) / 2;

                for(size_t i = 0; i < options.size(); ++i) {
                    gotoxy(1, menuStartY + (int)i * 2);
                    if((int)i == selected) {
                        std::cout << center(" " + options[i] + " ", CONTENT_WIDTH);
                        gotoxy((CONTENT_WIDTH - options[i].length())/2 - 2, menuStartY + (int)i * 2);
                        std::cout << YELLOW << "► " << INVERSE << options[i] << RESET;
                    } else {
                        std::cout << ORANGE << center(options[i], CONTENT_WIDTH) << RESET;
                    }
                }
                
                // Static instruction removed in favor of ticker
                
                needRedraw = false;
            } else if (!sidebar.empty()) {
                // Update just the clock if no full redraw needed
                drawBigClock(CONTENT_WIDTH + 1 + 6, 2);
                
                // Update Globe
                drawWorld(CONTENT_WIDTH + 1 + (SIDEBAR_WIDTH - 56)/2, CONSOLE_HEIGHT - 20, frameCounter++);
                
                // Update Ticker
                drawTicker(frameCounter);
            }

            // Wait for input with timeout for clock update
            if (WaitForSingleObject(hIn, 50) == WAIT_OBJECT_0) {
                ReadConsoleInput(hIn, ir, 128, &nRead);
                for (DWORD i = 0; i < nRead; i++) {
                    if (ir[i].EventType == KEY_EVENT && ir[i].Event.KeyEvent.bKeyDown) {
                        WORD key = ir[i].Event.KeyEvent.wVirtualKeyCode;
                        if (key == VK_UP) {
                            if (mediaButtonFocus != -1) {
                                // Leave media controls, go back to menu bottom
                                mediaButtonFocus = -1;
                                selected = static_cast<int>(options.size()) - 1;
                            } else {
                                selected--;
                                if (selected < 0) {
                                    // Wrap to media controls
                                    selected = -1;
                                    mediaButtonFocus = 1; // Default to Play button
                                }
                            }
                            playSound(400, 20);
                            needRedraw = true;
                        } else if (key == VK_DOWN) {
                            if (mediaButtonFocus != -1) {
                                // Wrap from media to top of menu
                                mediaButtonFocus = -1;
                                selected = 0;
                            } else {
                                selected++;
                                if (selected >= static_cast<int>(options.size())) {
                                    // Go to media controls
                                    selected = -1;
                                    mediaButtonFocus = 1;
                                }
                            }
                            playSound(400, 20);
                            needRedraw = true;
                        } else if (key == VK_RETURN) {
                            if (mediaButtonFocus != -1) {
                                // Execute Media Action
                                if (mediaButtonFocus == 0) { // Prev
                                    stopMusic();
                                    if (!playlist.empty()) currentSongIndex = (currentSongIndex - 1 + playlist.size()) % playlist.size();
                                    playMusic();
                                } else if (mediaButtonFocus == 1) { // Play/Pause
                                    togglePauseMusic();
                                } else if (mediaButtonFocus == 2) { // Next
                                    nextMusic();
                                }
                                playSound(400, 20);
                                needRedraw = true;
                            } else {
                                playSound(800, 50);
                                return selected + 1;
                            }
                        } else if (key == VK_RIGHT) {
                            if (mediaButtonFocus != -1) {
                                mediaButtonFocus = (mediaButtonFocus + 1) % 3;
                            } else {
                                sidebarPage++;
                            }
                            playSound(400, 20);
                            needRedraw = true;
                        } else if (key == VK_LEFT) {
                            if (mediaButtonFocus != -1) {
                                mediaButtonFocus = (mediaButtonFocus - 1 + 3) % 3;
                            } else {
                                sidebarPage--;
                            }
                            playSound(400, 20);
                            needRedraw = true;
                        }
                    } else if (ir[i].EventType == MOUSE_EVENT) {
                        MOUSE_EVENT_RECORD mer = ir[i].Event.MouseEvent;
                        int mouseRow = mer.dwMousePosition.Y;
                        int menuStartY = (CONSOLE_HEIGHT - (options.size() * 2)) / 2;
                        int optionIndex = (mouseRow - menuStartY) / 2;

                        if (optionIndex >= 0 && optionIndex < static_cast<int>(options.size())) {
                            if (selected != optionIndex) {
                                selected = optionIndex;
                                playSound(400, 20);
                                needRedraw = true;
                            }
                            if (mer.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
                                playSound(800, 50);
                                return selected + 1;
                            }
                        }
                        
                        // Check Sidebar Arrows
                        if (mer.dwMousePosition.X > CONTENT_WIDTH) {
                            int arrowY = 10 + (3 * 5) + 1; // Match drawLayout calculation
                            if (mer.dwMousePosition.Y == arrowY) {
                                if (mer.dwMousePosition.X < CONTENT_WIDTH + 15) {
                                    // Prev
                                    sidebarPage--;
                                    needRedraw = true;
                                } else if (mer.dwMousePosition.X > CONSOLE_WIDTH - 15) {
                                    // Next
                                    sidebarPage++;
                                    needRedraw = true;
                                }
                            }
                        }

                        // Check Media Player Controls
                        int mediaY = CONSOLE_HEIGHT - 1;
                        if (mer.dwMousePosition.Y == mediaY) {
                            int sep2 = CONTENT_WIDTH - 45;
                            // Layout: Title(13) + Waves(7) + Buttons
                            int btnStart = sep2 + 2 + 13 + 7; 
                            
                            int mx = mer.dwMousePosition.X;
                            if (mx >= btnStart && mx <= btnStart + 3) { // Prev
                                playSound(400, 20);
                                // prevMusic logic (simple decrement)
                                stopMusic();
                                if (!playlist.empty()) currentSongIndex = (currentSongIndex - 1 + playlist.size()) % playlist.size();
                                playMusic();
                                needRedraw = true;
                            } else if (mx >= btnStart + 5 && mx <= btnStart + 8) { // Play/Pause
                                playSound(400, 20); togglePauseMusic(); needRedraw = true;
                            } else if (mx >= btnStart + 10 && mx <= btnStart + 13) { // Next
                                playSound(400, 20); nextMusic(); needRedraw = true;
                            }
                        }
                    }
                }
            }
        }
    }
}

#endif