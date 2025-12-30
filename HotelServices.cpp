#include "HotelServices.h"
#include <iostream>
#include <numeric>
#include <map>
#include <vector>
#include <sstream>

// HotelService Implementation
HotelService::HotelService(const std::string& serviceName, double price)
    : serviceName(serviceName), price(price) {}

void HotelService::displayService() const {
    std::cout << "Service: " << serviceName << ", Price: $" << price << std::endl;
}

std::string HotelService::getServiceName() const {
    return serviceName;
}

double HotelService::getPrice() const {
    return price;
}

// Parking Implementation
Parking::Parking(int days)
    : HotelService("Parking", 25.0 * days), days(days) {}

void Parking::bookService() {
    std::cout << "Parking booked for " << days << " days. Total: $" << getPrice() << std::endl;
}

void Parking::displayService() const {
    std::cout << "Parking (" << days << " days) - $" << price << std::endl;
}

std::string Parking::getServiceData() const {
    return std::to_string(days);
}

// Restaurant Implementation
std::map<std::string, std::vector<MenuItem>> Restaurant::menu;

void Restaurant::initializeMenu() {
    menu["Breakfast"] = { {"Pancakes", 15.0}, {"Omelette", 12.0}, {"Coffee", 3.0} };
    menu["Lunch"] = { {"Burger", 20.0}, {"Salad", 18.0}, {"Pizza", 25.0} };
    menu["Dinner"] = { {"Steak", 45.0}, {"Pasta", 30.0}, {"Fish", 35.0} };
    menu["Drinks"] = { {"Soda", 4.0}, {"Juice", 5.0}, {"Wine", 10.0} };
}

void Restaurant::displayMenu() {
    std::cout << "\n--- Restaurant Menu ---" << std::endl;
    for (const auto& category : menu) {
        std::cout << "\n[" << category.first << "]" << std::endl;
        for (const auto& item : category.second) {
            std::cout << "- " << item.name << ": $" << item.price << std::endl;
        }
    }
}

// RoomService Implementation
RoomService::RoomService(int roomNumber, const std::vector<MenuItem>& order)
    : HotelService("Room Service", 0.0), roomNumber(roomNumber), order(order) {
    double total = 0.0;
    for (const auto& item : order) {
        total += item.price;
    }
    this->price = total;
}

void RoomService::bookService() {
    std::cout << "Room Service booked for Room " << roomNumber << ". Total: $" << price << std::endl;
}

void RoomService::displayService() const {
    std::cout << "Room Service (Items: ";
    for(const auto& item : order) std::cout << item.name << " ";
    std::cout << ") - $" << price << std::endl;
}

std::string RoomService::getServiceData() const {
    std::stringstream ss;
    for (size_t i = 0; i < order.size(); ++i) {
        ss << order[i].name << ":" << order[i].price;
        if (i < order.size() - 1) ss << ";";
    }
    return ss.str();
}

// Spa Implementation
Spa::Spa(const std::string& treatment)
    : HotelService("Spa: " + treatment, 50.0), treatment(treatment) {}

void Spa::bookService() {
    std::cout << "Spa treatment '" << treatment << "' booked. Price: $" << price << std::endl;
}

void Spa::displayService() const {
    std::cout << "Spa (" << treatment << ") - $" << price << std::endl;
}

std::string Spa::getServiceData() const {
    return treatment;
}

// Gym Implementation
Gym::Gym(int hours)
    : HotelService("Gym Access", 10.0 * hours), hours(hours) {}

void Gym::bookService() {
    std::cout << "Gym access booked for " << hours << " hours. Price: $" << price << std::endl;
}

void Gym::displayService() const {
    std::cout << "Gym (" << hours << " hours) - $" << price << std::endl;
}

std::string Gym::getServiceData() const {
    return std::to_string(hours);
}

// ParkingManager Implementation
int ParkingManager::totalSpots = 0;
int ParkingManager::occupiedSpots = 0;

void ParkingManager::initialize(int total) { totalSpots = total; occupiedSpots = 0; }

bool ParkingManager::occupySpot() {
    if (occupiedSpots < totalSpots) {
        occupiedSpots++;
        return true;
    }
    return false;
}

void ParkingManager::releaseSpot() {
    if (occupiedSpots > 0) occupiedSpots--;
}

int ParkingManager::getAvailableSpots() { return totalSpots - occupiedSpots; }
int ParkingManager::getTotalSpots() { return totalSpots; }

void RestaurantManager::addItem(const std::string& category, const std::string& name, double price) {
    Restaurant::menu[category].push_back({name, price});
}

void RestaurantManager::updatePrice(const std::string& name, double newPrice) {
    for (auto& cat : Restaurant::menu) {
        for (auto& item : cat.second) {
            if (item.name == name) {
                item.price = newPrice;
                return;
            }
        }
    }
}