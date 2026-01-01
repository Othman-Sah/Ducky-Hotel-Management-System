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

    // Setup initial data
    hotel.ajouterChambre(101, "Simple", 500.0);
    hotel.ajouterChambre(102, "Simple", 500.0);
    hotel.ajouterChambre(103, "Simple", 500.0);
    hotel.ajouterChambre(104, "Double", 700.0);
    hotel.ajouterChambre(105, "Double", 700.0);
    hotel.ajouterChambre(106, "Double", 700.0);
    hotel.ajouterChambre(107, "Suite", 900.0);
    hotel.ajouterChambre(108, "Simple", 500.0);
    
    hotel.ajouterChambre(201, "Double", 800.0);
    hotel.ajouterChambre(202, "Double", 800.0);
    hotel.ajouterChambre(203, "Suite", 1200.0);
    hotel.ajouterChambre(204, "Suite", 1200.0);
    hotel.ajouterChambre(205, "Double", 800.0);
    hotel.ajouterChambre(206, "Double", 800.0);
    hotel.ajouterChambre(207, "Suite", 1200.0);
    hotel.ajouterChambre(208, "Double", 800.0);
    
    hotel.ajouterChambre(301, "Suite", 1500.0);
    hotel.ajouterChambre(302, "Presid.", 2500.0);
    hotel.ajouterChambre(303, "Presid.", 2500.0);
    hotel.ajouterChambre(304, "Suite", 1500.0);
    hotel.ajouterChambre(305, "Suite", 1500.0);
    hotel.ajouterChambre(306, "Suite", 1500.0);
    hotel.ajouterChambre(307, "Presid.", 2500.0);
    hotel.ajouterChambre(308, "Suite", 1500.0);
    
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
