#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "Hotel.h"
#include "User.h"
#include "RetroUI.h"
#include "HotelServices.h"
#include <iostream>
#include <vector>
#include <string>

namespace UserUI {

    inline std::vector<MenuItem> commanderDuMenu() {
        std::vector<MenuItem> commande;
        std::string nomArticle;
        while (true) {
            std::cout << "Entrez le nom de l'article a commander (ou 'fin' pour terminer): ";
            std::cin >> nomArticle;
            if (nomArticle == "fin") break;

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
            if (!articleTrouve) std::cout << "Article non trouve.\n";
        }
        return commande;
    }

    inline void orderRestaurant(Hotel& hotel) {
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

    inline void bookParking(Hotel& hotel) {
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
                std::cout << "Place reservee!\n";
            }
        } else {
            std::cout << "Parking complet!\n";
        }
        system("pause");
    }

    inline void bookSpaAndGym(Hotel& hotel) {
        std::cout << "Entrez votre numero de chambre: ";
        int roomNum;
        std::cin >> roomNum;
        Room* room = hotel.findRoom(roomNum);
        
        if (room && !room->getDisponibilite()) {
            std::cout << "1. Spa\n2. Gym\nChoix: ";
            int c; std::cin >> c;
            if (c == 1) {
                std::string treatment;
                std::cout << "Soin (Massage/Facial): "; std::cin >> treatment;
                hotel.addServiceToRoom(roomNum, new Spa(treatment));
            } else {
                int h; std::cout << "Heures: "; std::cin >> h;
                hotel.addServiceToRoom(roomNum, new Gym(h));
            }
            std::cout << "Reservation effectuee!\n";
        } else {
            std::cout << "Chambre invalide.\n";
        }
        system("pause");
    }

    inline void dashboard(Hotel& hotel, User* user) {
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
                case 1: std::cout << "Fonctionnalite a venir.\n"; system("pause"); break;
                case 2: orderRestaurant(hotel); break;
                case 3: bookParking(hotel); break;
                case 4: bookSpaAndGym(hotel); break;
                case 5: return;
            }
        }
    }
}
#endif