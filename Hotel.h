#ifndef HOTEL_H
#define HOTEL_H

#include "Room.h"
#include <vector>
using namespace std;

class HotelService;
class Bill;

class Hotel {
private:
    string nomHotel;
    vector<Room> chambres;
    vector<HotelService*> services;

public:
    Hotel(const string& nom);
    ~Hotel();
    
    void ajouterChambre(int numero, const string& type, double prix);
    void afficherToutesChambres() const;
    void afficherChambresDisponibles() const;
    void reserverChambre(int numeroChambre, const string& nomClient, int nuits);
    void libererChambre(int numeroChambre);
    void rechercherChambre(int numeroChambre) const;

    void addService(HotelService* service);
    void addServiceToRoom(int roomNumber, HotelService* service);
    void displayServices() const;
    Room* findRoom(int roomNumber);
    
    int getOccupiedRoomsCount() const;
    int getAvailableRoomsCount() const;
    double getTotalRevenue() const;

    void saveReservations() const;
    void loadReservations();

    std::vector<std::string> getRoomStatusList() const;
    std::vector<std::string> getRoomListDetails(bool onlyAvailable) const;
    Bill prepareBill(int roomNumber);
    const std::vector<Room>& getChambres() const;
};

#endif
