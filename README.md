# Système de Gestion d'Hôtel

Une application console C++ simple pour gérer les opérations d'hôtel incluant la réservation, la libération et l'affichage des informations des chambres.

## Fonctionnalités
- Afficher toutes les chambres avec leur statut
- Afficher uniquement les chambres disponibles
- Réserver une chambre pour un client
- Libérer une chambre
- Rechercher les détails d'une chambre spécifique
- Pré-configuré avec 7 chambres de différents types

## Types de Chambres
- Simple: 500 DH/nuit
- Double: 800 DH/nuit
- Suite: 1500 DH/nuit
- Deluxe: 2000 DH/nuit

## Compilation

### Using g++:
```bash
g++ -o hotel main.cpp Room.cpp Hotel.cpp
```

### Using MSVC (Visual Studio):
```bash
cl /EHsc main.cpp Room.cpp Hotel.cpp
```

## Running the Program
```bash
./hotel
```
or on Windows:
```bash
hotel.exe
```

## Usage
Follow the menu prompts to:
1. View all rooms
2. View available rooms
3. Book a room (enter room number and guest name)
4. Check out (enter room number)
5. Search for a specific room
6. Exit the program
