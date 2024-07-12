// main.cpp

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <DatabaseHandler.h>
#include <SqliteDatabaseHandler.h>
#include <IniReader.h>

using namespace std;

int main()
{
    // Inicializar el manejador de base de datos:
    auto dbh = make_unique<SqliteDatabaseHandler>();
    dbh->setConfFile("configuration.ini");
    
    // Crear tabla con nuevos atributos
    dbh->prepareQuery("CREATE TABLE IF NOT EXISTS Users (username TEXT PRIMARY KEY, name TEXT NOT NULL, mail TEXT UNIQUE NOT NULL)");
    dbh->execute();

    // Insertar datos iniciales
    dbh->prepareQuery("INSERT INTO Users(username, name, mail) VALUES(?, ?, ?)");
    dbh->addParameter(1, "user1");
    dbh->addParameter(2, "Juan");
    dbh->addParameter(3, "juan@example.com");
    dbh->execute();

    dbh->prepareQuery("INSERT INTO Users(username, name, mail) VALUES(?, ?, ?)");
    dbh->addParameter(1, "user2");
    dbh->addParameter(2, "Pedro");
    dbh->addParameter(3, "pedro@example.com");
    dbh->execute();

    // Leer y mostrar datos
    dbh->prepareQuery("SELECT * FROM Users WHERE username=?");
    dbh->addParameter(1, "user1");
    Row row = dbh->fetch();

    std::cout << "ROW DATA user1" << std::endl;
    std::cout << "username: " << row["username"] << std::endl;
    std::cout << "name: " << row["name"] << std::endl;
    std::cout << "mail: " << row["mail"] << std::endl << std::endl;

    dbh->prepareQuery("SELECT * FROM Users WHERE username=?");
    dbh->addParameter(1, "user2");
    row = dbh->fetch();

    std::cout << "ROW DATA user2" << std::endl;
    std::cout << "username: " << row["username"] << std::endl;
    std::cout << "name: " << row["name"] << std::endl;
    std::cout << "mail: " << row["mail"] << std::endl << std::endl;

    // Actualizar datos
    std::cout << "Updating name of user with username 'user1' to 'Carlos' and mail to 'carlos@example.com'" << std::endl;
    dbh->prepareQuery("UPDATE Users SET name=?, mail=? WHERE username=?");
    dbh->addParameter(1, "Carlos");
    dbh->addParameter(2, "carlos@example.com");
    dbh->addParameter(3, "user1");
    dbh->execute();

    dbh->prepareQuery("SELECT * FROM Users WHERE username=?");
    dbh->addParameter(1, "user1");
    row = dbh->fetch();

    std::cout << "Updated ROW DATA user1" << std::endl;
    std::cout << "username: " << row["username"] << std::endl;
    std::cout << "name: " << row["name"] << std::endl;
    std::cout << "mail: " << row["mail"] << std::endl << std::endl;

    // Eliminar datos
    std::cout << "Deleting user with username 'user2'" << std::endl;
    dbh->prepareQuery("DELETE FROM Users WHERE username=?");
    dbh->addParameter(1, "user2");
    dbh->execute();

    dbh->prepareQuery("SELECT * FROM Users");
    Table table = dbh->fetchAll();

    std::cout << "Table DATA:" << std::endl;
    for (const auto& row : table) {
        std::cout << "username: " << row.at("username") << ", ";
        std::cout << "name: " << row.at("name") << ", ";
        std::cout << "mail: " << row.at("mail") << std::endl;
    }

    return 0;
}
