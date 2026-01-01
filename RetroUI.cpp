#include "RetroUI.h"
#include <conio.h>
#include <algorithm>
#include <cmath>
#include <iomanip>

namespace RetroUI {

    void drawTicker(int frame) {
        int yTop = CONSOLE_HEIGHT - 2, yMid = CONSOLE_HEIGHT - 1, yBot = CONSOLE_HEIGHT;
        int sep1 = CONTENT_WIDTH - 75, sep2 = CONTENT_WIDTH - 45;
        auto rep = [](const std::string& s, int n) { for(int i=0; i<n; i++) std::cout << s; };

        gotoxy(1, yTop); std::cout << ORANGE << "╔";
        std::string t1 = "[ LIVE NEWS ]"; int pad1 = (sep1 - 2 - t1.length()) / 2;
        rep("═", pad1); std::cout << t1; rep("═", sep1 - 2 - pad1 - t1.length());
        std::cout << "╦";
        std::string t2 = "[ METEO ]"; int w2 = sep2 - sep1 - 1, pad2 = (w2 - t2.length()) / 2;
        rep("═", pad2); std::cout << t2; rep("═", w2 - pad2 - t2.length());
        std::cout << "╦";
        std::string t3 = "[ MEDIA ]"; int w3 = CONTENT_WIDTH - sep2 - 1, pad3 = (w3 - t3.length()) / 2;
        rep("═", pad3); std::cout << t3; rep("═", w3 - pad3 - t3.length());
        std::cout << "╗" << RESET;

        gotoxy(1, yMid); std::cout << ORANGE << "║" << RESET;
        gotoxy(sep1, yMid); std::cout << ORANGE << "║" << RESET;
        gotoxy(sep2, yMid); std::cout << ORANGE << "║" << RESET;
        gotoxy(CONTENT_WIDTH, yMid); std::cout << ORANGE << "║" << RESET;

        gotoxy(1, yBot); std::cout << ORANGE << "╚";
        rep("═", sep1 - 1); std::cout << "╩";
        rep("═", sep2 - sep1 - 1); std::cout << "╩";
        rep("═", CONTENT_WIDTH - sep2 - 1); std::cout << "╝" << RESET;

        std::string news = "Welcome to Hotel Deluxe *** Special Offer: 20% off Spa treatments *** Weather: Sunny 25C *** System Status: ONLINE *** ";
        int newsW = sep1 - 2;
        int offset = (frame / 2) % news.length();
        std::string display = news.substr(offset) + news.substr(0, offset);
        if ((int)display.length() > newsW) display = display.substr(0, newsW);
        gotoxy(2, yMid); std::cout << WHITE << display << RESET;

        // Meteo
        std::time_t now = std::time(nullptr);
        std::tm *ltm = std::localtime(&now);
        bool isNight = (ltm->tm_hour >= 18 || ltm->tm_hour < 6);
        std::string meteoText = isNight ? "18C CLEAR SKY" : "25C SUNNY";
        gotoxy(sep1 + 1, yMid); std::cout << WHITE << center(meteoText, sep2 - sep1 - 1) << RESET;

        static bool musicLoaded = false;
        if (!musicLoaded) { loadMusic(); musicLoaded = true; }

        std::string prevBtn = (mediaButtonFocus == 0) ? INVERSE + "[<<]" + RESET + YELLOW : "[<<]";
        std::string playBtn = (mediaButtonFocus == 1) ? INVERSE + ((isMusicPlaying && !isMusicPaused) ? "[||]" : "[|>]") + RESET + YELLOW : ((isMusicPlaying && !isMusicPaused) ? "[||]" : "[|>]");
        std::string nextBtn = (mediaButtonFocus == 2) ? INVERSE + "[>>]" + RESET + YELLOW : "[>>]";

        std::string title = playlist.empty() ? "No MP3s" : playlist[currentSongIndex];
        if (title.length() > 12) title = title.substr(0, 9) + "...";
        gotoxy(sep2 + 2, yMid); std::cout << WHITE << std::left << std::setw(13) << title << RESET << YELLOW << prevBtn << " " << playBtn << " " << nextBtn << RESET;
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
        std::time_t t = std::time(nullptr); std::tm tm = *std::localtime(&t);
        int nums[] = { tm.tm_hour / 10, tm.tm_hour % 10, 10, tm.tm_min / 10, tm.tm_min % 10, 10, tm.tm_sec / 10, tm.tm_sec % 10 };
        for (int row = 0; row < 5; ++row) {
            gotoxy(x, y + row); std::cout << YELLOW << BOLD;
            for (int i = 0; i < 8; ++i) std::cout << DIGITS[nums[i]][row];
            std::cout << RESET;
        }
    }

