#include "Hotel.h"
#include "HotelServices.h"
#include "Authentication.h"
#include "User.h"
#include "RetroUI.h"
#include "Bill.h"
#include <iostream>
#include <limits>
#include <vector>
#include <cstdlib>

void clearScreen() {
    RetroUI::clear();
}

User* showLoginScreen(Authentication& auth) {
    std::string username, password;
    User* loggedInUser = nullptr;

    while (loggedInUser == nullptr) {
        RetroUI::drawLayout("SYSTEME AUTHENTIFICATION");
        username = RetroUI::textInput("Nom d'utilisateur", 12);
        password = RetroUI::textInput("Mot de passe     ", 14);

        // Role is now determined automatically by the backend
        loggedInUser = auth.login(username, password);

        if (loggedInUser == nullptr) {
            RetroUI::drawInfoBox({"Identifiants incorrects. Veuillez reessayer."});
            system("pause");
        }
    }
    return loggedInUser;
}

void showDashboard(Hotel& hotel) {
    RetroUI::drawLayout("TABLEAU DE BORD", hotel.getRoomStatusList());
    
    int midX = (RetroUI::CONTENT_WIDTH) / 2;
    
    RetroUI::gotoxy(midX - 15, 8);
    std::cout << RetroUI::ORANGE << "Chambres Occupees: " << RetroUI::RED << hotel.getOccupiedRoomsCount();
    
    RetroUI::gotoxy(midX - 15, 11);
    std::cout << RetroUI::ORANGE << "Chambres Disponibles: " << RetroUI::CYAN << hotel.getAvailableRoomsCount();
    
    RetroUI::gotoxy(midX - 15, 14);
    std::cout << RetroUI::ORANGE << "Revenu Estime: " << RetroUI::YELLOW << "$" << hotel.getTotalRevenue();
    
    RetroUI::gotoxy(midX - 10, 25);
    std::cout << "(Appuyez sur une touche)";
    system("pause");
}

std::vector<MenuItem> commanderDuMenu() {
    std::vector<MenuItem> commande;
    std::string nomArticle;
    while (true) {
        std::cout << "Entrez le nom de l'article a commander (ou 'fin' pour terminer): ";
        std::cin >> nomArticle;
        if (nomArticle == "fin") {
            break;
        }

        bool articleTrouve = false;
        for (const auto& categorie : Restaurant::menu) {
            for (const auto& article : categorie.second) {
                if (article.name == nomArticle) {
                    commande.push_back(article);
                    std::cout << nomArticle << " ajoute a la commande.\n";
                    articleTrouve = true;
                    break;
                }
            }
            if (articleTrouve) break;
        }
        
        if (!articleTrouve) {
            std::cout << "Article non trouve.\n";
        }
    }
    return commande;
}

