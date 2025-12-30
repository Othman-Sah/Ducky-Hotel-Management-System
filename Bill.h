#ifndef BILL_H
#define BILL_H

#include "Room.h"
#include "HotelServices.h"
#include <vector>
#include <string>

class Bill {
public:
    Bill(const Room& room);
    void addService(HotelService* service);
    void generateBill() const;
    double getTotalAmount() const;
    void saveToFile() const;
    std::vector<std::string> getBillLines() const;

private:
    const Room& room;
    std::vector<HotelService*> services;
};

#endif // BILL_H
