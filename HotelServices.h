#ifndef HOTELSERVICES_H
#define HOTELSERVICES_H

#include "MenuItem.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>


class HotelService {
public:
    HotelService(const std::string& serviceName, double price);
    virtual ~HotelService() = default;

    virtual void bookService() = 0;
    virtual void displayService() const;
    virtual std::string getServiceType() const = 0;
    virtual std::string getServiceData() const = 0;

    std::string getServiceName() const;
    double getPrice() const;

protected:
    std::string serviceName;
    double price;
};

class Parking : public HotelService {
public:
    Parking(int days);
    void bookService() override;
    void displayService() const override;
    std::string getServiceType() const override { return "Parking"; }
    std::string getServiceData() const override;

private:
    int days;
};

class Restaurant : public HotelService {
public:
    Restaurant(const std::vector<MenuItem>& order);
    void bookService() override;
    std::string getServiceType() const override { return "Restaurant"; }
    std::string getServiceData() const override { return ""; } // Not used directly on rooms usually
    
    static void initializeMenu();
    static void displayMenu();
    static std::map<std::string, std::vector<MenuItem>> menu;

private:
    std::vector<MenuItem> order;
};

class Spa : public HotelService {
public:
    Spa(const std::string& treatment);
    void bookService() override;
    void displayService() const override;
    std::string getServiceType() const override { return "Spa"; }
    std::string getServiceData() const override;

private:
    std::string treatment;
};

class Gym : public HotelService {
public:
    Gym(int hours);
    void bookService() override;
    void displayService() const override;
    std::string getServiceType() const override { return "Gym"; }
    std::string getServiceData() const override;
private:
    int hours;
};

class RoomService : public HotelService {
public:
    RoomService(int roomNumber, const std::vector<MenuItem>& order);
    void bookService() override;
    void displayService() const override;
    std::string getServiceType() const override { return "RoomService"; }
    std::string getServiceData() const override;

private:
    int roomNumber;
    std::vector<MenuItem> order;
};

class ParkingManager {
public:
    static void initialize(int total);
    static bool occupySpot();
    static void releaseSpot();
    static int getAvailableSpots();
    static int getTotalSpots();
private:
    static int totalSpots;
    static int occupiedSpots;
};

class RestaurantManager {
public:
    static void addItem(const std::string& category, const std::string& name, double price);
    static void updatePrice(const std::string& name, double newPrice);
    // Wrapper for existing static menu
};

#endif // HOTELSERVICES_H