    void drawLayout(const std::string& title, const std::vector<std::string>& sidebar) {
        clear();
        auto rep = [](const std::string& s, int n) { for(int i=0; i<n; i++) std::cout << s; };
        
        gotoxy(1, 1); std::cout << ORANGE << "╔"; rep("═", CONTENT_WIDTH - 2); std::cout << "╗";
        for(int y=2; y<CONSOLE_HEIGHT; y++) { gotoxy(1, y); std::cout << "║"; gotoxy(CONTENT_WIDTH, y); std::cout << "║"; }
        gotoxy(1, CONSOLE_HEIGHT); std::cout << "╚"; rep("═", CONTENT_WIDTH - 2); std::cout << "╝";

        gotoxy(1, 3); std::cout << "║" << BRIGHT_ORANGE << BOLD << center(title, CONTENT_WIDTH - 2) << RESET << ORANGE << "║";
        gotoxy(1, 4); std::cout << "╠"; rep("═", CONTENT_WIDTH - 2); std::cout << "╣";

        int sbX = CONTENT_WIDTH + 1;
        gotoxy(sbX, 1); std::cout << ORANGE << "╔"; rep("═", SIDEBAR_WIDTH - 2); std::cout << "╗";
        for(int y=2; y<CONSOLE_HEIGHT; y++) { gotoxy(sbX, y); std::cout << "║"; gotoxy(CONSOLE_WIDTH, y); std::cout << "║"; }
        gotoxy(sbX, CONSOLE_HEIGHT); std::cout << "╚"; rep("═", SIDEBAR_WIDTH - 2); std::cout << "╝";

        drawBigClock(sbX + 6, 2);
        gotoxy(sbX, 8); std::cout << ORANGE << "╠"; rep("═", SIDEBAR_WIDTH - 2); std::cout << "╣";

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
        int endIdx = (std::min)((int)rooms.size(), startIdx + itemsPerPage);

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

    void drawWorld(int x, int y, int frame) {
        float t = frame * 0.2f;
        float r = 0.6f;
        float dx = r * cos(t);
        float dz = r * sin(t) * 0.5f;
        float angle = t + 1.57f;
        float bob = 0.05f * fabs(sin(t * 4.0f));
        float cosA = cos(angle), sinA = sin(angle);
        float tilt = 0.3f, cosTilt = cos(tilt), sinTilt = sin(tilt);
        int width = 56, height = 16;
        
        std::vector<char> buffer(width * height, ' ');
        std::vector<float> zBuffer(width * height, -100.0f);
        
        auto plot = [&](float mx, float my, float mz, char c) {
            float rx = mx * cosA - mz * sinA;
            float ry = my;
            float rz = mx * sinA + mz * cosA;
            float wx = rx + dx, wy = ry + bob, wz = rz + dz;
            float cx = wx, cy = wy * cosTilt - wz * sinTilt, cz = wy * sinTilt + wz * cosTilt;
            int xp = (width / 2) + (int)(cx * 14.0f);
            int yp = (height / 2) - (int)(cy * 7.0f);
            if (xp >= 0 && xp < width && yp >= 0 && yp < height) {
                int idx = yp * width + xp;
                if (cz > zBuffer[idx]) { zBuffer[idx] = cz; buffer[idx] = c; }
            }
        };

        for(float i=0; i<6.28; i+=0.15) for(float j=0; j<3.14; j+=0.15) plot(1.1f * sin(j) * cos(i), 0.6f * sin(j) * sin(i), 0.7f * cos(j), '#');
        for(float i=0; i<6.28; i+=0.2) for(float j=0; j<3.14; j+=0.2) plot(0.45f * sin(j) * cos(i) + 0.8f, 0.45f * sin(j) * sin(i) + 0.7f, 0.45f * cos(j), '@');
        for(float r=0; r<0.3; r+=0.05) plot(1.3f + r, 0.6f, 0.0f, '>');
        for(float i=0; i<6.28; i+=0.3) plot(-1.2f, 0.2f + 0.2f*cos(i), 0.2f*sin(i), '*');
        for(float i=0; i<6.28; i+=0.2) for(float j=0; j<3.14; j+=0.2) { float wx = 0.5f * sin(j) * cos(i), wy = 0.2f * sin(j) * sin(i), wz = 0.1f * cos(j); plot(wx, wy, wz + 0.75f, '='); plot(wx, wy, wz - 0.75f, '='); }
        float legSwing = sin(t * 4.0f) * 0.2f;
        plot(0.0f + legSwing, -0.5f, 0.2f, '|'); plot(0.0f + legSwing, -0.6f, 0.2f, '\'');
        plot(0.0f - legSwing, -0.5f, -0.2f, '|'); plot(0.0f - legSwing, -0.6f, -0.2f, '\'');
        
        for(int row=0; row<height; row++) {
            gotoxy(x, y + row);
            for(int col=0; col<width; col++) {
                char pixel = buffer[row * width + col];
                std::cout << (pixel == ' ' ? " " : (YELLOW + std::string(1, pixel) + RESET));
            }
        }
    }

    std::string textInput(const std::string& label, int y) {
        int startX = (CONTENT_WIDTH - 40) / 2;
        gotoxy(startX, y); std::cout << ORANGE << label << ": " << YELLOW;
        std::cout << "\033[?25h"; std::string input; std::cin >> input; std::cout << "\033[?25l";
        return input;
    }

    void drawInfoBox(const std::vector<std::string>& lines) {
        int boxWidth = 60, startX = (CONTENT_WIDTH - boxWidth) / 2, startY = 10;
        auto rep = [](const std::string& s, int n) { for(int i=0; i<n; i++) std::cout << s; };
        gotoxy(startX, startY++); std::cout << ORANGE << "┌"; rep("─", boxWidth - 2); std::cout << "┐" << RESET;
        for (const auto& line : lines) {
            gotoxy(startX, startY++); std::cout << ORANGE << "│ " << WHITE << center(line, boxWidth - 4) << ORANGE << " │" << RESET;
        }
        gotoxy(startX, startY); std::cout << ORANGE << "└"; rep("─", boxWidth - 2); std::cout << "┘" << RESET;
    }

    void drawModal(const std::string& title, int height) {
        int width = 60, startX = (CONTENT_WIDTH - width) / 2, startY = (CONSOLE_HEIGHT - height) / 2;
        auto rep = [](const std::string& s, int n) { for(int i=0; i<n; i++) std::cout << s; };
        for(int i=0; i<height; i++) { gotoxy(startX, startY + i); std::cout << std::string(width, ' '); }
        gotoxy(startX, startY); std::cout << ORANGE << "╔"; rep("═", width - 2); std::cout << "╗" << RESET;
        gotoxy(startX, startY + 1); std::cout << ORANGE << "║" << WHITE << BOLD << center(title, width - 2) << RESET << ORANGE << "║" << RESET;
        gotoxy(startX, startY + 2); std::cout << ORANGE << "╠"; rep("═", width - 2); std::cout << "╣" << RESET;
        for(int i=3; i<height-1; i++) { gotoxy(startX, startY + i); std::cout << ORANGE << "║" << std::string(width - 2, ' ') << "║" << RESET; }
        gotoxy(startX, startY + height - 1); std::cout << ORANGE << "╚"; rep("═", width - 2); std::cout << "╝" << RESET;
    }

    void drawTable(const std::vector<std::string>& rows) {
        int y = 6;
        for(const auto& row : rows) { if (y > CONSOLE_HEIGHT - 3) break; gotoxy(4, y++); std::cout << WHITE << row << RESET; }
    }

    int showMenu(const std::string& title, const std::vector<std::string>& options, const std::vector<std::string>& sidebar) {
        int selected = 0;
        HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
        INPUT_RECORD ir[128];
        DWORD nRead;
        bool needRedraw = true;
        int frameCounter = 0;
        mediaButtonFocus = -1;
        FlushConsoleInputBuffer(hIn);
        
        while(true) {
            if (needRedraw) {
                drawLayout(title, sidebar);
                int menuStartY = (CONSOLE_HEIGHT - (options.size() * 2)) / 2;
                for(size_t i = 0; i < options.size(); ++i) {
                    gotoxy(1, menuStartY + (int)i * 2);
                    if((int)i == selected) {
                        std::cout << std::string(CONTENT_WIDTH, ' ');
                        int startX = (CONTENT_WIDTH - (int)options[i].length()) / 2 - 1;
                        if (startX < 1) startX = 1;
                        gotoxy(startX, menuStartY + (int)i * 2);
                        std::cout << YELLOW << "► " << INVERSE << options[i] << RESET;
                    } else std::cout << ORANGE << center(options[i], CONTENT_WIDTH) << RESET;
                }
                needRedraw = false;
            } else if (!sidebar.empty()) {
                drawBigClock(CONTENT_WIDTH + 1 + 6, 2);
                drawWorld(CONTENT_WIDTH + 1 + (SIDEBAR_WIDTH - 56)/2, CONSOLE_HEIGHT - 20, frameCounter++);
                drawTicker(frameCounter);
            }

            if (WaitForSingleObject(hIn, 50) == WAIT_OBJECT_0) {
                ReadConsoleInput(hIn, ir, 128, &nRead);
                for (DWORD i = 0; i < nRead; i++) {
                    if (ir[i].EventType == KEY_EVENT && ir[i].Event.KeyEvent.bKeyDown) {
                        WORD key = ir[i].Event.KeyEvent.wVirtualKeyCode;
                        if (key == VK_UP) {
                            if (mediaButtonFocus != -1) { mediaButtonFocus = -1; selected = options.size() - 1; }
                            else { selected--; if (selected < 0) { selected = -1; mediaButtonFocus = 1; } }
                            playSound(400, 20); needRedraw = true;
                        } else if (key == VK_DOWN) {
                            if (mediaButtonFocus != -1) { mediaButtonFocus = -1; selected = 0; }
                            else { selected++; if (selected >= options.size()) { selected = -1; mediaButtonFocus = 1; } }
                            playSound(400, 20); needRedraw = true;
                        } else if (key == VK_RETURN) {
                            if (mediaButtonFocus != -1) {
                                if (mediaButtonFocus == 0) { stopMusic(); if (!playlist.empty()) currentSongIndex = (currentSongIndex - 1 + playlist.size()) % playlist.size(); playMusic(); }
                                else if (mediaButtonFocus == 1) togglePauseMusic();
                                else if (mediaButtonFocus == 2) nextMusic();
                                playSound(400, 20); needRedraw = true;
                            } else { playSound(800, 50); return selected + 1; }
                        } else if (key == VK_RIGHT) {
                            if (mediaButtonFocus != -1) mediaButtonFocus = (mediaButtonFocus + 1) % 3;
                            else sidebarPage++;
                            playSound(400, 20); needRedraw = true;
                        } else if (key == VK_LEFT) {
                            if (mediaButtonFocus != -1) mediaButtonFocus = (mediaButtonFocus - 1 + 3) % 3;
                            else sidebarPage--;
                            playSound(400, 20); needRedraw = true;
                        }
                    }
                }
            }
        }
    }

    void showMapViewer(const std::vector<RoomData>& rooms, const std::vector<ParkingData>& parking, const std::vector<TableData>& tables) {
        int currentMap = 0;
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
                if (currentMap == 0) title = "HOTEL FLOOR PLAN";
                else if (currentMap == 1) title = "PARKING LOT";
                else if (currentMap == 2) title = "RESTAURANT & BAR";
                else title = "MAIN ENTRANCE";
                
                drawLayout(title, {});
                if (currentMap == 0) drawFloorPlan(rooms);
                else if (currentMap == 1) drawParkingMap(parking);
                else if (currentMap == 2) drawRestaurantMap(tables);
                else drawEntranceMap();
                
                gotoxy(4, CONSOLE_HEIGHT - 4);
                std::cout << YELLOW << "◄ PREV MAP (Left Arrow)   |   NEXT MAP (Right Arrow) ►   |   EXIT (Esc/Enter)" << RESET;
                needRedraw = false;
            }
            drawBigClock(CONTENT_WIDTH + 1 + 6, 2);
            drawWorld(CONTENT_WIDTH + 1 + (SIDEBAR_WIDTH - 56)/2, CONSOLE_HEIGHT - 20, frameCounter++);
            drawTicker(frameCounter);

            if (WaitForSingleObject(hIn, 50) == WAIT_OBJECT_0) {
                ReadConsoleInput(hIn, ir, 128, &nRead);
                for (DWORD i = 0; i < nRead; i++) {
                    if (ir[i].EventType == KEY_EVENT && ir[i].Event.KeyEvent.bKeyDown) {
                        WORD key = ir[i].Event.KeyEvent.wVirtualKeyCode;
                        if (key == VK_LEFT) { currentMap = (currentMap - 1 + 4) % 4; needRedraw = true; }
                        else if (key == VK_RIGHT) { currentMap = (currentMap + 1) % 4; needRedraw = true; }
                        else if (key == VK_ESCAPE || key == VK_RETURN) inMapMode = false;
                    }
                }
            }
        }
    }
}