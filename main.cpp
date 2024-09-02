#include <iostream>
#include <string>
#include <memory>
#include "SqliteDatabaseHandler.h"

using namespace std;

void createUser();  
void createGame();  
void buyGame();  
void listUserGames();  
void listGameUsers();  
void listAllJoins();  

void showMenu(); 

int main() {
    int option;
    do {
        showMenu();
        cin >> option;

        switch (option) {
            case 1:
                createUser();
                break;
            case 2:
                createGame();
                break;
            case 3:
                buyGame();
                break;
            case 4:
                listUserGames();
                break;
            case 5:
                listGameUsers();
                break;
            case 6:
                listAllJoins();
                break;
            case 0:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid option. Please try again.\n";
        }
    } while (option != 0);

    return 0;
}

void showMenu() {
    cout << "===== MENU =====\n";
    cout << "1. Create User\n";  
    cout << "2. Create Game\n";  
    cout << "3. Buy Game\n";  
    cout << "4. List User's Games\n";  
    cout << "5. List Game's Users\n";  
    cout << "6. List All Joins\n";  
    cout << "0. Exit\n";  
    cout << "Choose an option: ";
}

void createUser() {
    string username;

    cout << "Enter username: ";
    cin >> username;

    // Instanciar el manejador de la base de datos
    auto dbHandler = make_unique<SqliteDatabaseHandler>("path/to/database.db");

    // Conectar a la base de datos
    dbHandler->setConfFile("configuration.ini");

    // Preparar la consulta SQL para insertar un nuevo usuario
    dbHandler->prepareQuery("INSERT INTO Users (username) VALUES (?)");
    dbHandler->addParameter(1, username);
    dbHandler->execute();

    if (dbHandler->getErrorStatus()) {
        cout << "Error creating user.\n";
    } else {
        cout << "User created successfully!\n";
    }
}

void createGame() {
    string gameTitle;

    cout << "Enter game title: ";
    cin >> gameTitle;

    // Instanciar el manejador de la base de datos
    auto dbHandler = make_unique<SqliteDatabaseHandler>("path/to/database.db");

    // Conectar a la base de datos
    dbHandler->setConfFile("configuration.ini");

    // Preparar la consulta SQL para insertar un nuevo juego
    dbHandler->prepareQuery("INSERT INTO Games (title) VALUES (?)");
    dbHandler->addParameter(1, gameTitle);
    dbHandler->execute();

    if (dbHandler->getErrorStatus()) {
        cout << "Error creating game.\n";
    } else {
        cout << "Game created successfully!\n";
    }
}

void buyGame() {
    string username;
    string gameTitle;

    cout << "Enter username: ";
    cin >> username;
    cout << "Enter game title: ";
    cin >> gameTitle;

    auto dbHandler = make_unique<SqliteDatabaseHandler>("path/to/database.db");
    dbHandler->setConfFile("configuration.ini");

    dbHandler->prepareQuery("INSERT INTO UserGames (username, gameTitle) VALUES (?, ?)");
    dbHandler->addParameter(1, username);
    dbHandler->addParameter(2, gameTitle);
    dbHandler->execute();

    if (dbHandler->getErrorStatus()) {
        cout << "Error buying game.\n";
    } else {
        cout << "Game bought successfully!\n";
    }
}

void listUserGames() {
    string username;

    cout << "Enter username: ";
    cin >> username;

    auto dbHandler = make_unique<SqliteDatabaseHandler>("path/to/database.db");
    dbHandler->setConfFile("configuration.ini");

    dbHandler->prepareQuery("SELECT Games.title FROM Games "
                           "JOIN UserGames ON Games.title = UserGames.gameTitle "
                           "WHERE UserGames.username = ?");
    dbHandler->addParameter(1, username);

    auto table = dbHandler->fetchAll();
    if (dbHandler->getErrorStatus()) {
        cout << "Error retrieving user's games.\n";
    } else {
        cout << "Games for user " << username << ":\n";
        for (const auto& row : table) {
            cout << "- " << row.at("title") << endl;
        }
    }
}

void listGameUsers() {
    string gameTitle;

    cout << "Enter game title: ";
    cin >> gameTitle;

    auto dbHandler = make_unique<SqliteDatabaseHandler>("path/to/database.db");
    dbHandler->setConfFile("configuration.ini");

    dbHandler->prepareQuery("SELECT Users.username FROM Users "
                           "JOIN UserGames ON Users.username = UserGames.username "
                           "WHERE UserGames.gameTitle = ?");
    dbHandler->addParameter(1, gameTitle);

    auto table = dbHandler->fetchAll();
    if (dbHandler->getErrorStatus()) {
        cout << "Error retrieving users of the game.\n";
    } else {
        cout << "Users who own the game " << gameTitle << ":\n";
        for (const auto& row : table) {
            cout << "- " << row.at("username") << endl;
        }
    }
}

void listAllJoins() {
    auto dbHandler = make_unique<SqliteDatabaseHandler>("path/to/database.db");
    dbHandler->setConfFile("configuration.ini");

    dbHandler->prepareQuery("SELECT Users.username, Games.title FROM UserGames "
                           "JOIN Users ON UserGames.username = Users.username "
                           "JOIN Games ON UserGames.gameTitle = Games.title");

    auto table = dbHandler->fetchAll();
    if (dbHandler->getErrorStatus()) {
        cout << "Error retrieving all joins.\n";
    } else {
        cout << "All joins (User - Game):\n";
        for (const auto& row : table) {
            cout << "User: " << row.at("username") << ", Game: " << row.at("title") << endl;
        }
    }
}
