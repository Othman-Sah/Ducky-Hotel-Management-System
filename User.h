#ifndef USER_H
#define USER_H

#include <string>

enum class Role {
    USER,
    ADMIN
};

class User {
public:
    User(const std::string& username, const std::string& password, Role role);

    std::string getUsername() const;
    Role getRole() const;
    bool checkPassword(const std::string& password) const;

private:
    std::string username;
    std::string password; // In a real application, this should be hashed
    Role role;
};

#endif // USER_H
