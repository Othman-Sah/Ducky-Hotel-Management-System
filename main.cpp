#include "Hotel.h"
#include "HotelServices.h"
#include "Authentication.h"
#include "User.h"
#include "AuthUI.h"
#include "AdminInterface.h"
#include "UserInterface.h"
#include <iostream>

int main() {
    RetroUI::setupConsole();
    
    // Cool Loading Animation
    RetroUI::clear();
    RetroUI::gotoxy(40, 12);
    std::cout << RetroUI::ORANGE << "INITIALIZING SYSTEM..." << std::endl;
    RetroUI::gotoxy(40, 14);
    std::cout << RetroUI::YELLOW << "[";
    for(int i=0; i<40; i++) { std::cout << "="; Sleep(30); }
    std::cout << "]" << RetroUI::RESET;
    Sleep(500);

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
    
    hotel.ajouterChambre(201, "Double", 800.0);
    hotel.ajouterChambre(202, "Double", 800.0);
    hotel.ajouterChambre(203, "Suite", 1200.0);
    hotel.ajouterChambre(204, "Suite", 1200.0);
    
    hotel.ajouterChambre(301, "Suite", 1500.0);
    hotel.ajouterChambre(302, "Presid.", 2500.0);
    hotel.ajouterChambre(303, "Presid.", 2500.0);
    
    hotel.loadReservations(); // Load saved reservations

    while (true) {
        User* loggedInUser = AuthUI::showLoginScreen(auth);
        
        if (loggedInUser->getRole() == Role::ADMIN) {
            AdminUI::dashboard(hotel, loggedInUser, auth);
        } else {
            UserUI::dashboard(hotel, loggedInUser);
        }
        
        // Reset for next user
        loggedInUser = nullptr;
    }

    return 0;
}
