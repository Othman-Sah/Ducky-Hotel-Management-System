#include "Hotel.h"
#include "Bill.h"
#include "HotelServices.h"
#include <iostream>
#include <numeric>
#include <algorithm>
#include <fstream>
#include <sstream>

Hotel::Hotel(const std::string& nom) : nomHotel(nom) {
    std::cout << "Bienvenue a " << nomHotel << "!" << std::endl;
}

Hotel::~Hotel() {
    for (auto service : services) {
        delete service;
    }
}

void Hotel::ajouterChambre(int numero, const std::string& type, double prix) {
    chambres.push_back(Room(numero, type, prix));
}

void Hotel::afficherToutesChambres() const {
    std::cout << "\n=== Toutes les chambres de " << nomHotel << " ===" << std::endl;
    for (const auto& chambre : chambres) {
        chambre.afficherInfo();
    }
    std::cout << std::endl;
}

void Hotel::afficherChambresDisponibles() const {
    std::cout << "\n=== Chambres Disponibles ===" << std::endl;
    bool trouve = false;
    for (const auto& chambre : chambres) {
        if (chambre.getDisponibilite()) {
            chambre.afficherInfo();
            trouve = true;
        }
    }
    if (!trouve) {
        std::cout << "Aucune chambre disponible!" << std::endl;
    }
    std::cout << std::endl;
}

void Hotel::reserverChambre(int numeroChambre, const std::string& nomClient, int nuits) {
    Room* room = findRoom(numeroChambre);
    if (room) {
        room->reserverChambre(nomClient, nuits);
        saveReservations();
    } else {
        std::cout << "Chambre " << numeroChambre << " introuvable!" << std::endl;
    }
}

void Hotel::libererChambre(int numeroChambre) {
    Room* room = findRoom(numeroChambre);
    if (room && !room->getDisponibilite()) {
        Bill bill(*room);
        for(auto* service : room->getServicesUtilises()){
            bill.addService(service);
        }
        bill.generateBill();
        bill.saveToFile();
        room->libererChambre();
        saveReservations();
    } else if (room) {
        std::cout << "Chambre " << numeroChambre << " est deja libre!" << std::endl;
    } 
    else {
        std::cout << "Chambre " << numeroChambre << " introuvable!" << std::endl;
    }
}

void Hotel::rechercherChambre(int numeroChambre) const {
    for (const auto& chambre : chambres) {
        if (chambre.getNumeroChambre() == numeroChambre) {
            std::cout << "\n=== Details de la Chambre ===" << std::endl;
            chambre.afficherInfo();
            std::cout << std::endl;
            return;
        }
    }
    std::cout << "Chambre " << numeroChambre << " introuvable!" << std::endl;
}

void Hotel::addService(HotelService* service) {
    services.push_back(service);
}

void Hotel::addServiceToRoom(int roomNumber, HotelService* service) {
    Room* room = findRoom(roomNumber);
    if (room) {
        room->addService(service);
        services.push_back(service); // Also add to hotel's main service list for memory management
    } else {
        std::cout << "Cannot add service, room " << roomNumber << " not found." << std::endl;
        delete service; // cleanup if room not found
    }
}


void Hotel::displayServices() const {
    std::cout << "\n=== Services de l'Hotel ===" << std::endl;
    for (const auto& service : services) {
        service->displayService();
    }
    std::cout << std::endl;
}

Room* Hotel::findRoom(int roomNumber) {
    for (auto& chambre : chambres) {
        if (chambre.getNumeroChambre() == roomNumber) {
            return &chambre;
        }
    }
    return nullptr;
}

int Hotel::getOccupiedRoomsCount() const {
    return std::count_if(chambres.begin(), chambres.end(), [](const Room& r){ return !r.getDisponibilite(); });
}

int Hotel::getAvailableRoomsCount() const {
    return std::count_if(chambres.begin(), chambres.end(), [](const Room& r){ return r.getDisponibilite(); });
}