void adminManageRooms(Hotel& hotel, Authentication& auth) {
    int choix;
    int numeroChambre;
    std::string nomClient;
    int nuits;
    std::string username, password;

    while (true) {
        choix = RetroUI::showMenu("ADMIN - Gestion des Chambres", {"Afficher toutes les chambres", "Reserver (Force)", "Liberer (Check-out)", "Retour"}, hotel.getRoomStatusList());

        switch (choix) {
            case 1: 
                {
                    RetroUI::drawLayout("PLAN DES ETAGES", hotel.getRoomStatusList());
                    std::vector<RetroUI::RoomData> roomData;
                    for(const auto& r : hotel.getChambres()) roomData.push_back({r.getNumeroChambre(), r.getTypeChambre(), r.getDisponibilite()});
                    RetroUI::drawFloorPlan(roomData);
                    system("pause > nul"); 
                }
                break;
            case 2: {
                RetroUI::drawLayout("NOUVELLE RESERVATION", hotel.getRoomStatusList());
                RetroUI::drawModal("NOUVELLE RESERVATION", 16);
                
                int startY = (RetroUI::CONSOLE_HEIGHT - 16) / 2 + 4;
                
                std::string numStr = RetroUI::textInput("Numero Chambre", startY);
                numeroChambre = std::atoi(numStr.c_str());
                if (numeroChambre == 0) break;
                
                nomClient = RetroUI::textInput("Nom Client    ", startY + 2);
                std::string nuitStr = RetroUI::textInput("Nombre Nuits  ", startY + 4);
                nuits = std::atoi(nuitStr.c_str());
                
                // Create User Account
                username = RetroUI::textInput("Username      ", startY + 6);
                password = RetroUI::textInput("Password      ", startY + 8);

                Room* room = hotel.findRoom(numeroChambre);
                if (room && room->getDisponibilite()) {
                    hotel.reserverChambre(numeroChambre, nomClient, nuits);
                    auth.addUser(username, password, Role::USER);
                    RetroUI::drawModal("SUCCES", 5);
                    RetroUI::gotoxy((RetroUI::CONTENT_WIDTH - 20)/2, (RetroUI::CONSOLE_HEIGHT - 5)/2 + 2);
                    std::cout << "Reservation effectuee!";
                } else {
                    RetroUI::drawModal("ERREUR", 5);
                    RetroUI::gotoxy((RetroUI::CONTENT_WIDTH - 30)/2, (RetroUI::CONSOLE_HEIGHT - 5)/2 + 2);
                    std::cout << "Chambre introuvable/occupee";
                }
                system("pause > nul");
                break;
            }
            case 3:
                {
                    RetroUI::drawLayout("CHECK-OUT", hotel.getRoomStatusList());
                    RetroUI::drawModal("CHECK-OUT", 8);
                    std::string numStr = RetroUI::textInput("Numero Chambre", (RetroUI::CONSOLE_HEIGHT - 8)/2 + 3);
                    numeroChambre = std::atoi(numStr.c_str());
                    
                    try {
                        Bill bill = hotel.prepareBill(numeroChambre);
                        RetroUI::drawLayout("FACTURE PREVIEW", hotel.getRoomStatusList());
                        RetroUI::drawTable(bill.getBillLines());
                        
                        int choice = RetroUI::showMenu("ACTIONS", {"Imprimer & Liberer", "Retour"}, {});
                        if (choice == 1) {
                            hotel.libererChambre(numeroChambre);
                            RetroUI::drawInfoBox({"Chambre liberee et facture sauvegardee!"});
                            system("pause > nul");
                        }
                    } catch (...) {
                        RetroUI::drawInfoBox({"Chambre introuvable ou vide."});
                        system("pause > nul");
                    }
                }
                break;
            case 4: return;
            default: RetroUI::drawInfoBox({"Choix invalide!"}); system("pause");
        }
    }
}

void adminManageRestaurant(Hotel& hotel) {
    int choice;
    while(true) {
        // Split Screen Layout
        RetroUI::drawLayout("GESTION RESTAURANT", hotel.getRoomStatusList());
        
        // Top: Menu Display
        RetroUI::gotoxy(2, 5); std::cout << RetroUI::YELLOW << "--- MENU ACTUEL ---" << RetroUI::RESET;
        int y = 6;
        for (const auto& category : Restaurant::menu) {
            if (y > 18) break;
            RetroUI::gotoxy(2, y++); std::cout << RetroUI::BRIGHT_ORANGE << "[" << category.first << "]";
            for (const auto& item : category.second) {
                if (y > 18) break;
                RetroUI::gotoxy(4, y++); std::cout << RetroUI::WHITE << item.name << " - $" << item.price;
            }
        }

        // Divider
        RetroUI::gotoxy(1, 20); std::cout << RetroUI::ORANGE << "╠" << std::string(RetroUI::CONTENT_WIDTH - 2, '═') << "╣";

        // Bottom: Options
        RetroUI::gotoxy(2, 21); std::cout << "1. Ajouter Article";
        RetroUI::gotoxy(2, 22); std::cout << "2. Modifier Prix";
        RetroUI::gotoxy(2, 23); std::cout << "3. Retour";
        
        std::string c = RetroUI::textInput("Choix", 25);
        choice = std::atoi(c.c_str());

        if (choice == 1) {
            std::string cat = RetroUI::textInput("Categorie", 25);
            std::string name = RetroUI::textInput("Nom", 26);
            std::string pStr = RetroUI::textInput("Prix", 27);
            RestaurantManager::addItem(cat, name, std::stod(pStr));
        } else if (choice == 2) {
            std::string name = RetroUI::textInput("Nom Article", 25);
            std::string pStr = RetroUI::textInput("Nouveau Prix", 26);
            RestaurantManager::updatePrice(name, std::stod(pStr));
        } else if (choice == 3) {
            return;
        }
    }
}

