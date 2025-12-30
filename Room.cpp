#include "Room.h"
#include "HotelServices.h"
#include <iostream>

Room::Room(int numero, std::string type, double prix) 
    : numeroChambre(numero), typeChambre(type), prix(prix), estDisponible(true), nomClient(""), nbNuits(0) {}

void Room::addService(HotelService* service) {
    servicesUtilises.push_back(service);
}

const std::vector<HotelService*>& Room::getServicesUtilises() const {
    return servicesUtilises;
}

void Room::reserverChambre(const std::string& nom, int nuits) {
    if (estDisponible) {
        nomClient = nom;
        nbNuits = nuits;
        estDisponible = false;
        std::cout << "Chambre " << numeroChambre << " reservee pour " << nom << " (" << nuits << " nuits)." << std::endl;
    } else {
        std::cout << "Chambre " << numeroChambre << " est deja occupee!" << std::endl;
    }
}

void Room::libererChambre() {
    if (!estDisponible) {
        std::cout << "Client " << nomClient << " a libere la chambre " << numeroChambre << std::endl;
        nomClient = "";
        nbNuits = 0;
        estDisponible = true;
        // Clear services, but don't delete pointers as they are managed by the Hotel class
        servicesUtilises.clear();
    } else {
        std::cout << "Chambre " << numeroChambre << " est deja libre!" << std::endl;
    }
}

void Room::afficherInfo() const {
    std::cout << "Chambre " << numeroChambre << " | Type: " << typeChambre 
         << " | Prix: " << prix << " DH/nuit | Statut: " 
         << (estDisponible ? "Disponible" : "Occupee");
    if (!estDisponible) {
        std::cout << " | Client: " << nomClient << " (" << nbNuits << " nuits)";
    }
    std::cout << std::endl;
}