double Hotel::getTotalRevenue() const {
    double revenue = 0.0;
    for(const auto& room : chambres) {
        if(!room.getDisponibilite()) {
            revenue += room.getPrix() * (room.getNbNuits() > 0 ? room.getNbNuits() : 1);
        }
    }
    for(const auto& service : services) {
        revenue += service->getPrice();
    }
    return revenue;
}

void Hotel::saveReservations() const {
    std::ofstream file("reservations.txt");
    if (file.is_open()) {
        for (const auto& room : chambres) {
            if (!room.getDisponibilite()) {
                // Format: Number Nights ClientName
                file << room.getNumeroChambre() << " " << room.getNbNuits() << " " << room.getNomClient() << "\n";
                for (auto* s : room.getServicesUtilises()) {
                    file << "SERVICE " << s->getServiceType() << " " << s->getServiceData() << "\n";
                }
                file << "END_ROOM\n";
            }
        }
        file.close();
    }
}

void Hotel::loadReservations() {
    std::ifstream file("reservations.txt");
    if (file.is_open()) {
        int num, nuits;
        std::string client;
        while (file >> num >> nuits) {
            std::getline(file >> std::ws, client); // Read rest of line as name
            Room* r = findRoom(num);
            if (r) {
                r->reserverChambre(client, nuits);
                
                // Load Services
                std::string marker;
                while(file >> marker) {
                    if (marker == "END_ROOM") break;
                    if (marker == "SERVICE") {
                        std::string type, data;
                        file >> type;
                        std::getline(file >> std::ws, data);
                        
                        HotelService* service = nullptr;
                        if (type == "Parking") {
                            service = new Parking(std::stoi(data));
                            ParkingManager::occupySpot(); // Sync manager
                        } else if (type == "Spa") {
                            service = new Spa(data);
                        } else if (type == "Gym") {
                            service = new Gym(std::stoi(data));
                        } else if (type == "RoomService") {
                            std::vector<MenuItem> items;
                            std::stringstream ss(data);
                            std::string segment;
                            while(std::getline(ss, segment, ';')) {
                                size_t sep = segment.find(':');
                                if(sep != std::string::npos) {
                                    items.push_back({segment.substr(0, sep), std::stod(segment.substr(sep+1))});
                                }
                            }
                            service = new RoomService(num, items);
                        }
                        
                        if (service) {
                            addServiceToRoom(num, service);
                        }
                    }
                }
            }
        }
        file.close();
    }
}

std::vector<std::string> Hotel::getRoomStatusList() const {
    std::vector<std::string> list;
    std::vector<Room> sortedRooms = chambres;
    std::sort(sortedRooms.begin(), sortedRooms.end(), [](const Room& a, const Room& b) {
        return a.getNumeroChambre() < b.getNumeroChambre();
    });

    int currentFloor = -1;
    for (const auto& room : sortedRooms) {
        int floor = room.getNumeroChambre() / 100;
        if (floor != currentFloor) {
            currentFloor = floor;
            list.push_back("--- ETAGE " + std::to_string(floor) + " ---");
        }
        std::string status = room.getDisponibilite() ? "FREE" : "OCCUPIED";
        list.push_back(std::to_string(room.getNumeroChambre()) + ":" + status + ":" + room.getTypeChambre());
    }
    return list;
}

std::vector<std::string> Hotel::getRoomListDetails(bool onlyAvailable) const {
    std::vector<std::string> list;
    for (const auto& room : chambres) {
        if (onlyAvailable && !room.getDisponibilite()) continue;
        
        std::stringstream ss;
        ss << "Chambre " << room.getNumeroChambre() << " | " << room.getTypeChambre() 
           << " | " << room.getPrix() << " DH";
        if (!room.getDisponibilite()) {
            ss << " | " << room.getNomClient();
        } else {
            ss << " | DISPONIBLE";
        }
        list.push_back(ss.str());
    }
    return list;
}

Bill Hotel::prepareBill(int roomNumber) {
    Room* room = findRoom(roomNumber);
    if (!room) throw std::runtime_error("Room not found");
    Bill bill(*room);
    for(auto* service : room->getServicesUtilises()){
        bill.addService(service);
    }
    return bill;
}

const std::vector<Room>& Hotel::getChambres() const {
    return chambres;
}