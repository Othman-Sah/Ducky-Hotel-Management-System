#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include "User.h"
#include <vector>
#include <string>

class Authentication {
public:
    Authentication();
    User* login(const std::string& username, const std::string& password);
    void addUser(const std::string& username, const std::string& password, Role role);

private:
    std::vector<User> users;
};

#endif // AUTHENTICATION_H
