#include "gameScores.h"
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

DatabaseManager::DatabaseManager(const std::string &dbFile)
{
    std::string dbFolder = "database";
    std::string dbPath = dbFolder + "/" + dbFile;

    mkdir(dbFolder.c_str(), 0777);

    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK)
    {
        std::cerr << "Cannot open database. Error: " <<
        sqlite3_errmsg(db) << std::endl;
    }
    else
    {
        std::cout << "Database created at: " << dbPath << std::endl;
        std::string createTableSQL = 
        "CREATE TABLE IF NOT EXISTS scores ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "player_name TEXT NOT NULL, "
        "score INTEGER NOT NULL, "
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");";

        executeSQL(createTableSQL);
    }
    
}

DatabaseManager::~DatabaseManager()
{
}