void adminManageParking() {
    std::vector<std::string> info;
    info.push_back("Total Places: " + std::to_string(ParkingManager::getTotalSpots()));
    info.push_back("Places Libres: " + std::to_string(ParkingManager::getAvailableSpots()));
    RetroUI::drawLayout("ADMIN - Gestion Parking");
    RetroUI::drawInfoBox(info);
    system("pause");
}

void adminDashboard(Hotel& hotel, User* user, Authentication& auth) {
    int choix;
    while (true) {
        choix = RetroUI::showMenu("TABLEAU DE BORD ADMIN", {
            "Vue d'ensemble (Dashboard)", 
            "Gestion Chambres", 
            "Gestion Restaurant", 
            "Gestion Parking", 
            "Deconnexion"
        }, hotel.getRoomStatusList());

        switch (choix) {
            case 1: showDashboard(hotel); break;
            case 2: adminManageRooms(hotel, auth); break;
            case 3: adminManageRestaurant(hotel); break;
            case 4: adminManageParking(); break;
            case 5: return;
            default: break;
        }
    }
}

void userOrderRestaurant(Hotel& hotel) {
    Restaurant::displayMenu();
    std::vector<MenuItem> commande = commanderDuMenu();
    if (!commande.empty()) {
        std::cout << "Entrez votre numero de chambre pour la facturation: ";
        int roomNum;
        std::cin >> roomNum;
        Room* room = hotel.findRoom(roomNum);
        if (room && !room->getDisponibilite()) {
            RoomService* rs = new RoomService(roomNum, commande);
            hotel.addServiceToRoom(roomNum, rs);
            std::cout << "Commande envoyee en cuisine!\n";
        } else {
            std::cout << "Chambre invalide. Commande annulee.\n";
        }
    }
    system("pause");
}

void userBookParking(Hotel& hotel) {
    if (ParkingManager::getAvailableSpots() > 0) {
        std::cout << "Entrez votre numero de chambre: ";
        int roomNum;
        std::cin >> roomNum;
        std::cout << "Nombre de jours: ";
        int days;
        std::cin >> days;
        
        if (ParkingManager::occupySpot()) {
            Parking* p = new Parking(days);
            hotel.addServiceToRoom(roomNum, p);
            std::cout << "Place reservee! (Note: Simulation - place liberee manuellement ou a la fin)\n";
        }
    } else {
        std::cout << "Parking complet!\n";
    }
    system("pause");
}

void userBookSpa(Hotel& hotel) {
    std::cout << "Entrez votre numero de chambre: ";
    int roomNum;
    std::cin >> roomNum;
    Room* room = hotel.findRoom(roomNum);
    
    if (room && !room->getDisponibilite()) {
        std::string treatment;
        std::cout << "Entrez le soin desire (Massage, Facial, Sauna): ";
        std::cin.ignore();
        getline(std::cin, treatment);
        
        Spa* spa = new Spa(treatment);
        hotel.addServiceToRoom(roomNum, spa);
        std::cout << "Rendez-vous Spa reserve!\n";
    } else {
        std::cout << "Chambre invalide.\n";
    }
    system("pause");
}

void userBookGym(Hotel& hotel) {
    std::cout << "Entrez votre numero de chambre: ";
    int roomNum;
    std::cin >> roomNum;
    Room* room = hotel.findRoom(roomNum);
    
    if (room && !room->getDisponibilite()) {
        int hours;
        std::cout << "Nombre d'heures: ";
        std::cin >> hours;
        Gym* gym = new Gym(hours);
        hotel.addServiceToRoom(roomNum, gym);
        std::cout << "Acces Gym reserve!\n";
    } else {
        std::cout << "Chambre invalide.\n";
    }
    system("pause");
}

