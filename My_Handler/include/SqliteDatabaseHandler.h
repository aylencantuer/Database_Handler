// SqliteDatabaseHandler.h

#ifndef SQLITE_DATABASE_HANDLER_H
#define SQLITE_DATABASE_HANDLER_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sqlite3.h>
#include <DatabaseHandler.h>
#include <IniReader.h>

class SqliteDatabaseHandler : public DatabaseHandler
{
    private:
        sqlite3* pDB;
        sqlite3_stmt* stmt;
        bool errorStatus;
        Row row;
        Table table;
        std::string columnName;
        std::string columnValue;
        int columnCount;
        std::shared_ptr<IniReader> iniReader;

        void openDb(std::string dbPath)
        {
            errorStatus = false;

            int iReturn = sqlite3_open(dbPath.c_str(), &pDB);
            if (iReturn != SQLITE_OK) 
            {  
                errorStatus = true;
                std::cerr << "Error open DB " << sqlite3_errmsg(pDB) << std::endl; 
                exit(-1);
            }
            stmt = nullptr;
        }

    public:
        SqliteDatabaseHandler()
        {
            this->iniReader = std::make_shared<IniReader>();
        }
        
        SqliteDatabaseHandler(std::string dbPath)
        {
            this->openDb(dbPath);
        }

        ~SqliteDatabaseHandler()
        {
            if (stmt) {
                sqlite3_finalize(stmt);
            }
            sqlite3_close(pDB);
        }

        void setConfFile(std::string confFilePath)
        {
            iniReader->open("configuration.ini");
            std::string dbPath = iniReader->selectSection("SQLITE")->getValue("path");
            this->openDb(dbPath);         
        }

        void prepareQuery( std::string query )
        {
            int iReturn = sqlite3_prepare_v2(pDB, query.c_str(), query.size() + 1, &stmt, 0);
            if (iReturn != SQLITE_OK) { 
                errorStatus = true; 
                std::cerr << "Error preparing query: " << sqlite3_errmsg(pDB) << std::endl;
                sqlite3_close(pDB); 
                exit(-1); 
            }
        }

        void addParameter( int index, std::string value )
        {
            int iReturn = sqlite3_bind_text(stmt, index, value.c_str(), value.size(), 0);
            if (iReturn != SQLITE_OK) { 
                errorStatus = true; 
                std::cerr << "Error binding parameter: " << sqlite3_errmsg(pDB) << std::endl;
                exit(-1); 
            }
        }

        void execute()
        {
            if (sqlite3_step(stmt) != SQLITE_DONE) {
                std::cerr << "Error executing statement: " << sqlite3_errmsg(pDB) << std::endl;
                errorStatus = true;
            }
            sqlite3_reset(stmt);
        }

        Row fetch()
        {
            row.clear();
            int step = sqlite3_step(stmt);
            if (step == SQLITE_ROW) {
                columnCount = sqlite3_column_count(stmt);
                for( int i = 0; i < columnCount; i++ )
                {
                    columnName = std::string( (char *)sqlite3_column_name(stmt, i) );
                    try
                    {
                        columnValue = std::string( (char *)sqlite3_column_text( stmt, i ) );
                    }
                    catch(const std::exception& e)
                    {
                        columnValue = "";
                    }
                    
                    row[columnName] = columnValue;           
                }   
            } else if (step != SQLITE_DONE) {
                std::cerr << "Error fetching row: " << sqlite3_errmsg(pDB) << std::endl;
                errorStatus = true;
            }
            sqlite3_reset(stmt);
            return row; 
        }

        Table fetchAll()
        {
            table.clear();
            row.clear();
            columnCount = sqlite3_column_count(stmt);
            while(sqlite3_step(stmt)==SQLITE_ROW)
            {
                row.clear();
                for( int i = 0; i < columnCount; i++ )
                {
                    columnName = std::string( (char *)sqlite3_column_name(stmt, i) );
                    columnValue = std::string( (char *)sqlite3_column_text( stmt, i ) );
                    row[columnName] = columnValue;
                } 
                table.push_back(row);
            }

            if (sqlite3_errcode(pDB) != SQLITE_DONE && sqlite3_errcode(pDB) != SQLITE_ROW) {
                std::cerr << "Error fetching all rows: " << sqlite3_errmsg(pDB) << std::endl;
                errorStatus = true;
            }
            sqlite3_reset(stmt);
            return table;
        }

        bool getErrorStatus()
        { 
            return this->errorStatus; 
        } 

        void update(const std::string& username, const std::string& name, const std::string& mail)
        {
            prepareQuery("UPDATE Users SET name=?, mail=? WHERE username=?");
            addParameter(1, name);
            addParameter(2, mail);
            addParameter(3, username);
            execute();
        }

        void remove(const std::string& username)
        {
            prepareQuery("DELETE FROM Users WHERE username=?");
            addParameter(1, username);
            execute();
        }
};

#endif // SQLITE_DATABASE_HANDLER_H
