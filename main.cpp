#include "Hotel.h"
#include "HotelServices.h"
#include "Authentication.h"
#include "User.h"
#include "AuthUI.h"
#include "AdminInterface.h"
#include "UserInterface.h"
#include "RetroUI.h"
#include <iostream>
#include <vector>

int main() {
    RetroUI::setupConsole();
    RetroUI::updateTheme();
    
    // CRT Boot Sequence
    RetroUI::bootSequence();

    Authentication auth;
    Hotel hotel("Hotel Deluxe EMSI");
    Restaurant::initializeMenu();
    ParkingManager::initialize(50); // 50 Parking spots

    hotel.loadReservations(); // Load saved reservations

    while (true) {
        std::vector<std::string> mainOptions = { "LOGIN", "HOTEL MAPS", "EXIT SYSTEM" };
        int choice = RetroUI::showMenu("HOTEL DELUXE MAIN MENU", mainOptions);

        if (choice == 1) {
            User* loggedInUser = AuthUI::showLoginScreen(auth);
            if (loggedInUser) {
                if (loggedInUser->getRole() == Role::ADMIN) {
                    AdminUI::dashboard(hotel, loggedInUser, auth);
                } else {
                    UserUI::dashboard(hotel, loggedInUser);
                }
            }
        } else if (choice == 2) {
            // Prepare Map Data
            std::vector<RetroUI::RoomData> roomData;
            for (const auto& r : hotel.getChambres()) {
                roomData.push_back({r.getNumeroChambre(), r.getTypeChambre(), r.getDisponibilite()});
            }

            std::vector<RetroUI::ParkingData> parkingData;
            int occupied = ParkingManager::getTotalSpots() - ParkingManager::getAvailableSpots();
            for (int i = 1; i <= 50; i++) {
                parkingData.push_back({i, i > occupied});
            }

            std::vector<RetroUI::TableData> tableData;
            for (int i = 1; i <= 24; i++) tableData.push_back({i, true});

            RetroUI::showMapViewer(roomData, parkingData, tableData);
        } else if (choice == 3) {
            break;
        }
    }

    return 0;
}
