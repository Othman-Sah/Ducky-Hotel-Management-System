#ifndef RETROUI_H
#define RETROUI_H

#include "RetroCore.h"
#include "RetroMedia.h"
#include "RetroMaps.h"
#include <vector>
#include <string>

namespace RetroUI {
    void drawTicker(int frame);
    void drawBigClock(int x, int y);
    void drawLayout(const std::string& title, const std::vector<std::string>& sidebar = {});
    void drawWorld(int x, int y, int frame);
    std::string textInput(const std::string& label, int y);
    void drawInfoBox(const std::vector<std::string>& lines);
    void drawModal(const std::string& title, int height);
    void drawTable(const std::vector<std::string>& rows);
    int showMenu(const std::string& title, const std::vector<std::string>& options, const std::vector<std::string>& sidebar = {});
    void showMapViewer(const std::vector<RoomData>& rooms, const std::vector<ParkingData>& parking, const std::vector<TableData>& tables);
}
#endif