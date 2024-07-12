//main.cpp
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <DatabaseHandler.h>
#include <SqliteDatabaseHandler.h>
#include <IniReader.h>

using namespace std;

// Función para verificar si un usuario existe por su nombre de usuario
bool userExistsByUsername(const std::string& username, DatabaseHandler* dbh) {
    dbh->prepareQuery("SELECT COUNT(*) AS count FROM Users WHERE username=?");
    dbh->addParameter(1, username);
    Row result = dbh->fetch();
    int count = std::stoi(result["count"]);
    return count > 0;
}

// Función para buscar juegos por usuario
void findGamesByUser(SqliteDatabaseHandler& dbh, const std::string& username) {
    dbh.prepareQuery("SELECT Games.name AS game_name FROM Users JOIN UserGames ON Users.username = UserGames.user_id JOIN Games ON UserGames.game_id = Games.id WHERE Users.username = ?");
    dbh.addParameter(1, username);
    Table gamesTable = dbh.fetchAll();

    if (gamesTable.empty()) {
        std::cout << "No games found for user " << username << std::endl;
    } else {
        std::cout << "Games for user " << username << ":" << std::endl;
        for (const auto& row : gamesTable) {
            std::cout << "- " << row.at("game_name") << std::endl;
        }
    }
}

// Función para buscar usuarios por juego
void findUsersByGame(SqliteDatabaseHandler& dbh, const std::string& gameName) {
    dbh.prepareQuery("SELECT Users.username, Users.mail FROM Users JOIN UserGames ON Users.username = UserGames.user_id JOIN Games ON UserGames.game_id = Games.id WHERE Games.name = ?");
    dbh.addParameter(1, gameName);
    Table usersTable = dbh.fetchAll();

    if (usersTable.empty()) {
        std::cout << "No users found for game " << gameName << std::endl;
    } else {
        std::cout << "Users playing game " << gameName << ":" << std::endl;
        for (const auto& row : usersTable) {
            std::cout << "- Username: " << row.at("username") << ", Mail: " << row.at("mail") << std::endl;
        }
    }
}

int main() {
    // Inicializar el manejador de base de datos:
    auto dbh = make_unique<SqliteDatabaseHandler>();
    dbh->setConfFile("configuration.ini");
    
    // Crear tabla con users
    dbh->prepareQuery("CREATE TABLE IF NOT EXISTS Users (username TEXT PRIMARY KEY, name TEXT NOT NULL, mail TEXT UNIQUE NOT NULL)");
    dbh->execute();

    // Crear tabla de juegos
    dbh->prepareQuery("CREATE TABLE IF NOT EXISTS Games (id INTEGER PRIMARY KEY, name TEXT NOT NULL)");
    dbh->execute();

    // Creación de tabla UserGames
    dbh->prepareQuery("CREATE TABLE IF NOT EXISTS UserGames (user_id TEXT, game_id INTEGER, FOREIGN KEY (user_id) REFERENCES Users(username), FOREIGN KEY (game_id) REFERENCES Games(id))");
    dbh->execute();

    // Insertar datos iniciales en Users
    dbh->prepareQuery("INSERT INTO Users(username, name, mail) VALUES(?, ?, ?)");
    dbh->addParameter(1, "juanchii");
    dbh->addParameter(2, "Juan");
    dbh->addParameter(3, "juan@example.com");
    dbh->execute();

    dbh->prepareQuery("INSERT INTO Users(username, name, mail) VALUES(?, ?, ?)");
    dbh->addParameter(1, "pedrope");
    dbh->addParameter(2, "Pedro");
    dbh->addParameter(3, "pedro@example.com");
    dbh->execute();

    // Insertar datos iniciales en Games
    dbh->prepareQuery("INSERT INTO Games(name) VALUES(?)");
    dbh->addParameter(1, "The sims");
    dbh->execute();

    dbh->prepareQuery("INSERT INTO Games(name) VALUES(?)");
    dbh->addParameter(1, "Butterflies");
    dbh->execute();

    // Insertar datos en UserGames (asociaciones usuario - juego)
    dbh->prepareQuery("INSERT INTO UserGames(user_id, game_id) VALUES(?, ?)");
    dbh->addParameter(1, "juanchii");  // Aquí debes usar el username correcto de tu tabla Users
    dbh->addParameter(2, "1");      // Aquí el ID del juego que deseas asociar al usuario
    dbh->execute();

    dbh->prepareQuery("INSERT INTO UserGames(user_id, game_id) VALUES(?, ?)");
    dbh->addParameter(1, "pedrope");  // Aquí debes usar el username correcto de tu tabla Users
    dbh->addParameter(2, "2");      // Aquí el ID del juego que deseas asociar al usuario
    dbh->execute();

    // Consulta para mostrar qué juegos tiene cada usuario usando JOIN
    findGamesByUser(*dbh, "juanchii");

    // Consulta para mostrar qué usuarios tienen un juego específico usando JOIN
    findUsersByGame(*dbh, "Butterflies");

    return 0;
}
