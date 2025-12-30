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

#pragma comment(lib, "user32.lib")

namespace RetroUI {

    // Layout Constants
    static int CONSOLE_WIDTH = 210;
    static int CONSOLE_HEIGHT = 58;
    static int SIDEBAR_WIDTH = 60;
    static int CONTENT_WIDTH = CONSOLE_WIDTH - SIDEBAR_WIDTH;

    const std::string RESET = "\033[0m";
    const std::string ORANGE = "\033[38;5;208m";      // Main Borders
    const std::string BRIGHT_ORANGE = "\033[38;5;214m"; // Highlights
    const std::string YELLOW = "\033[38;5;226m";      // Accents
    const std::string CYAN = "\033[38;5;51m";         // Contrast (Free rooms)
    const std::string RED = "\033[38;5;196m";         // Occupied
    const std::string WHITE = "\033[38;5;255m";       // Text
    const std::string BOLD = "\033[1m";
    const std::string INVERSE = "\033[7m";

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
        int y = 9;
        for(const auto& item : sidebar) {
            if(y >= CONSOLE_HEIGHT - 30) break; // Reserve space for duck
            gotoxy(sbX + 2, y++);
            std::string color = (item.find("FREE") != std::string::npos) ? CYAN : RED;
            std::cout << color << item.substr(0, SIDEBAR_WIDTH - 4) << RESET;
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

            if (y > CONSOLE_HEIGHT - 6) break; // Safety check

            gotoxy(4, y++);
            std::cout << BRIGHT_ORANGE << "ETAGE " << floorNum << RESET;
            
            // Draw Top Borders
            gotoxy(4, y);
            for(const auto& r : floorRooms) {
                std::string color = r.free ? CYAN : RED;
                std::cout << color << "┌────────────┐  " << RESET;
            }
            y++;

            // Draw Room Number
            gotoxy(4, y);
            for(const auto& r : floorRooms) {
                std::string color = r.free ? CYAN : RED;
                std::cout << color << "│ " << WHITE << BOLD << std::left << std::setw(10) << r.id << color << " │  " << RESET;
            }
            y++;

            // Draw Room Type
            gotoxy(4, y);
            for(const auto& r : floorRooms) {
                std::string color = r.free ? CYAN : RED;
                std::string typeShort = r.type.substr(0, 10);
                std::cout << color << "│ " << WHITE << std::left << std::setw(10) << typeShort << color << " │  " << RESET;
            }
            y++;

            // Draw Bottom Borders
            gotoxy(4, y);
            for(const auto& r : floorRooms) {
                std::string color = r.free ? CYAN : RED;
                std::cout << color << "└────────────┘  " << RESET;
            }
            y += 2; // Space between floors
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

        FlushConsoleInputBuffer(hIn);
        
        while(true) {
            if (needRedraw) {
                drawLayout(title, sidebar);
                
                int menuStartY = (CONSOLE_HEIGHT - (options.size() * 2)) / 2;

                for(size_t i = 0; i < options.size(); ++i) {
                    gotoxy(1, menuStartY + (int)i * 2);
                    if(i == selected) {
                        std::cout << center(" " + options[i] + " ", CONTENT_WIDTH);
                        gotoxy((CONTENT_WIDTH - options[i].length())/2 - 2, menuStartY + (int)i * 2);
                        std::cout << YELLOW << "► " << INVERSE << options[i] << RESET;
                    } else {
                        std::cout << ORANGE << center(options[i], CONTENT_WIDTH) << RESET;
                    }
                }
                
                gotoxy(1, CONSOLE_HEIGHT - 2);
                std::cout << ORANGE << center("(Mouse or Arrow Keys + Enter)", CONTENT_WIDTH) << RESET;
                
                needRedraw = false;
            } else if (!sidebar.empty()) {
                // Update just the clock if no full redraw needed
                drawBigClock(CONTENT_WIDTH + 1 + 6, 2);
                
                // Update Globe
                drawWorld(CONTENT_WIDTH + 1 + (SIDEBAR_WIDTH - 56)/2, CONSOLE_HEIGHT - 26, frameCounter++);
            }

            // Wait for input with timeout for clock update
            if (WaitForSingleObject(hIn, 50) == WAIT_OBJECT_0) {
                ReadConsoleInput(hIn, ir, 128, &nRead);
                for (DWORD i = 0; i < nRead; i++) {
                    if (ir[i].EventType == KEY_EVENT && ir[i].Event.KeyEvent.bKeyDown) {
                        WORD key = ir[i].Event.KeyEvent.wVirtualKeyCode;
                        if (key == VK_UP) {
                            selected--;
                            if (selected < 0) selected = static_cast<int>(options.size()) - 1;
                            playSound(400, 20);
                            needRedraw = true;
                        } else if (key == VK_DOWN) {
                            selected++;
                            if (selected >= static_cast<int>(options.size())) selected = 0;
                            playSound(400, 20);
                            needRedraw = true;
                        } else if (key == VK_RETURN) {
                            playSound(800, 50);
                            return selected + 1;
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
                    }
                }
            }
        }
    }
}

#endif