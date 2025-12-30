#ifndef ROOM_H
#define ROOM_H

#include <string>
#include <vector>
using namespace std;

class HotelService;

class Room {
private:
    int numeroChambre;
    string typeChambre;
    double prix;
    bool estDisponible;
    string nomClient;
    vector<HotelService*> servicesUtilises;
    int nbNuits;

public:
    Room(int numero, string type, double prix);
    
    int getNumeroChambre() const { return numeroChambre; }
    string getTypeChambre() const { return typeChambre; }
    double getPrix() const { return prix; }
    bool getDisponibilite() const { return estDisponible; }
    string getNomClient() const { return nomClient; }
    int getNbNuits() const { return nbNuits; }
    
    void reserverChambre(const string& nom, int nuits);
    void libererChambre();
    void afficherInfo() const;
    void addService(HotelService* service);
    const vector<HotelService*>& getServicesUtilises() const;
};

#endif