void userDashboard(Hotel& hotel, User* user) {
    int choix;
    while (true) {
        choix = RetroUI::showMenu("ESPACE CLIENT - " + user->getUsername(), {
            "Voir mes infos (Simule)", 
            "Restaurant (Menu & Commande)", 
            "Reserver Parking", 
            "Spa & Gym", 
            "Deconnexion"
        }, hotel.getRoomStatusList());

        switch(choix) {
            case 1: 
                std::cout << "Fonctionnalite a venir: lier User a Room.\n"; 
                system("pause"); 
                break;
            case 2: userOrderRestaurant(hotel); break;
            case 3: userBookParking(hotel); break;
            case 4: 
                userBookSpa(hotel);
                userBookGym(hotel);
                break;
            case 5: return;
        }
    }
}

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
        User* loggedInUser = showLoginScreen(auth);
        
        if (loggedInUser->getRole() == Role::ADMIN) {
            adminDashboard(hotel, loggedInUser, auth);
        } else {
            userDashboard(hotel, loggedInUser);
        }
        
        // Reset for next user
        loggedInUser = nullptr;
    }

    return 0;
}

/* Old code removed for clarity and architecture fix */
/*
void gererServices(Hotel& hotel) {
    int choix;
    while (true) {
        clearScreen();
        afficherMenuServices();
        std::cin >> choix;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            ConsoleUI::drawInfoBox({"Entree invalide!"});
            system("pause");
            continue;
        }

        switch (choix) {
            case 1: Restaurant::displayMenu(); system("pause"); break;
            case 2: {
                Restaurant::displayMenu();
                std::vector<MenuItem> commande = commanderDuMenu();
                if (!commande.empty()) {
                    Restaurant* restaurant = new Restaurant(commande);
                    restaurant->bookService();
                    hotel.addService(restaurant);
                    ConsoleUI::drawInfoBox({"Note: This order is not tied to a room."});
                }
                system("pause");
                break;
            }
            case 3: {
                std::cout << "Entrez le numero de chambre pour le service: ";
                int numChambre;
                std::cin >> numChambre;
                if(hotel.findRoom(numChambre) == nullptr || hotel.findRoom(numChambre)->getDisponibilite()){
                    ConsoleUI::drawInfoBox({"Chambre non valide ou non occupee."});
                    break;
                }
                Restaurant::displayMenu();
                std::vector<MenuItem> commande = commanderDuMenu();
                if (!commande.empty()) {
                    RoomService* roomService = new RoomService(numChambre, commande);
                    roomService->bookService();
                    hotel.addServiceToRoom(numChambre, roomService);
                }
                system("pause");
                break;
            }
            case 4: {
                std::cout << "Entrez le numero de chambre pour le service: ";
                int numChambre;
                std::cin >> numChambre;
                 if(hotel.findRoom(numChambre) == nullptr || hotel.findRoom(numChambre)->getDisponibilite()){
                    ConsoleUI::drawInfoBox({"Chambre non valide ou non occupee."});
                    break;
                }
                std::cout << "Entrez le nombre de jours pour le parking: ";
                int jours;
                std::cin >> jours;
                Parking* parking = new Parking(jours);
                parking->bookService();
                hotel.addServiceToRoom(numChambre, parking);
                system("pause");
                break;
            }
            case 5: {
                 std::cout << "Entrez le numero de chambre pour le service: ";
                int numChambre;
                std::cin >> numChambre;
                 if(hotel.findRoom(numChambre) == nullptr || hotel.findRoom(numChambre)->getDisponibilite()){
                    ConsoleUI::drawInfoBox({"Chambre non valide ou non occupee."});
                    break;
                }
                std::cout << "Entrez le nom du soin spa desire: ";
                std::string soin;
                std::cin.ignore();
                getline(std::cin, soin);
                Spa* spa = new Spa(soin);
                spa->bookService();
                hotel.addServiceToRoom(numChambre, spa);
                system("pause");
                break;
            }
            case 6: {
                 std::cout << "Entrez le numero de chambre pour le service: ";
                int numChambre;
                std::cin >> numChambre;
                 if(hotel.findRoom(numChambre) == nullptr || hotel.findRoom(numChambre)->getDisponibilite()){
                    ConsoleUI::drawInfoBox({"Chambre non valide ou non occupee."});
                    break;
                }
                std::cout << "Entrez le nombre d'heures pour la salle de sport: ";
                int heures;
                std::cin >> heures;
                Gym* gym = new Gym(heures);
                gym->bookService();
                hotel.addServiceToRoom(numChambre, gym);
                system("pause");
                break;
            }
            case 7: return;
            default: ConsoleUI::drawInfoBox({"Choix invalide!"}); system("pause");
        }
    }
*/