#include "Bill.h"
#include <iostream>
#include <numeric>
#include <fstream>
#include <sstream>
#include <direct.h> // For _mkdir on Windows
#include <ctime>
#include <iomanip>

Bill::Bill(const Room& room) : room(room) {}

void Bill::addService(HotelService* service) {
    services.push_back(service);
}

double Bill::getTotalAmount() const {
    double total = room.getPrix() * (room.getNbNuits() > 0 ? room.getNbNuits() : 1);
    total += std::accumulate(services.begin(), services.end(), 0.0,
        [](double sum, const HotelService* service) {
            return sum + service->getPrice();
        });
    return total;
}

void Bill::generateBill() const {
    std::cout << "\n--- Facture pour la Chambre " << room.getNumeroChambre() << " ---" << std::endl;
    std::cout << "Client: " << room.getNomClient() << std::endl;
    std::cout << "------------------------------------" << std::endl;
    std::cout << "Prix par nuit:      $" << room.getPrix() << std::endl;
    std::cout << "Nombre de nuits:    " << room.getNbNuits() << std::endl;
    std::cout << "Total Sejour:       $" << (room.getPrix() * room.getNbNuits()) << std::endl;
    
    if (!services.empty()) {
        std::cout << "\nServices utilises:" << std::endl;
        for (const auto& service : services) {
            service->displayService();
        }
    }

    std::cout << "------------------------------------" << std::endl;
    std::cout << "Montant Total: $" << getTotalAmount() << std::endl;
    std::cout << "------------------------------------" << std::endl;
}

void Bill::saveToFile() const {
    // Create directory if it doesn't exist
    _mkdir("billing");
    
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::stringstream ss;
    ss << "billing/" << room.getNomClient() << "_" << std::put_time(&tm, "%m_%d_%y") << ".txt";

    std::string filename = ss.str();
    std::ofstream file(filename);
    
    if (file.is_open()) {
        file << "================ FACTURE ================\n";
        file << "Client: " << room.getNomClient() << "\n";
        file << "Chambre: " << room.getNumeroChambre() << "\n";
        file << "-----------------------------------------\n";
        file << "Prix par nuit:      $" << room.getPrix() << "\n";
        file << "Nombre de nuits:    " << room.getNbNuits() << "\n";
        file << "Sous-total Chambre: $" << (room.getPrix() * room.getNbNuits()) << "\n";
        file << "-----------------------------------------\n";
        
        if (!services.empty()) {
            file << "Services:\n";
            for (const auto& service : services) {
                file << "- " << service->getServiceName() << " ($" << service->getPrice() << ")\n";
                
                // Add details based on type
                std::string type = service->getServiceType();
                std::string data = service->getServiceData();
                
                if (type == "RoomService" && !data.empty()) {
                    std::stringstream ss(data);
                    std::string item;
                    while (std::getline(ss, item, ';')) {
                        size_t sep = item.find(':');
                        if (sep != std::string::npos) {
                            file << "    * " << item.substr(0, sep) << " ($" << item.substr(sep + 1) << ")\n";
                        }
                    }
                } else if (type == "Parking") {
                    file << "    Details: " << data << " jours\n";
                } else if (type == "Gym") {
                    file << "    Details: " << data << " heures\n";
                }
            }
            file << "-----------------------------------------\n";
        }
        
        file << "TOTAL A PAYER: $" << getTotalAmount() << "\n";
        file << "=========================================\n";
        file.close();
        std::cout << "Facture sauvegardee dans " << filename << std::endl;
    } else {
        std::cerr << "Erreur: Impossible de creer le fichier de facture." << std::endl;
    }
}

std::vector<std::string> Bill::getBillLines() const {
    std::vector<std::string> lines;
    std::stringstream ss;
    
    ss << "--- Facture Chambre " << room.getNumeroChambre() << " ---"; lines.push_back(ss.str()); ss.str("");
    ss << "Client: " << room.getNomClient(); lines.push_back(ss.str()); ss.str("");
    lines.push_back("------------------------------------");
    ss << "Prix par nuit:      $" << room.getPrix(); lines.push_back(ss.str()); ss.str("");
    ss << "Nombre de nuits:    " << room.getNbNuits(); lines.push_back(ss.str()); ss.str("");
    ss << "Total Sejour:       $" << (room.getPrix() * room.getNbNuits()); lines.push_back(ss.str()); ss.str("");
    
    if (!services.empty()) {
        lines.push_back("");
        lines.push_back("Services utilises:");
        for (const auto& service : services) {
            ss << "- " << service->getServiceName() << " ($" << service->getPrice() << ")";
            lines.push_back(ss.str()); ss.str("");
        }
    }
    lines.push_back("------------------------------------");
    ss << "Montant Total: $" << getTotalAmount(); lines.push_back(ss.str()); ss.str("");
    
    return lines;
}
