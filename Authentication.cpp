#include "Authentication.h"
#include <fstream>
#include <sstream>

Authentication::Authentication() {
    std::ifstream file("users.txt");
    if (file.is_open()) {
        std::string line, username, password;
        int roleInt;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            if (ss >> username >> password >> roleInt) {
                users.emplace_back(username, password, static_cast<Role>(roleInt));
            }
        }
        file.close();
    } else {
        // Defaults if file doesn't exist
        addUser("admin", "admin123", Role::ADMIN);
        addUser("user", "user123", Role::USER);
    }
}

User* Authentication::login(const std::string& username, const std::string& password) {
    for (auto& user : users) {
        if (user.getUsername() == username && user.checkPassword(password)) {
            return &user;
        }
    }
    return nullptr;
}

void Authentication::addUser(const std::string& username, const std::string& password, Role role) {
    users.emplace_back(username, password, role);
    std::ofstream file("users.txt", std::ios::app);
    if (file.is_open()) {
        file << username << " " << password << " " << static_cast<int>(role) << "\n";
        file.close();
    }
}
