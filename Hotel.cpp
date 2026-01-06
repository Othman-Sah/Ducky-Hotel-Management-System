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
    
    // Generate default rooms.csv if it doesn't exist
    std::ifstream checkRoom("rooms.csv");
    if (!checkRoom.is_open()) {
        std::ofstream outRoom("rooms.csv");
        outRoom << "Number,Type,Price\n";
        outRoom << "101,Simple,500\n";
        outRoom << "102,Simple,500\n";
        outRoom << "103,Simple,500\n";
        outRoom << "104,Double,700\n";
        outRoom << "105,Double,700\n";
        outRoom << "106,Double,700\n";
        outRoom << "107,Suite,900\n";
        outRoom << "108,Simple,500\n";
        outRoom << "201,Double,800\n";
        outRoom << "202,Double,800\n";
        outRoom << "203,Suite,1200\n";
        outRoom << "204,Suite,1200\n";
        outRoom << "205,Double,800\n";
        outRoom << "206,Double,800\n";
        outRoom << "207,Suite,1200\n";
        outRoom << "208,Double,800\n";
        outRoom << "301,Suite,1500\n";
        outRoom << "302,Presid.,2500\n";
        outRoom << "303,Presid.,2500\n";
        outRoom << "304,Suite,1500\n";
        outRoom << "305,Suite,1500\n";
        outRoom << "306,Suite,1500\n";
        outRoom << "307,Presid.,2500\n";
        outRoom << "308,Suite,1500\n";
        outRoom.close();
        std::cout << "Generated default rooms.csv" << std::endl;
    } else {
        checkRoom.close();
    }

    // Generate empty reservations files if they don't exist
    std::ifstream checkRes("reservations.csv");
    if (!checkRes.is_open()) {
        std::ofstream outRes("reservations.csv");
        outRes << "RoomNumber,ClientName,Nights\n";
        outRes.close();
    } else {
        checkRes.close();
    }

    std::ifstream checkSvc("reservation_services.csv");
    if (!checkSvc.is_open()) {
        std::ofstream outSvc("reservation_services.csv");
        outSvc << "RoomNumber,ServiceType,ServiceData\n";
        outSvc.close();
    } else {
        checkSvc.close();
    }

    // Load rooms from CSV if available
    std::ifstream roomFile("rooms.csv");
    if (roomFile.is_open()) {
        std::string line;
        std::getline(roomFile, line); // Skip header
        while (std::getline(roomFile, line)) {
            std::stringstream ss(line);
            std::string segment;
            std::vector<std::string> row;
            while(std::getline(ss, segment, ',')) {
                row.push_back(segment);
            }
            if(row.size() >= 3) {
                try {
                    ajouterChambre(std::stoi(row[0]), row[1], std::stod(row[2]));
                } catch (...) {}
            }
        }
        roomFile.close();
    }
}

Hotel::~Hotel() {
    for (auto service : services) {
        delete service;
    }
}

void Hotel::ajouterChambre(int numero, const std::string& type, double prix) {
    // Prevent duplicates (e.g. if main.cpp tries to add hardcoded rooms that were already loaded from CSV)
    for (const auto& room : chambres) {
        if (room.getNumeroChambre() == numero) return;
    }
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
    std::ofstream resFile("reservations.csv");
    std::ofstream svcFile("reservation_services.csv");

    if (resFile.is_open()) {
        resFile << "RoomNumber,ClientName,Nights\n";
        for (const auto& room : chambres) {
            if (!room.getDisponibilite()) {
                resFile << room.getNumeroChambre() << "," << room.getNomClient() << "," << room.getNbNuits() << "\n";
            }
        }
        resFile.close();
    }

    if (svcFile.is_open()) {
        svcFile << "RoomNumber,ServiceType,ServiceData\n";
        for (const auto& room : chambres) {
            if (!room.getDisponibilite()) {
                for (auto* s : room.getServicesUtilises()) {
                    svcFile << room.getNumeroChambre() << "," << s->getServiceType() << "," << s->getServiceData() << "\n";
                }
            }
        }
        svcFile.close();
    }
}

void Hotel::loadReservations() {
    // Load Reservations
    std::ifstream resFile("reservations.csv");
    if (resFile.is_open()) {
        std::string line;
        std::getline(resFile, line); // Skip header
        while (std::getline(resFile, line)) {
            std::stringstream ss(line);
            std::string segment;
            std::vector<std::string> row;
            while(std::getline(ss, segment, ',')) {
                row.push_back(segment);
            }
            if (row.size() >= 3) {
                try {
                    int num = std::stoi(row[0]);
                    std::string client = row[1];
                    int nuits = std::stoi(row[2]);
                    Room* r = findRoom(num);
                    if (r) {
                        r->reserverChambre(client, nuits);
                    }
                } catch (...) {}
            }
        }
        resFile.close();
    }

    // Load Services
    std::ifstream svcFile("reservation_services.csv");
    if (svcFile.is_open()) {
        std::string line;
        std::getline(svcFile, line); // Skip header
        while (std::getline(svcFile, line)) {
            std::stringstream ss(line);
            std::string segment;
            std::vector<std::string> row;
            while(std::getline(ss, segment, ',')) {
                row.push_back(segment);
            }
            
            if (row.size() >= 3) {
                try {
                    int num = std::stoi(row[0]);
                    std::string type = row[1];
                    std::string data = row[2];

                    Room* r = findRoom(num);
                    if (r) {
                        HotelService* service = nullptr;
                        if (type == "Parking") {
                            service = new Parking(std::stoi(data));
                            ParkingManager::occupySpot(); 
                        } else if (type == "Spa") {
                            service = new Spa(data);
                        } else if (type == "Gym") {
                            service = new Gym(std::stoi(data));
                        } else if (type == "RoomService") {
                            std::vector<MenuItem> items;
                            std::stringstream ssData(data);
                            std::string itemSegment;
                            while(std::getline(ssData, itemSegment, ';')) {
                                size_t sep = itemSegment.find(':');
                                if(sep != std::string::npos) {
                                    items.push_back({itemSegment.substr(0, sep), std::stod(itemSegment.substr(sep+1))});
                                }
                            }
                            service = new RoomService(num, items);
                        }
                        
                        if (service) {
                            addServiceToRoom(num, service);
                        }
                    }
                } catch (...) {}
            }
        }
        svcFile.close();
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