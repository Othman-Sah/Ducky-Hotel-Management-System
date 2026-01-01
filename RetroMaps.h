#ifndef RETRO_MAPS_H
#define RETRO_MAPS_H

#include <vector>
#include <string>

namespace RetroUI {
    struct RoomData { int id; std::string type; bool free; };
    struct ParkingData { int id; bool free; };
    struct TableData { int id; bool free; };

    void drawFloorPlan(const std::vector<RoomData>& rooms);
    void drawParkingMap(const std::vector<ParkingData>& spots);
    void drawRestaurantMap(const std::vector<TableData>& tables);
    void drawEntranceMap();
}
#endif