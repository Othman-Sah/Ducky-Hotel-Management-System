#ifndef ADMIN_INTERFACE_H
#define ADMIN_INTERFACE_H

#include "Hotel.h"
#include "Authentication.h"
#include "User.h"
#include "RetroUI.h"
#include "Bill.h"
#include "HotelServices.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>

namespace AdminUI {

    inline void showDashboard(Hotel& hotel) {
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

    inline void manageRooms(Hotel& hotel, Authentication& auth) {
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

    inline void manageRestaurant(Hotel& hotel) {
        int choice;
        while(true) {
            RetroUI::drawLayout("GESTION RESTAURANT", hotel.getRoomStatusList());
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
            RetroUI::gotoxy(1, 20); std::cout << RetroUI::ORANGE << "╠" << std::string(RetroUI::CONTENT_WIDTH - 2, '═') << "╣";
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

    inline void manageParking() {
        std::vector<std::string> info;
        info.push_back("Total Places: " + std::to_string(ParkingManager::getTotalSpots()));
        info.push_back("Places Libres: " + std::to_string(ParkingManager::getAvailableSpots()));
        RetroUI::drawLayout("ADMIN - Gestion Parking");
        RetroUI::drawInfoBox(info);
        system("pause");
    }

    inline void dashboard(Hotel& hotel, User* user, Authentication& auth) {
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
                case 2: manageRooms(hotel, auth); break;
                case 3: manageRestaurant(hotel); break;
                case 4: manageParking(); break;
                case 5: return;
                default: break;
            }
        }
    }
}
#endif