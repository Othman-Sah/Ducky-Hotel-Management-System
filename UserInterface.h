#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "Hotel.h"
#include "User.h"
#include "RetroUI.h"
#include "HotelServices.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace UserUI {

    // Helper to display menu
    inline void drawRestaurantMenu() {
        RetroUI::gotoxy(2, 5); std::cout << RetroUI::YELLOW << "--- MENU ---" << RetroUI::RESET;
        int y = 6;
        for (const auto& category : Restaurant::menu) {
            if (y > 20) break;
            RetroUI::gotoxy(2, y++); std::cout << RetroUI::BRIGHT_ORANGE << "[" << category.first << "]";
            for (const auto& item : category.second) {
                if (y > 20) break;
                RetroUI::gotoxy(4, y++); std::cout << RetroUI::WHITE << item.name << " - $" << item.price;
            }
        }
        RetroUI::gotoxy(1, 22); std::cout << RetroUI::ORANGE << "╠" << std::string(RetroUI::CONTENT_WIDTH - 2, '═') << "╣";
    }

    inline void orderRestaurant(Hotel& hotel) {
        std::vector<MenuItem> commande;
        
        while(true) {
            RetroUI::drawLayout("RESTAURANT", hotel.getRoomStatusList());
            drawRestaurantMenu();
            
            // Display current order
            RetroUI::gotoxy(40, 5); std::cout << RetroUI::YELLOW << "--- VOTRE COMMANDE ---" << RetroUI::RESET;
            int oy = 6;
            double total = 0;
            for(const auto& item : commande) {
                RetroUI::gotoxy(40, oy++); 
                std::cout << RetroUI::WHITE << "- " << item.name << " ($" << item.price << ")";
                total += item.price;
            }
            RetroUI::gotoxy(40, 21); std::cout << "Total: $" << total;

            // Options
            RetroUI::gotoxy(2, 24); std::cout << "1. Ajouter Article";
            RetroUI::gotoxy(2, 25); std::cout << "2. Finaliser Commande";
            RetroUI::gotoxy(2, 26); std::cout << "3. Retour";

            std::string c = RetroUI::textInput("Choix", 28);
            int choice = std::atoi(c.c_str());

            if (choice == 1) {
                std::string nomArticle = RetroUI::textInput("Nom Article", 30);
                bool found = false;
                for (const auto& category : Restaurant::menu) {
                    for (const auto& item : category.second) {
                        if (item.name == nomArticle) {
                            commande.push_back(item);
                            found = true;
                            break;
                        }
                    }
                    if (found) break;
                }
                if (!found) {
                    RetroUI::drawInfoBox({"Article non trouve!"});
                    system("pause > nul");
                }
            } else if (choice == 2) {
                if (commande.empty()) {
                    RetroUI::drawInfoBox({"Commande vide!"});
                    system("pause > nul");
                    continue;
                }
                
                std::string roomStr = RetroUI::textInput("Numero Chambre", 30);
                int roomNum = std::atoi(roomStr.c_str());
                
                Room* room = hotel.findRoom(roomNum);
                if (room && !room->getDisponibilite()) {
                    RoomService* rs = new RoomService(roomNum, commande);
                    hotel.addServiceToRoom(roomNum, rs);
                    RetroUI::drawInfoBox({"Commande envoyee en cuisine!", "Merci!"});
                    system("pause > nul");
                    return;
                } else {
                    RetroUI::drawInfoBox({"Chambre invalide ou libre."});
                    system("pause > nul");
                }
            } else if (choice == 3) {
                return;
            }
        }
    }

    inline void bookParking(Hotel& hotel) {
        RetroUI::drawLayout("RESERVATION PARKING", hotel.getRoomStatusList());
        
        std::vector<std::string> info;
        info.push_back("Places Libres: " + std::to_string(ParkingManager::getAvailableSpots()));
        info.push_back("Tarif: $25 / jour");
        RetroUI::drawInfoBox(info);

        if (ParkingManager::getAvailableSpots() > 0) {
            std::string roomStr = RetroUI::textInput("Numero Chambre", 20);
            int roomNum = std::atoi(roomStr.c_str());
            
            std::string daysStr = RetroUI::textInput("Nombre Jours  ", 22);
            int days = std::atoi(daysStr.c_str());
            
            Room* room = hotel.findRoom(roomNum);
            if (room && !room->getDisponibilite()) {
                if (ParkingManager::occupySpot()) {
                    Parking* p = new Parking(days);
                    hotel.addServiceToRoom(roomNum, p);
                    RetroUI::drawInfoBox({"Place reservee avec succes!"});
                } else {
                    RetroUI::drawInfoBox({"Erreur technique (Plus de place)."});
                }
            } else {
                RetroUI::drawInfoBox({"Chambre invalide."});
            }
        } else {
            RetroUI::drawInfoBox({"Desole, le parking est complet."});
        }
        system("pause > nul");
    }

    inline void bookSpaAndGym(Hotel& hotel) {
        while(true) {
            int choice = RetroUI::showMenu("SPA & GYM", {"Reserver Spa", "Reserver Gym", "Retour"}, hotel.getRoomStatusList());
            
            if (choice == 3) return;

            RetroUI::drawLayout(choice == 1 ? "RESERVATION SPA" : "RESERVATION GYM", hotel.getRoomStatusList());
            
            std::string roomStr = RetroUI::textInput("Numero Chambre", 12);
            int roomNum = std::atoi(roomStr.c_str());
            
            Room* room = hotel.findRoom(roomNum);
            if (room && !room->getDisponibilite()) {
                if (choice == 1) {
                    // Spa
                    RetroUI::gotoxy(2, 15); std::cout << "Soins disponibles: Massage ($50), Facial ($50), Sauna ($50)";
                    std::string treatment = RetroUI::textInput("Type de Soin", 17);
                    hotel.addServiceToRoom(roomNum, new Spa(treatment));
                    RetroUI::drawInfoBox({"Rendez-vous Spa confirme!"});
                } else {
                    // Gym
                    std::string hoursStr = RetroUI::textInput("Nombre Heures ($10/h)", 15);
                    int h = std::atoi(hoursStr.c_str());
                    hotel.addServiceToRoom(roomNum, new Gym(h));
                    RetroUI::drawInfoBox({"Acces Gym confirme!"});
                }
            } else {
                RetroUI::drawInfoBox({"Chambre invalide."});
            }
            system("pause > nul");
        }
    }

    inline void showUserInfo(Hotel& hotel, User* user) {
        RetroUI::drawLayout("MES INFORMATIONS");
        
        const Room* userRoom = nullptr;
        for(const auto& room : hotel.getChambres()) {
            if(!room.getDisponibilite() && room.getNomClient() == user->getUsername()) {
                userRoom = &room;
                break;
            }
        }

        if (userRoom) {
            std::vector<std::string> info;
            info.push_back("Chambre: " + std::to_string(userRoom->getNumeroChambre()));
            info.push_back("Type: " + userRoom->getTypeChambre());
            
            // Dates (Simulated)
            std::time_t now = std::time(nullptr);
            std::tm* ltm = std::localtime(&now);
            char buffer[80];
            std::strftime(buffer, sizeof(buffer), "%d/%m/%Y", ltm);
            info.push_back("Check-in: " + std::string(buffer));
            
            std::time_t checkout = now + (userRoom->getNbNuits() * 24 * 3600);
            std::tm* ctm = std::localtime(&checkout);
            std::strftime(buffer, sizeof(buffer), "%d/%m/%Y", ctm);
            info.push_back("Checkout Prevu: " + std::string(buffer));
            
            info.push_back("Temps Restant: " + std::to_string(userRoom->getNbNuits()) + " jours");

            // Financials
            double roomCost = userRoom->getPrix() * userRoom->getNbNuits();
            double servicesCost = 0;
            for(auto* s : userRoom->getServicesUtilises()) {
                servicesCost += s->getPrice();
            }
            
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << servicesCost;
            info.push_back("Depenses Services: $" + ss.str());
            
            ss.str("");
            ss << std::fixed << std::setprecision(2) << (roomCost + servicesCost);
            info.push_back("Total a Payer: $" + ss.str());
            
            RetroUI::drawInfoBox(info);
        } else {
            RetroUI::drawInfoBox({"Aucune reservation active."});
        }
        system("pause > nul");
    }

    inline void dashboard(Hotel& hotel, User* user) {
        int choix;
        while (true) {
            std::vector<std::string> sidebar;
            for(const auto& room : hotel.getChambres()) {
                if(!room.getDisponibilite() && room.getNomClient() == user->getUsername()) {
                    sidebar.push_back(std::to_string(room.getNumeroChambre()) + ":OCCUPIED:" + room.getTypeChambre());
                }
            }

            choix = RetroUI::showMenu("ESPACE CLIENT - " + user->getUsername(), {
                "Voir mes infos (Simule)", 
                "Restaurant (Menu & Commande)", 
                "Reserver Parking", 
                "Spa & Gym", 
                "Deconnexion"
            }, sidebar);

            switch(choix) {
                case 1: 
                    showUserInfo(hotel, user);
                    break;
                case 2: orderRestaurant(hotel); break;
                case 3: bookParking(hotel); break;
                case 4: bookSpaAndGym(hotel); break;
                case 5: return;
            }
        }
    }
}
#endif