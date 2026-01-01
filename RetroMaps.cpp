#include "RetroMaps.h"
#include "RetroCore.h"
#include <map>
#include <iostream>
#include <iomanip>

namespace RetroUI {
    
    void drawFloorPlan(const std::vector<RoomData>& rooms) {
        auto rep = [](const std::string& s, int n) { for(int i=0; i<n; i++) std::cout << s; };
        std::map<int, std::vector<RoomData>> floors;
        for (const auto& r : rooms) floors[r.id / 100].push_back(r);

        int y = 6;
        for (auto it = floors.rbegin(); it != floors.rend(); ++it) {
            if (y + 15 > CONSOLE_HEIGHT - 3) break;
            int mapX = 4;
            
            gotoxy(mapX, y++);
            std::cout << ORANGE << BLOCK << " FLOOR " << it->first << " ";
            rep(BLOCK_TOP, 80); std::cout << RESET;

            static const std::vector<std::string> mapBase = {
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

            for(size_t i=0; i<mapBase.size(); i++) { gotoxy(mapX, y + i); std::cout << DARK_GRAY << mapBase[i] << RESET; }

            struct Slot { int suffix; int x; int y; int w; int h; };
            static const std::vector<Slot> slots = {
                {1, 16, 1, 10, 3}, {2, 30, 1, 10, 3}, {3, 44, 1, 20, 3}, {4, 68, 1, 10, 3},
                {5, 16, 9, 10, 3}, {6, 30, 9, 10, 3}, {7, 44, 9, 20, 3}, {8, 68, 9, 10, 3}
            };

            for (const auto& r : it->second) {
                int suffix = r.id % 100;
                for (const auto& s : slots) {
                    if (s.suffix == suffix) {
                        std::string color = r.free ? CYAN : RED;
                        gotoxy(mapX + s.x - 1, y + s.y - 1); std::cout << color << "┌" << std::string(s.w, '─') << "┐";
                        for(int k=0; k<s.h; k++) {
                            gotoxy(mapX + s.x - 1, y + s.y + k); std::cout << color << "│";
                            gotoxy(mapX + s.x + s.w, y + s.y + k); std::cout << color << "│";
                        }
                        gotoxy(mapX + s.x - 1, y + s.y + s.h); std::cout << color << "└" << std::string(s.w, '─') << "┘";
                        gotoxy(mapX + s.x, y + s.y);     std::cout << color << BOLD << center(std::to_string(r.id), s.w) << RESET;
                        gotoxy(mapX + s.x, y + s.y + 1); std::cout << WHITE << center(r.type.substr(0, s.w), s.w) << RESET;
                        gotoxy(mapX + s.x, y + s.y + 2); std::cout << (r.free ? GREEN : RED) << center(r.free ? "FREE" : "BUSY", s.w) << RESET;
                    }
                }
            }
            y += 15;
        }
    }

    void drawParkingMap(const std::vector<ParkingData>& spots) {
        auto rep = [](const std::string& s, int n) { for(int i=0; i<n; i++) std::cout << s; };
        int y = 6, mapX = 4;
        gotoxy(mapX, y++); std::cout << ORANGE << BLOCK << " PARKING LOT " << std::string(70, ' ') << RESET;
        gotoxy(mapX + 14, y-1); rep(BLOCK_TOP, 80);

        static const std::vector<std::string> mapBase = {
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
        for(size_t i=0; i<mapBase.size(); i++) { gotoxy(mapX, y + i); std::cout << DARK_GRAY << mapBase[i] << RESET; }

        for (const auto& s : spots) {
            if (s.id > 50) continue;
            int row = (s.id - 1) / 10;
            int col = (s.id - 1) % 10;
            int px = 20 + col * 6;
            int py = 2 + row * 4;
            std::string color = s.free ? GREEN : RED;
            gotoxy(mapX + px, y + py - 1); std::cout << color << "┌" << (s.id < 10 ? "0" : "") << s.id << "┐" << RESET;
            gotoxy(mapX + px, y + py);     std::cout << color << "│" << (s.free ? "Fr" : "Bs") << "│" << RESET;
            gotoxy(mapX + px, y + py + 1); std::cout << color << "└───┘" << RESET;
        }
    }

    void drawRestaurantMap(const std::vector<TableData>& tables) {
        auto rep = [](const std::string& s, int n) { for(int i=0; i<n; i++) std::cout << s; };
        int y = 6, mapX = 4;
        gotoxy(mapX, y++); std::cout << ORANGE << BLOCK << " RESTAURANT " << std::string(70, ' ') << RESET;
        gotoxy(mapX + 13, y-1); rep(BLOCK_TOP, 81);

        static const std::vector<std::string> mapBase = {
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
        for(size_t i=0; i<mapBase.size(); i++) { gotoxy(mapX, y + i); std::cout << DARK_GRAY << mapBase[i] << RESET; }
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

        static const std::vector<std::string> mapBase = {
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

        for(size_t i=0; i<mapBase.size(); i++) {
            gotoxy(mapX, y + i);
            std::cout << WHITE << mapBase[i] << RESET;
        }
    }
}