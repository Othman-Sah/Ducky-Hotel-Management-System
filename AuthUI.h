#ifndef AUTH_UI_H
#define AUTH_UI_H

#include "Authentication.h"
#include "User.h"
#include "RetroUI.h"
#include <string>

namespace AuthUI {
    inline User* showLoginScreen(Authentication& auth) {
        std::string username, password;
        User* loggedInUser = nullptr;

        while (loggedInUser == nullptr) {
            RetroUI::drawLayout("SYSTEME AUTHENTIFICATION");
            username = RetroUI::textInput("Nom d'utilisateur", 12);
            password = RetroUI::textInput("Mot de passe     ", 14);

            loggedInUser = auth.login(username, password);

            if (loggedInUser == nullptr) {
                RetroUI::drawInfoBox({"Identifiants incorrects. Veuillez reessayer."});
                system("pause");
            }
        }
        return loggedInUser;
    }
}
#endif